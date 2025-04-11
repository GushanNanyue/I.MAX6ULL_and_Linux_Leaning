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

#define DTSLED_COUNT 1 //设备个数
#define DTSLED_NAME "dtsled" //设备名字
//设置开关LED
#define LEDON 1
#define LEDOFF 0

/* 映射后的寄存器虚拟地址指针 */
static void __iomem *IMX6U_CCM_CCGR1;
static void __iomem *SW_MUX_GPIO1_IO03;
static void __iomem *SW_PAD_GPIO1_IO03;
static void __iomem *GPIO1_DR;
static void __iomem *GPIO1_GDIR;


//led设备结构体
struct dtsled_dev
{
	dev_t devid; //设备号
	int major; //主设备号
	int minor; //次设备号

	struct cdev cdev; //字符设备

	struct class *class; //类
	struct device *device; //设备

	struct device_node *nd; //设备节点
	
};

struct dtsled_dev dtsled; //具体led设备

//设备操作函数
void led_switch(u8 sta)
{
	u32 val = 0;
	if(sta == LEDON) {
		val = readl(GPIO1_DR);
		val &= ~(1 << 3);	
		writel(val, GPIO1_DR);
	}else if(sta == LEDOFF) {
		val = readl(GPIO1_DR);
		val|= (1 << 3);	
		writel(val, GPIO1_DR);
	}	
}

static int led_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int led_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static ssize_t led_read(struct file *filp, char __user *buf, size_t cnt, loff_t *offt)
{
	return 0;
}

static ssize_t led_write(struct file *filp, const char __user *buf, size_t cnt, loff_t *offt)
{
	int ret;
	unsigned char databuf[1];
	unsigned char ledstat;
	ret = copy_from_user(databuf,buf,cnt);
	if (ret < 0)
	{
		return -EFAULT;
	}
	//判断是开灯还是关灯

	ledstat = databuf[0];

	if(ledstat == LEDON) {	
		led_switch(LEDON);		/* 打开LED灯 */
	} else if(ledstat == LEDOFF) {
		led_switch(LEDOFF);	/* 关闭LED灯 */
	}
	
	
	return 0;
}



//字符设备操作集
static const struct file_operations dtsled_fops = {
	.owner = THIS_MODULE,
	.open = led_open,
	.release = led_release,
	.write = led_write,
	.read = led_read,
};



