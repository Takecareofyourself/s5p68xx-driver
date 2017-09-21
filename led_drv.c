

#include<linux/init.h>
#include<linux/module.h>
#include<linux/uaccess.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<mach/platform.h>

#define CMD_LED_ON 0x10001
#define CMD_LED_OFF 0x10002

MODULE_LICENSE("GPL");

dev_t led_dev;
int led_major=0;
int led-minor=0;
struct cdev led_cdev;
struct class *led_class=NULL;

int led_open(struct inode *inode,struct file *file)
{
	return 0;
}
int led_close(struct inode *inode,struct file *file)
{
	return 0;
}
long led_ioctl(struct *file,unsigned int cmd,unsigned long arg)
{
	/*实现用户空间哪个灯的开和关*/
	/*由于用户空间传递arg的形参时候是地址*/
	int k_index=0;
	copy_form_user(&k_index,&arg,sizeof(int));
	switch(k_index){
		case 1:
			if(cmd==CMD_LED_ON){
				/*设置led1的管脚为0*/
				gpio_set_value(PAD_GPIO_C+12,0);
			}
			else if(cmd==CMD_LED_OFF){
				/*设置led1的管脚为1*/
				gpio_set_value(PAD_GPIO_C+12,1);
			}
			break;
		case 2:
			if(cmd==CMD_LED_ON){
				/*设置led2的管脚为0*/
			}
			else if(cmd==CMD_LED_OFF){
				/*设置led2的管脚为1*/
			}
			break;
		case 3:
			if(cmd==CMD_LED_ON){
				/*设置led3的管脚为0*/
			}
			else if(cmd==CMD_LED_OFF){
				/*设置led3的管脚为1*/
			}
			break;
		case 4:
			if(cmd==CMD_LED_ON){
				/*设置led4的管脚为0*/
			}
			else if(cmd==CMD_LED_OFF){
				/*设置led4的管脚为1*/
			}
			break;
		default:
			return -EINVAL;
	}	
	return 0;
}

struct file_operations led_ops = 
{
	.owner=THIS_MODULE,
	.open=led_open,
	.release=led_close,
	.unlocked_ioctl=led_ioctl,
};

int __init led_drv_init(void)
{
	/*注册一个设备号*/
	if(major){//练习写法，major不为0就静态注册设备号
		led_dev=MKDEV(led_major,led_minor);
		register_chrdev_region(led_dev,1,"LED_DRV");
	}
	else{
		alloc_chrdev_region(&led_dev,1,1,"LED_DRV");
	}
	/*注册实例化cdev*/
	cdev_init(&led_cdev,&led_ops);
	cdev_add(&led_cdev,led_dev,1);
	/*申请GPIO管脚，并初始化GPIO管脚的模式*/
	gpio_request(PAD_GPIO_C+12,"LED1");
	gpio_direction_output(PAD_GPIO_C+1,1);
	/*注册class，注册device，实现设备的自动挂载*/
	class_create(THIS_MODULE,"led_device");//创建设备文件夹
	device_create(led_class,NULL,led_dev,"LED");//创建设备名字
	return 0;
}

void __exit led_drv_exit(void)
{
	/*注销device，注销class，节省资源*/
	device_destroy(led_class,led_dev);
	class_destroy(led_class);
	/*注销管脚，释放资源*/
	gpio_free(PAD_GPIO_C+12);
	/*注销cdev*/
	cdev_del(&led_cdev);
	/*注销设备号，减少占用，节省资源*/
	unregister_chrdev_region(led_dev,1);
}

module_init(led_drv_init);
module_exit(led_drv_exit);
