/******************************
 * 模块的例子1
 * 本例测试模块的加载和卸载
 * 如果在ubuntu下，需要sudo(个别人可能加载失败)；开发板上不用
 * $>modinfo mod_test01.ko	打印模块的信息
 * $>sudo insmod mod_test01.ko 加载模块(从磁盘拷贝到内存)
 * $>dmesg	打印内核信息(查看模块中调用的printk语句输出的信息)
 * $>lsmod 	显示内核已经加载了哪些模块
 * $>sudo rmmod mod_test01	卸载模块
 * Author: zht
 * Date: 2018-03-26
 *******************************/
//内核驱动不会调用用户态的头文件，只会包含内核源码下的头文件
#include <linux/module.h> //对应kernel/include/module.h
#include <linux/interrupt.h>
#include <linux/gpio.h>

static irqreturn_t myIntHandler(int irq, void *dev_id)
{
	printk("<0>Interrupt IN\n");
	return IRQ_HANDLED;
}

//模块是没有main的，因为不是可执行的程序
//模块有入口函数，在insmod时调用，完成驱动的初始化工作
static int __attribute__((__section__(".init.text"))) my_init(void)
{
	//printk默认是带有消息级别的，默认的消息基本为<4>
	//如果采用默认级别，需要用dmesg来查看消息
	//但如果将消息级别提升为<0>，则默认直接在控制台打印内核消息
	printk("<0>Hello kernel! 2018-03-26\n");
	int ret, irqno;
	int gpioNo = 113;
	ret = gpio_request(gpioNo, "mygpiopin");
	if(ret){
		printk("<0>irq pin request io failed.\n");
		return -1;
	}
 
	irqno= gpio_to_irq(gpioNo);
 
 
	ret = request_irq(irqno,myIntHandler , IRQF_TRIGGER_RISING|IRQF_TRIGGER_HIGH, "myinterrupt", NULL); 
	if(ret) {
		printk(KERN_ERR "<0>can not get irq\n");
		return ret;
	}
	printk("<0> ADD Interrupt kernel! 2018-03-26\n");
	return 0;	//如果初始化成功返回0，否则返回负值
}

//模块还有出口函数，在rmmod时调用，和入口中完成的工作相反
static void __exit my_exit(void)
{
	printk("<0>Farewell, I've to go. 2018-03-26\n");
}
MODULE_AUTHOR("ZHT");
MODULE_LICENSE("GPL");	//General Public License
module_init(my_init);
module_exit(my_exit);

