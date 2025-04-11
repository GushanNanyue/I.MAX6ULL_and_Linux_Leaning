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

#define GPIODEV_COUNT 1
#define GPIODEV_NAME "beep"
#define BEEPON 1
#define BEEPOFF 0

//设备结构体
struct gpio_dev{
	dev_t devid; //设备号
	int major;	//主设备号
	int minor;	//次设备号

	struct cdev cdev;

	struct class* class;
	struct device* device;

	struct device_node* nd;
	int beep_gpio;

};
/*
 * @description		: 打开设备
 * @param - inode 	: 传递给驱动的inode
 * @param - filp 	: 设备文件，file结构体有个叫做private_data的成员变量
 * 					  一般在open的时候将private_data指向设备结构体。
 * @return 			: 0 成功;其他 失败
 */
static int beep_open(struct inode *inode, struct file *filp)
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
static ssize_t beep_read(struct file *filp, char __user *buf, size_t cnt, loff_t *offt)
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
static ssize_t beep_write(struct file *filp, const char __user *buf, size_t cnt, loff_t *offt)
{
	int retvalue;
	unsigned char databuf[1];
	unsigned char beepstat;
	struct gpio_dev *dev = filp->private_data;

	retvalue = copy_from_user(databuf, buf, cnt);
	if(retvalue < 0) {
		printk("kernel write failed!\r\n");
		return -EFAULT;
	}

	beepstat = databuf[0];		/* 获取状态值 */

	if(beepstat == BEEPON) {	
		gpio_set_value(dev->beep_gpio, 0);	/* 打开蜂鸣器 */
	} else if(beepstat == BEEPOFF) {
		gpio_set_value(dev->beep_gpio, 1);	/* 关闭蜂鸣器 */
	}
	
	return 0;
	
}

/*
 * @description		: 关闭/释放设备
 * @param - filp 	: 要关闭的设备文件(文件描述符)
 * @return 			: 0 成功;其他 失败
 */
static int beep_release(struct inode *inode, struct file *filp)
{
	return 0;
}

//设备操作集合
static const struct file_operations beep_fops = {
		.owner = THIS_MODULE,
		.open = beep_open,
		.release = beep_release,
		.read = beep_read,
		.write = beep_write,
};




struct gpio_dev beep;

//驱动入口函数
static int __init beep_init(void){

	int ret = 0;
	//注册字符设备驱动
	beep.devid = 0;
	if (beep.devid)
	{
		beep.devid = MKDEV(beep.major,0);
		ret = register_chrdev_region(beep.major,GPIODEV_COUNT,GPIODEV_NAME);
	}else
	{
		ret = alloc_chrdev_region(&beep.devid,0,GPIODEV_COUNT,GPIODEV_NAME);
		beep.major = MAJOR(beep.devid);
		beep.minor = MINOR(beep.devid);
	}

	if (ret < 0)
	{
		goto fail_devid;
	}
	

	//初始化cdev
	beep.cdev.owner = THIS_MODULE;
	cdev_init(&beep.cdev,&beep_fops);

	//添加cdev
	ret = cdev_add(&beep.cdev,beep.devid,GPIODEV_COUNT);
	if (ret < 0)
	{
		goto fail_addcdev;
	}
	
	
	//创建类
	beep.class = class_create(THIS_MODULE,GPIODEV_NAME);
	if (IS_ERR(beep.class))
	{
		ret = PTR_ERR(beep.class);
		goto fial_createclass;
	}
	
	//创建设备
	beep.device = device_create(beep.class,NULL,beep.devid,NULL,GPIODEV_NAME);
	if (IS_ERR(beep.device))
	{
		ret = PTR_ERR(beep.device);
		goto fail_createdevice;
	}

	//初始化beep
	//获取设备树节点
	beep.nd = of_find_node_by_path("/beep");
	if (beep.nd == NULL)
	{
		ret = -EINVAL;
		goto fail_nd;
	}

	//获取gpio编号
	beep.beep_gpio = of_get_named_gpio(beep.nd,"beep-gpios",0);
	if (beep.beep_gpio < 0)
	{
		ret = -EINVAL;
		goto fail_nd;
	}

	//申请gpio
	ret = gpio_request(beep.beep_gpio,"beep-gpio");
	if (ret)
	{
		printk("Can't request beep gpio\r\n");
	}
	
	//操作输出
	ret = gpio_direction_output(beep.beep_gpio,0);
	if (ret < 0)
	{
		goto fail_set;
	}
	
	gpio_set_value(beep.beep_gpio,0);
	

	

	return ret;
fail_set:
	gpio_free(beep.beep_gpio);
fail_nd:
	device_destroy(beep.class,beep.devid);
fail_createdevice:
	class_destroy(beep.class);
fial_createclass:
	cdev_del(&beep.cdev);
fail_addcdev:
	unregister_chrdev_region(beep.devid,GPIODEV_COUNT);
fail_devid:
	return ret;

}

//驱动出口函数
static void __exit beep_exit(void){

	//关闭蜂鸣器
	gpio_set_value(beep.beep_gpio,1);

	//注销字符设备驱动
	cdev_del(&beep.cdev);
	unregister_chrdev_region(beep.devid,GPIODEV_COUNT);

	device_destroy(beep.class,beep.devid);
	class_destroy(beep.class);

	gpio_free(beep.beep_gpio);

	
}

//注册和卸载驱动

module_init(beep_init);
module_exit(beep_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CL");