//入口
static int __init dtsled_init(void){
	
	int ret = 0;
	const char *str;
	u32 val = 0;
	//注册字符设备
	//申请设备号
	dtsled.major = 0; //设备号由内核分配
	if (dtsled.major){
		dtsled.devid = MKDEV(dtsled.major,0);
		ret = register_chrdev_region(dtsled.devid,DTSLED_COUNT,DTSLED_NAME);
	}else{
		ret = alloc_chrdev_region(&dtsled.devid,0,DTSLED_COUNT,DTSLED_NAME);
		dtsled.major = MAJOR(dtsled.devid);
		dtsled.minor = MINOR(dtsled.minor);
	}
	if (ret < 0)
	{
		goto fail_devid;
	}
	//添加字符设备
	dtsled.cdev.owner = THIS_MODULE;
	cdev_init(&dtsled.cdev,&dtsled_fops);
	ret = cdev_add(&dtsled.cdev,dtsled.devid,DTSLED_COUNT);
	if (ret < 0)
	{
		goto fail_cdev;
	}
	
	//自动创建设备节点
	//创建设备类
	dtsled.class =  class_create(THIS_MODULE,DTSLED_NAME);
	if (IS_ERR(dtsled.class))
	{
		ret = PTR_ERR(dtsled.class);
		goto fail_class;
	}
	//创建设备
	dtsled.device = device_create(dtsled.class,NULL,dtsled.devid,NULL,DTSLED_NAME);
	if (IS_ERR(dtsled.device))
	{
		ret = PTR_ERR(dtsled.device);
		goto fail_device;
	}
	//获取设备树属性信息
	dtsled.nd = of_find_node_by_path("/alphaled");
	if (dtsled.nd == NULL)
	{
		ret = -EINVAL;
		goto fail_findnd;
	}
	//获取status
	ret = of_property_read_string(dtsled.nd,"status",&str);
	if (ret < 0)
	{
		goto fail_rs;
	}else{
		printk("status = %s \r\n",str);
	}
	//获取compatible
	ret = of_property_read_string(dtsled.nd,"compatible",&str);
	if (ret < 0)
	{
		goto fail_rs;
	}else{
		printk("compatible = %s \r\n",str);
	}

#if 0
	//获取reg
	ret = of_property_read_u32_array(dtsled.nd,"reg",regdata,10);
	if (ret < 0)
	{
		goto fail_rs;
	}else{
		printk("reg data:\r\n");
		for ( i = 0; i < 10; i++)
		{
			printk("%#x ",regdata[i]);
		}
		printk("\r\n");
		
	}

	//led初始化
	//内存映射
	IMX6U_CCM_CCGR1 = ioremap(regdata[0], regdata[1]);
	SW_MUX_GPIO1_IO03 = ioremap(regdata[2], regdata[3]);
  	SW_PAD_GPIO1_IO03 = ioremap(regdata[4], regdata[5]);
	GPIO1_DR = ioremap(regdata[6], regdata[7]);
	GPIO1_GDIR = ioremap(regdata[8], regdata[9]);
#endif
	IMX6U_CCM_CCGR1 = of_iomap(dtsled.nd,0);
	SW_MUX_GPIO1_IO03 = of_iomap(dtsled.nd,1);
  	SW_PAD_GPIO1_IO03 = of_iomap(dtsled.nd,2);
	GPIO1_DR = of_iomap(dtsled.nd,3);
	GPIO1_GDIR = of_iomap(dtsled.nd,4);


	/* 2、使能GPIO1时钟 */
	val = readl(IMX6U_CCM_CCGR1);
	val &= ~(3 << 26);	/* 清楚以前的设置 */
	val |= (3 << 26);	/* 设置新值 */
	writel(val, IMX6U_CCM_CCGR1);
	/* 3、设置GPIO1_IO03的复用功能，将其复用为
	 *    GPIO1_IO03，最后设置IO属性。
	 */
	writel(5, SW_MUX_GPIO1_IO03);
	
	/*寄存器SW_PAD_GPIO1_IO03设置IO属性
	 *bit 16:0 HYS关闭
	 *bit [15:14]: 00 默认下拉
     *bit [13]: 0 kepper功能
     *bit [12]: 1 pull/keeper使能
     *bit [11]: 0 关闭开路输出
     *bit [7:6]: 10 速度100Mhz
     *bit [5:3]: 110 R0/6驱动能力
     *bit [0]: 0 低转换率
	 */
	writel(0x10B0, SW_PAD_GPIO1_IO03);

	/* 4、设置GPIO1_IO03为输出功能 */
	val = readl(GPIO1_GDIR);
	val &= ~(1 << 3);	/* 清除以前的设置 */
	val |= (1 << 3);	/* 设置为输出 */
	writel(val, GPIO1_GDIR);

	/* 5、默认关闭LED */
	val = readl(GPIO1_DR);
	val |= (1 << 3);	
	writel(val, GPIO1_DR);

	return 0;

fail_rs:

fail_findnd:
	device_destroy(dtsled.class,dtsled.devid);
fail_device:
	class_destroy(dtsled.class);
fail_class:
	cdev_del(&dtsled.cdev);
fail_cdev:
	unregister_chrdev_region(dtsled.devid,DTSLED_COUNT);
fail_devid:
	return ret;

}

//出口
static void __exit disled_exit(void){
	
	u32 val = 0;
	val = readl(GPIO1_DR);
	val |= (1 << 3);	
	writel(val, GPIO1_DR);

	/* 取消映射 */
	iounmap(IMX6U_CCM_CCGR1);
	iounmap(SW_MUX_GPIO1_IO03);
	iounmap(SW_PAD_GPIO1_IO03);
	iounmap(GPIO1_DR);
	iounmap(GPIO1_GDIR);

	//删除字符设备
	cdev_del(&dtsled.cdev);
	//释放设备号
	unregister_chrdev_region(dtsled.devid,DTSLED_COUNT);
	//摧毁设备
	device_destroy(dtsled.class,dtsled.devid);
	//摧毁类
	class_destroy(dtsled.class);
}



//注册和卸载驱动
module_init(dtsled_init);
module_exit(disled_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CL");