#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>   

#include "filesystem_filter.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rhydon");

static int driver_init(void)
{	
	printk(KERN_ALERT "hello...\n");
	initialize_filter("ext4_file_operations");
	return 0;
}

static void driver_exit(void)
{
	printk(KERN_WARNING "bye...\n");
}

module_init(driver_init);
module_exit(driver_exit);