#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/parport.h>
#include <linux/pci.h>
#include <linux/version.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("max ");
MODULE_DESCRIPTION("\"Calculator\" minimal module");

#define HAS_PROC
#define HAS_SYS

#define SIZE 100

char buf_str[SIZE];
char num_one_str[SIZE];
char num_two_str[SIZE];
char operation_str[SIZE];
char result_str[SIZE];

int res_read(void)
{
	int num_one = 20;
	int num_two = 30;
	int result;
	int len;
	
	printk("%s ", num_one_str);
	printk("%s ", operation_str);
	printk("%s ", num_two_str);
	sscanf(num_one_str, "%d", &num_one);
	sscanf(num_two_str, "%d", &num_two);
	if (operation_str[0] == '+')
		result = num_one + num_two;
	else if (operation_str[0] == '-')
		result = num_one - num_two;
	else if (operation_str[0] == '*')
		result = num_one * num_two;
	else if (operation_str[0] == '/' || operation_str[0] == ':'){
		if (num_two == 0){
			printk("Error:division by zero \n");
			return 0;
		}
		result = num_one / num_two;
	}
	else 
	  return 0;
	printk("= %d\n",result);
	sprintf(result_str, "%d\n", result);
	len = strlen(result_str);
	return len;
}

#ifdef HAS_PROC

static struct proc_dir_entry *Num_One;
static struct proc_dir_entry *Num_Two;
static struct proc_dir_entry *Operation;
static struct proc_dir_entry *Result;

int read_simbol(char *buffer, char **buffer_location,
	      off_t offset, int buffer_length, int *eof, void *data)
{
	int len = strlen(buf_str);
	if (buffer_length < len)
		return -EINVAL;
	if (offset > 0) 
		return 0;
	printk("%s\n", buf_str);
	strcpy(buffer,buf_str);
	*eof = 1;
	return len;
}

int result_read(char *buffer, char **buffer_location,
	      off_t offset, int buffer_length, int *eof, void *data)
{
	int len = res_read();
	
	if (buffer_length < len)
		return -EINVAL;
	if (offset > 0)
		return 0;
	strcpy(buffer, result_str);
	*eof = 1;
	return len;
}

int write(char *str, const char *buffer, unsigned long count)
{
	int len = count;
	if (copy_from_user(num_one_str, buffer, len))
		return -EFAULT;
	return len;
}

int num_one_write(struct file *file, const char *buffer, unsigned long count,
		   void *data)
{
// 	int len = count;
// 	if (copy_from_user(num_one_str, buffer, len))
// 		return -EFAULT;
	return write(num_one_str, buffer, count);
}

int num_two_write(struct file *file, const char *buffer, unsigned long count,
		   void *data)
{
// 	int len = count;
// 	if (copy_from_user(num_two_str, buffer, len))
// 		return -EFAULT;
	return write(num_two_str, buffer, count);
}

int operation_write(struct file *file, const char *buffer, unsigned long count,
		   void *data)
{
// 	int len = count;
// 	if (copy_from_user(operation_str, buffer, len))
// 		return -EFAULT;
	return write(operation_str, buffer, count);
}

int result_write(struct file *file, const char *buffer, unsigned long count,
		   void *data)
{
// 	int len = count;
// 	if (copy_from_user(result_str, buffer, len))
// 		return -EFAULT;
	return write(result_str, buffer, count);
}
#endif

#ifdef HAS_SYS
static ssize_t show(struct class *class, struct class_attribute *attr, char *buf )
{
	int len;
	strcpy(buf, buf_str);
	len = strlen(buf);
	printk("read %d\n", len);
	return len;
}

static ssize_t result_show(struct class *class, struct class_attribute *attr, char *buf )
{
	int len = res_read();
	strcpy(buf, result_str);
	len = strlen(buf);
	printk("read %d\n", len);
	return len;
}

size_t store(char *str, const char *buf, size_t count)
{
	int len = count;
	printk("write %d\n", len);
	strncpy(str, buf, len);
	str[len] = '\0';
	return len;
}

static ssize_t result_store(struct class *class, struct class_attribute *attr, const char *buf, size_t count )
{
	return store(result_str, buf, count);
}

static ssize_t num_one_store(struct class *class, struct class_attribute *attr, const char *buf, size_t count )
{
	return store(num_one_str, buf, count);
}

