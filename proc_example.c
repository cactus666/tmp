#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/uaccess.h>

MODULE_LICENSE("WTFPL");
MODULE_AUTHOR("Dmitrii Medvedev");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.1");

static struct proc_dir_entry* entry;

static ssize_t proc_write(struct file *file, const char __user * ubuf, size_t count, loff_t* ppos) 
{
	printk(KERN_DEBUG "Attempt to write proc file");
	return -1;
}

static ssize_t proc_read(struct file *file, char __user * ubuf, size_t count, loff_t* ppos) 
{
	size_t len = strlen(THIS_MODULE->name);
	if (*ppos > 0 || count < len)
	{
		return 0;
	}
	if (copy_to_user(ubuf, THIS_MODULE->name, len) != 0)
	{
		return -EFAULT;
	}
	*ppos = len;
	return len;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = proc_read,
	.write = proc_write,
};


static int __init proc_example_init(void)
{
	entry = proc_create("proc_example", 0444, NULL, &fops);
	printk(KERN_INFO "%s: proc file is created\n", THIS_MODULE->name);
	return 0;
}

static void __exit proc_example_exit(void)
{
	proc_remove(entry);
	printk(KERN_INFO "%s: proc file is deleted\n", THIS_MODULE->name);
}

module_init(proc_example_init);
module_exit(proc_example_exit);

