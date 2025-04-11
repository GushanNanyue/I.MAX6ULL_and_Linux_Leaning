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
#include <linux/ide.h>
#include <asm/mach/map.h>
#include <asm/uaccess.h>

//设备名
#define NEWCHRLED_NAME "newchrled"
#define NEWCHRLED_COUNT 1
#define LEDOFF 0 //关闭
#define LEDON 1

//寄存器物理地址
#define CCM_CCGR1_BASE			(0x020c406c)
#define SW_MUX_GPIO1_IO03_BASE 	(0x020e0068)
#define SW_PAD_GPIO1_IO03_BASE	(0x020e02f4)
#define GPIO1_DR_BASE			(0x0209c000)
#define GPIO1_GDIR_BASE			(0x0209c004)

//地址映射后的虚拟地址指针
static void __iomem* IMX6U_CCM_CCGR1;
static void __iomem* SW_MUX_GPIO1_IO03;
static void __iomem* SW_PAD_GPIO1_IO03;
static void __iomem* GPIO1_DR;
static void __iomem* GPIO1_GDIR;


//LED设备结构体
struct newchrled_dev
{
	struct cdev cdev; 		//字符设备
	dev_t devid; 			//设备号
	int major;				//主设备号
	int minor;				//次设备号
	struct class* class; 	//设备节点类
	struct device* device;	//设备类
};

struct newchrled_dev newchrled;		//led设备


//led灯打开或关闭
void led_switch(u8 sta){
	u32 val = 0;
	if (sta == LEDON)
	{
		val = readl(GPIO1_DR);//开灯
		val &= ~(1 << 3);	
		writel(val,GPIO1_DR); 
	}else if (sta == LEDOFF)
	{
		val = readl(GPIO1_DR);
		val |= (1 << 3);	//关灯
		writel(val,GPIO1_DR);
	}
	
}


static int newchrled_open(struct inode* inode,struct file *filp){

	filp->private_data = &newchrled;
	
	return 0;

}

static int newchrled_close(struct inode* inode, struct file* filp){

	struct newchrled_dev* dev = (struct newchrled_dev*)filp->private_data;
	return 0;
}


static int newchrled_write(struct file* filp, const char __user *buf, size_t count, loff_t *ppos){

	int ret;
	unsigned char databuf[1];


	ret = copy_from_user(databuf,buf,count);
	if (ret < 0 )
	{
		printk("kernel write failed!\r\n");
		return -EFAULT;
	}
	//判断是开灯还是关灯
	led_switch(databuf[0]);

	return 0;

}


//字符设备操作集
static const struct file_operations newchrled_fops = {
	.owner 		= THIS_MODULE,
	.write 		= newchrled_write,
	.open 		= newchrled_open,
	.release 	= newchrled_close,
};

//入口函数
static int __init led_init(void){

	int ret = 0;
	unsigned int val = 0;

	printk("newchrled_init\r\n");
	

	//初始化led灯,地址映射
	IMX6U_CCM_CCGR1 	= ioremap(CCM_CCGR1_BASE,4);
	SW_MUX_GPIO1_IO03	= ioremap(SW_MUX_GPIO1_IO03_BASE,4);
	SW_PAD_GPIO1_IO03	= ioremap(SW_PAD_GPIO1_IO03_BASE,4);
	GPIO1_DR			= ioremap(GPIO1_DR_BASE,4);		
	GPIO1_GDIR			= ioremap(GPIO1_GDIR_BASE,4);
	
	//初始化
	val = readl(IMX6U_CCM_CCGR1);
	val &= ~(3 << 26); //清除以前的配置26-27位
	val |= 3 << 26;	//将26-27位置1
	writel(val,IMX6U_CCM_CCGR1); //写入

	writel(0x5,SW_MUX_GPIO1_IO03); //设置复用
	writel(0x10b0,SW_PAD_GPIO1_IO03); //设置电气属性

	val = readl(GPIO1_GDIR);
	val |= 1 << 3;	
	writel(val,GPIO1_GDIR); //写入

	val = readl(GPIO1_DR);//默认打开灯
	val &= ~(1 << 3);	
	writel(val,GPIO1_DR); //写入

	newchrled.major = 0; //设置为0表示由系统申请设备号

	//注册字符设备条件判断
	if(newchrled.major){
		//给定主设备号
		newchrled.devid = MKDEV(newchrled.major,0);
		ret = register_chrdev_region(newchrled.devid,NEWCHRLED_COUNT,NEWCHRLED_NAME);

	}else{
		//未给定主设备号
		ret = alloc_chrdev_region(&newchrled.devid,0,NEWCHRLED_COUNT,NEWCHRLED_NAME);
		newchrled.major = MAJOR(newchrled.devid);
		newchrled.minor = MINOR(newchrled.devid);
	}

	if (ret < 0)
	{
		printk("newchrled chrdev_region err!\r\n");
		goto fail_devid;
	}
	
	printk("newchrled major = %d , minor = %d \r\n",newchrled.major,newchrled.minor);

	//字符设备注册
	newchrled.cdev.owner = THIS_MODULE;
	cdev_init(&newchrled.cdev,&newchrled_fops);
	ret = cdev_add(&newchrled.cdev,newchrled.devid,NEWCHRLED_COUNT);
	if (ret < 0)
	{
		goto fail_cdev;
	}
	
	
	//自动创建设备节点
	newchrled.class = class_create(THIS_MODULE, NEWCHRLED_NAME);
	if (IS_ERR(newchrled.class)) {

		ret = PTR_ERR(newchrled.class);
		goto fail_class;

	}
	
	//创建设备
	newchrled.device = device_create(newchrled.class, NULL, newchrled.devid, NULL, NEWCHRLED_NAME);
	if (IS_ERR(newchrled.device)) {

		ret =  PTR_ERR(newchrled.device);
		goto fail_device;
	}

	return 0;


fail_device:
	class_destroy(newchrled.class);
fail_class:
	cdev_del(&newchrled.cdev);
fail_cdev:
	unregister_chrdev_region(newchrled.devid,NEWCHRLED_COUNT);
fail_devid:
	return ret;



}

//出口函数
static void __exit led_exit(void){

	unsigned int val = 0;
	val = readl(GPIO1_DR);
	val |= (1 << 3);	//关灯
	writel(val,GPIO1_DR);

	//取消地址映射
	iounmap(IMX6U_CCM_CCGR1);
	iounmap(SW_MUX_GPIO1_IO03);
	iounmap(SW_PAD_GPIO1_IO03);
	iounmap(GPIO1_DR);
	iounmap(GPIO1_GDIR);


	//删除字符设备
	cdev_del(&newchrled.cdev);
	
	//注销设备号
	unregister_chrdev_region(newchrled.devid,NEWCHRLED_COUNT);

	//摧毁设备
	device_destroy(newchrled.class,newchrled.devid);

	//删除设备节点
	class_destroy(newchrled.class);


	printk("newchrled_exit\r\n");
}

//注册驱动的入口和出口函数
module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CL");