static ssize_t num_two_store(struct class *class, struct class_attribute *attr, const char *buf, size_t count )
{
	return store(num_two_str, buf, count);
}

static ssize_t operation_store(struct class *class, struct class_attribute *attr, const char *buf, size_t count )
{
	return store(operation_str, buf, count);
}

struct class_attribute class_attr_result = __ATTR( result, 0666, &result_show, &result_store);
struct class_attribute class_attr_num_one = __ATTR( num_one, 0666, &show, &num_one_store);
struct class_attribute class_attr_num_two = __ATTR( num_two, 0666, &show, &num_two_store);
struct class_attribute class_attr_operation = __ATTR( operation, 0666, &show, &operation_store);

static struct class *calculator_class;

#endif


int init_module(void)
{
	int res;
#ifdef HAS_PROC
	Num_One = create_proc_entry("num_one", 0666, NULL);
	if (Num_One == NULL) {
		remove_proc_entry("num_one", NULL);
		printk(KERN_ALERT "Error: Could not initialize /proc/num_one\n");
		return -ENOMEM;
	}
	strcpy(buf_str, num_one_str);
	Num_One->read_proc  = read_simbol;
	Num_One->write_proc = num_one_write;
	Num_One->mode 	  = S_IFREG | S_IRUGO;
	Num_One->uid 	  = 0;
	Num_One->gid 	  = 0;
	Num_One->size 	  = 37;
	
	Num_Two = create_proc_entry("num_two", 0666, NULL);
	if (Num_Two == NULL) {
		remove_proc_entry("num_two", NULL);
		printk(KERN_ALERT "Error: Could not initialize /proc/num_two\n");
		return -ENOMEM;
	}
	strcpy(buf_str, num_two_str);
	Num_Two->read_proc  = read_simbol;
	Num_Two->write_proc = num_two_write;
	Num_Two->mode 	  = S_IFREG | S_IRUGO;
	Num_Two->uid 	  = 0;
	Num_Two->gid 	  = 0;
	Num_Two->size 	  = 37;
	
	Operation = create_proc_entry("operation", 0666, NULL);
	if (Operation == NULL) {
		remove_proc_entry("operation", NULL);
		printk(KERN_ALERT "Error: Could not initialize /proc/operation\n");
		return -ENOMEM;
	}
	strcpy(buf_str, operation_str);
	Operation->read_proc  = read_simbol;
	Operation->write_proc = operation_write;
	Operation->mode   = S_IFREG | S_IRUGO;
	Operation->uid 	  = 0;
	Operation->gid 	  = 0;
	Operation->size   = 37;
	
	Result = create_proc_entry("result", 0666, NULL);
	if (Result == NULL) {
		remove_proc_entry("result", NULL);
		printk(KERN_ALERT "Error: Could not initialize /proc/result\n");
		return -ENOMEM;
	}
	Result->read_proc  = result_read;
	Result->write_proc = result_write;
	Result->mode 	  = S_IFREG | S_IRUGO; 
	Result->uid 	  = 0;
	Result->gid 	  = 0;
	Result->size 	  = 37;
	
#endif
#ifdef HAS_SYS
	calculator_class = class_create(THIS_MODULE, "calculator_class");
	if (IS_ERR(calculator_class))
		printk("error class create\n");
	res = class_create_file(calculator_class, &class_attr_num_one);
	res = class_create_file(calculator_class, &class_attr_num_two);
	res = class_create_file(calculator_class, &class_attr_operation);
	res = class_create_file(calculator_class, &class_attr_result);
	printk("'calculator' module initialized\n");
#endif
	return 0;
}

void cleanup_module(void)
{
#ifdef HAS_PROC
	remove_proc_entry("num_one", NULL);
	remove_proc_entry("num_two", NULL);
	remove_proc_entry("operation", NULL);
	remove_proc_entry("result", NULL);
#endif

#ifdef HAS_SYS
	class_remove_file(calculator_class, &class_attr_num_one);
	class_remove_file(calculator_class, &class_attr_num_two);
	class_remove_file(calculator_class, &class_attr_operation);
	class_remove_file(calculator_class, &class_attr_result);
	class_destroy(calculator_class);
#endif
	return;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("max ");
MODULE_DESCRIPTION("\"Calculator\" minimal module");
MODULE_VERSION("sys_proc");