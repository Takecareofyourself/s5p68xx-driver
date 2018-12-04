/*1,create a charactor driver
  2,auto create the driver device file
  3,add a lock when use sharing resource*/

#include<linux/init.h>
#include<linux/module.h>

MODULE_LICENSE("GPL");

dev_t btn_dev;
struct cdev btn_cdev;
struct class *btn_cls;
spinlock_t btn_lock;
/*建立一个临界资源，练习使用*/
static int cnt=1;

int btn_open(struct inode *inode, struct file *file)
{
    spin_lock(&btn_lock);
    if((--cnt)!=0){//执行到这，说明程序获取锁成功，但是如果--cnt!=0成立，那么会造成死锁
        cnt++;
        spin_lock(&btn_lock);//为了防止死锁
        return -EBUSY;
    }
    spin_unlock(&btn_lock);
    return 0;
}

int btn_close(struct inode *inode, struct file *file)
{
    spin_lock(&btn_lock);
    cnt++;
    spin_unlock(&btn_lock);
    return 0;
}

struct file_operations btn_ops =
{
    .owner = THIS_MODULE,
    .open = btn_open,
    .release = btn_close,
};

int __init btn_drv_init(void)
{
    /*注册设备号*/
    alloc_chrdev_region(&btn_dev,1,1,"K1");
    /*初始化cdev*/
    cdev_init(&btn_cdev, &btn_ops);
    /*自动创建设备文件*/
    btn_cls=class_create(THIS_MODULE,"button");
    device_create(btn_cls,NULL,btn_dev,NULL,"BTNK1");
    
    /*初始化spinlock*/
    spin_lock_init(&btn_lock);
    return 0;
}

void __exit btn_drv_exit(void)
{
    /*注销设备文件*/
    device_destroy(btn_cls, btn_dev);
    class_destroy(btn_cls);
    /*注销设备号*/
    unregister_chrdev_region(btn_dev,1);
}

module_init(btn_drv_init);
module_exit(btn_drv_init);

