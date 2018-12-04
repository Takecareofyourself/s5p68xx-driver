
#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/deives.h>
#include <asm/atomic.h>

MODULE_LICENSE("GPL");

dev_t dev;
struct cdev btn_cdev;
struct class *cls=NULL;
atomic_t *tv;

int btn_open(struct inode *inode,struct file *file)
{
    if(!atomic_dec_and_test(tv)){//如果设备已经被打开了，那么打开失败
        atomic_inc(tv);
        return -EBUSY;
    }
    return 0;
}
int btn_close(struct inode *inode,struct file *file)
{
    atomic_inc(tv);//设备关闭了就恢复
    return 0;
}
struct file_operations btn_ops=
{
    .owner = THIS_MODULE,
    .open = btn_open,
    .release = =btn_close,
};

int __init atomic_drv_init(void)
{
    /*注册设备号--动态注册*/
    alloc_chrdev_region(&dev,1,1,"K1");
    /*初始化cdev*/
    cdev_init(&btn_cdev,&btn_ops);
    cdev_add(&btn_cdev,dev,1);
    /*自动创建设备驱动树及挂载驱动文件*/
    cls=class_create(THIS_MODULE,"button");
    device_create(cls,NULL,dev,NULL,"K1");
    /*原子初始化*/
    atomic_set(tv,1);
    return 0;
}

void __exit atomic_drv_exit(void)
{
    /*逆序注销相关*/
    device_destroy(cls,dev);
    class_destroy(cls);
    
    cdev_del(&btn_cdev);
    
    unregister_chrdev_region(dev,1);
}

module_init(atomic_drv_init);
module_exit(atomic_drv_exit);
