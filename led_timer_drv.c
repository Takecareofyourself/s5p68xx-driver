
#include<linux/init.h>
#include<linux/module.h>
#include<linux/gpio.h>
#include<mach/platform.h>

MODULE_LICENSE("GPL");

struct timer_list timer_struct_t;

void led_function(unsigned long data)
{
    data=!data;
    gpio_set_value(PAD_GPIO_C+12,data);
    /*由于初始化中时间定时器在模块安装后只会调用一次*/
    /*让led_function多次调用，请讲诉实现原理*/
    mod_timer(&timer_struct_t,jiffies+1*HZ);
}

int __init led_timer_init(void)
{
    init_timer(&timer_struct_t);
    /*jiffies是记录系统启动以来中断的次数，因此利用这个特性，实现模块安装后计时1S*/
    timer_struct_t.expires = jiffies+1*HZ;
    timer_struct_t.function = led_function;
    timer_struct_t.data=1;
    
    add_timer(&timer_struct_t);
    
    gpio_request(PAD_GPIO_C+12,"LED");
    gpio_direction_output(PAD_GPIO_C+12,1);
    return 0;
}

void __exit led_timer_exit(void)
{
    /*模块卸载后删除定时器，防止内核崩溃*/
    del_timer(&timer_struct_t);
    /*释放gpio管脚*/
    gpio_free(PAD_GPIO_C+12);
}

module_init(led_timer_init);
module_exit(led_timer_exit);
