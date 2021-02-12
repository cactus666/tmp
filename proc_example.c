#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/uaccess.h>

#define MAX_VALUES 1000
#define BUFSIZE 1000

MODULE_LICENSE("WTFPL");
MODULE_AUTHOR("Dmitrii Medvedev");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.1");

static struct proc_dir_entry* entry;
static unsigned int* values;
static int values_idx;

static ssize_t proc_write(struct file *file, const char __user * ubuf, size_t count, loff_t* ppos) 
{
	printk(KERN_DEBUG "Attempt to write proc file");
	char buf[BUFSIZE];

	if (*ppos > 0 || count > BUFSIZE)
		return -EFAULT;
	if (copy_from_user(buf, ubuf, count))
		return -EFAULT;

	int num = sscanf(buf, "%c");

	if (values_idx >= MAX_VALUES)
		return -EFAULT; 

	values[values_idx++] = num;
	int c = strlen(buf);
	*ppos = c;
	return c;
}

static ssize_t proc_read(struct file *file, char __user * ubuf, size_t count, loff_t* ppos) 
{
	char str[80];
	int len = sizeof(str);
	sprintf(str, "Hello user: %d", values_idx);
	if (*ppos > 0 || count < len)
	{
		return 0;
	}
	if (copy_to_user(ubuf, str, len) != 0)
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
	entry = proc_create("proc_example", 0666, NULL, &fops);
	values = kzalloc(MAX_VALUES * sizeof(unsigned int), GFP_KERNEL);
	values_idx = 0;

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

