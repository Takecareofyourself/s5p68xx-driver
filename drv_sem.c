/*
  1,建立一个字符驱动
  2，利用信号量的机制保护字符驱动临界资源
      防止竟态的产生
*/

#include<linux/init.h>
#include<linux/module.h>

MODULE_LICENSE("GPL");

dev_t dev;
struct cdev btn_cdev;
struct class *cls=NULL;
struct semaphore btn_sema;

int btn_open(struct inode *inode, struct file *file)
{
    /*使用信号量做保护，当设备被打开时候则，其他进程或则线程不能打开设备*/
    down(&btn_sema);//特点：获取到后需要up释放，获取不到则睡眠等待直到获取到，不可以被中断。就是函数睡眠不会完成
    down_interruptable(&btn_sema);//特点：获取不到则睡眠等待，但是可以被中断
    down_trylock(&btn_sema);//特点：获取到需要up释放，获取到与不到，函数都会完成，0获取成功
    return 0;
}
int btn_close(struct inode *inode, struct file *file)
{
    up(&btn_sema);
    return 0;
}

struct file_operations btn_ops =
{
    .owner = THIS_MODULE,
    .open = btn_open,
    .release = btn_close,
};

int __init drv_sema_init(void)
{
    /*注册设备号*/
    alloc_chrdev_region(&dev,1,1,"K1");
    /*实例化cdev*/
    cdev_init(&btn_cdev,&btn_ops);
    cdev_add(&cdev,dev,1);
    /*挂载设备*/
    cls=class_create(THIS_MODULE,"button");
    device_create(cls,NULL,dev,NULL,"btnk1");
    /*初始化信号量*/
    sema_init(&btn_sema,1);
    return 0;
}

void __exit drv_sema_exit(void)
{
    /*注销设备的挂载*/
    device_destroy(cls,dev);
    class_destroy(cls);
    /*注销cdev*/
    cdev_del(&btn_cdev);
    /*注销设备号*/
    unregister_chrdev_region(dev,1);
}

module_init(drv_sema_init);
module_exit(drv_sema_exit);

