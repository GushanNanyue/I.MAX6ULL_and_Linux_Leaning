#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/io.h>

#define LED_MAjOR 200 //主设备号
#define LED_NAME "led"

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

#define LEDOFF 0 //关闭
#define LEDON 1

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


static int led_open(struct inode* inode,struct file *filp){
	
	return 0;

}

static int led_close(struct inode* inode, struct file* filp){

	return 0;
}


static int led_write(struct file* filp, const char __user *buf, size_t count, loff_t *ppos){

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
static const struct file_operations led_fops = {
	.owner = THIS_MODULE,
	.write = led_write,
	.open = led_open,
	.release = led_close,
};

//入口函数
static int __init led_init(void){

	int ret = 0;
	unsigned int val = 0;
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


	//注册字符设备
	ret = register_chrdev(LED_MAjOR,LED_NAME,&led_fops);
	if (ret < 0)
	{
		printk("register chardev failed! \r\n");
		return -EIO;
	}
	


	printk("led_init\r\n");
	return 0;
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


	unregister_chrdev(LED_MAjOR,LED_NAME);
	printk("led_exit\r\n");
}

//注册驱动的入口和出口函数
module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CL");
