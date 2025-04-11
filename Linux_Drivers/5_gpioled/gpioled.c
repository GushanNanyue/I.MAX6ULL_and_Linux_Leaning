#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/gpio.h>
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>

#define GPIOLED_COUNT 1
#define GPIOLED_NAME "gpioled"
#define LEDON 1
#define LEDOFF 0

//设备结构体
struct gpiole_dev{
	dev_t devid; //设备号
	int major;	//主设备号
	int minor;	//次设备号

	struct cdev cdev;

	struct class* class;
	struct device* device;

	struct device_node* nd;
	int led_gpio;

};
/*
 * @description		: 打开设备
 * @param - inode 	: 传递给驱动的inode
 * @param - filp 	: 设备文件，file结构体有个叫做private_data的成员变量
 * 					  一般在open的时候将private_data指向设备结构体。
 * @return 			: 0 成功;其他 失败
 */
static int led_open(struct inode *inode, struct file *filp)
{
	return 0;
}

/*
 * @description		: 从设备读取数据 
 * @param - filp 	: 要打开的设备文件(文件描述符)
 * @param - buf 	: 返回给用户空间的数据缓冲区
 * @param - cnt 	: 要读取的数据长度
 * @param - offt 	: 相对于文件首地址的偏移
 * @return 			: 读取的字节数，如果为负值，表示读取失败
 */
static ssize_t led_read(struct file *filp, char __user *buf, size_t cnt, loff_t *offt)
{
	return 0;
}

/*
 * @description		: 向设备写数据 
 * @param - filp 	: 设备文件，表示打开的文件描述符
 * @param - buf 	: 要写给设备写入的数据
 * @param - cnt 	: 要写入的数据长度
 * @param - offt 	: 相对于文件首地址的偏移
 * @return 			: 写入的字节数，如果为负值，表示写入失败
 */
static ssize_t led_write(struct file *filp, const char __user *buf, size_t cnt, loff_t *offt)
{
	int retvalue;
	unsigned char databuf[1];
	unsigned char ledstat;
	struct gpiole_dev *dev = filp->private_data;

	retvalue = copy_from_user(databuf, buf, cnt);
	if(retvalue < 0) {
		printk("kernel write failed!\r\n");
		return -EFAULT;
	}

	ledstat = databuf[0];		/* 获取状态值 */

	if(ledstat == LEDON) {	
		gpio_set_value(dev->led_gpio, 0);	/* 打开LED灯 */
	} else if(ledstat == LEDOFF) {
		gpio_set_value(dev->led_gpio, 1);	/* 关闭LED灯 */
	}
	return 0;
	
}

/*
 * @description		: 关闭/释放设备
 * @param - filp 	: 要关闭的设备文件(文件描述符)
 * @return 			: 0 成功;其他 失败
 */
static int led_release(struct inode *inode, struct file *filp)
{
	return 0;
}

//设备操作集合
static const struct file_operations gpioled_fops = {
		.owner = THIS_MODULE,
		.open = led_open,
		.release = led_release,
		.read = led_read,
		.write = led_write,
};




struct gpiole_dev gpioled;

//驱动入口函数
static int __init led_init(void){

	int ret = 0;
	//注册字符设备驱动
	gpioled.devid = 0;
	if (gpioled.devid)
	{
		gpioled.devid = MKDEV(gpioled.major,0);
		register_chrdev_region(gpioled.major,GPIOLED_COUNT,GPIOLED_NAME);
	}else
	{
		alloc_chrdev_region(&gpioled.devid,0,GPIOLED_COUNT,GPIOLED_NAME);
		gpioled.major = MAJOR(gpioled.devid);
		gpioled.minor = MINOR(gpioled.devid);
	}

	//初始化cdev
	gpioled.cdev.owner = THIS_MODULE;
	cdev_init(&gpioled.cdev,&gpioled_fops);

	//添加cdev
	cdev_add(&gpioled.cdev,gpioled.devid,GPIOLED_COUNT);
	
	//创建类
	gpioled.class = class_create(THIS_MODULE,GPIOLED_NAME);
	if (IS_ERR(gpioled.class))
	{
		return PTR_ERR(gpioled.class);
	}
	
	//创建设备
	gpioled.device = device_create(gpioled.class,NULL,gpioled.devid,NULL,GPIOLED_NAME);
	if (IS_ERR(gpioled.device))
	{
		return PTR_ERR(gpioled.device);
	}

	//获取设备节点
	gpioled.nd = of_find_node_by_path("/gpioled");
	if (gpioled.nd == NULL)
	{
		ret = -EINVAL;
		goto fail_findnode;
	}
	
	//获取led所对应的gpio
	gpioled.led_gpio = of_get_named_gpio(gpioled.nd,"led-gpios",0);
	if (gpioled.led_gpio < 0)
	{
		printk("can't find led gpio\r\n");
		ret = -EINVAL;
		goto fail_findnode;
	}

	//申请IO
	ret = gpio_request(gpioled.led_gpio,"led-gpios");
	if (ret)
	{
		printk("Failed to request the led gpio\r\n");
		ret = -EINVAL;
		goto fail_findnode;
	}
	
	//使用IO
	//设置输出
	ret = gpio_direction_output(gpioled.led_gpio,1);
	if (ret)
	{
		goto fail_setoutput;
	}
	//输出低电平电量led灯
	gpio_set_value(gpioled.led_gpio,0);

	return 0;
fail_setoutput:
	gpio_free(gpioled.led_gpio);
fail_findnode:
	return ret;
}

//驱动出口函数
static void __exit led_exit(void){

	//关灯
	gpio_set_value(gpioled.led_gpio,1);

	//注销字符设备驱动
	cdev_del(&gpioled.cdev);
	unregister_chrdev_region(gpioled.devid,GPIOLED_COUNT);

	device_destroy(gpioled.class,gpioled.devid);
	class_destroy(gpioled.class);

	//释放IO
	gpio_free(gpioled.led_gpio);
}

//注册和卸载驱动

module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CL");