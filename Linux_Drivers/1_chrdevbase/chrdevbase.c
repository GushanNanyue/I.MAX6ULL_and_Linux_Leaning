#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/uaccess.h>



#define CHRDEVBASE_MAJOR	200				/* 主设备号 */
#define CHRDEVBASE_NAME		"chrdevbase" 	/* 设备名     */

static char readbuf[100];
static char writebuf[100];
static char kerneldate[] = {"lonly mountain"};


static int chrdevbase_open(struct inode *inode, struct file *filp)
{
	// printk("chrdevbase open!\r\n");
	return 0;
}

static int chrdevbase_release(struct inode *inode, struct file *filp)
{
	// printk("chrdevbase release!\r\n");
	return 0;
}

static ssize_t chrdevbase_read(struct file *filp, char __user *buf, size_t cnt, loff_t *offt)
{
	//printk("chrdevbase read!\r\n");
	int ret = 0;
	memcpy(readbuf,kerneldate,sizeof(kerneldate));
	ret = copy_to_user(buf,readbuf,cnt);
	if(ret == 0){
		printk("kernel senddata ok!\r\n");
	}else{
		printk("kernel senddata failed!\r\n");
	}

	return 0;
}


static ssize_t chrdevbase_write(struct file *filp, const char __user *buf, size_t cnt, loff_t *offt)
{
	// printk("chrdevbase write!\r\n");
	int ret = 0;
	ret =  copy_from_user(writebuf,buf,cnt);
	if(ret == 0){
		printk("kerneel recevdata: %s \r\n",writebuf);
	}
	else
	{
		printk("kernel recevdata failed!\r\n");
	}
	
	return 0;
}




/*
 * 设备操作函数结构体
 */
static struct file_operations chrdevbase_fops = {
	.owner = THIS_MODULE,	
	.open = chrdevbase_open,
	.read = chrdevbase_read,
	.write = chrdevbase_write,
	.release = chrdevbase_release,
};


static int __init chrdevbase_init(void)
{
	int retvalue = 0;

	/* 注册字符设备驱动 */
	retvalue = register_chrdev(CHRDEVBASE_MAJOR, CHRDEVBASE_NAME, &chrdevbase_fops);
	if(retvalue < 0){
		printk("chrdevbase driver register failed\r\n");
	}
	printk("chrdevbase init!\r\n");
	return 0;
}


static void __exit chrdevbase_exit(void)
{
	/* 注销字符设备驱动 */
	unregister_chrdev(CHRDEVBASE_MAJOR, CHRDEVBASE_NAME);
	printk("chrdevbase exit!\r\n");
}

/* 
 * 将上面两个函数指定为驱动的入口和出口函数 
 */
module_init(chrdevbase_init);
module_exit(chrdevbase_exit);

/* 
 * LICENSE和作者信息
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("CL");