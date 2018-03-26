#include <linux/kernel.h>
#include <linux/linkage.h>

asmlinkage long sys_Show(void)
{
	printk("B04902023 Shih-Hsiang Cheng\n");
	printk("B04902027 Sheng Chen\n");
	return 0;
}
