#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

static struct proc_dir_entry *Num_One;
static struct proc_dir_entry *Num_Two;
static struct proc_dir_entry *Operation;
static struct proc_dir_entry *Result;

#define SIZE 100
char num_one_str[SIZE];
char num_two_str[SIZE];
char operation_str[SIZE];
char result_str[SIZE];

int num_one_read(char *buffer, char **buffer_location,
	      off_t offset, int buffer_length, int *eof, void *data)
{
	int len = strlen(num_one_str);
	if (buffer_length < len)
		return -EINVAL;
	if (offset > 0) 
	  return 0;
	strcpy(buffer, num_one_str);
	*eof = 1;
	return len;
}

int num_two_read(char *buffer, char **buffer_location,
	      off_t offset, int buffer_length, int *eof, void *data)
{
	int len = strlen(num_two_str);
	if (buffer_length < len)
		return -EINVAL;
	if (offset > 0) 
	  return 0;
	strcpy(buffer, num_two_str);
	*eof = 1;
	return len;
}

int operation_read(char *buffer, char **buffer_location,
	      off_t offset, int buffer_length, int *eof, void *data)
{
	int len = strlen(operation_str);
	if (buffer_length < len)
		return -EINVAL;
	if (offset > 0) 
	  return 0;
	strcpy(buffer, operation_str);
	*eof = 1;
	return len;
}

int result_read(char *buffer, char **buffer_location,
	      off_t offset, int buffer_length, int *eof, void *data)
{
	int num_one = 20;
	int num_two = 30;
	int result;
	int len;
	
// 	int sign;
// 	int i;
// 	int j;
// 	char c;
	/*
	for(i = 0, j = 0; num_one_str[i] >= '0' && num_one_str[i] <= '9'; i++)
		j = j * 10 + (num_one_str[i] - '0');
	num_one = j;
	for(i = 0, j = 0; num_two_str[i] >= '0' && num_two_str[i] <= '9'; i++)
		j = j * 10 + (num_two_str[i] - '0');
	num_two = j;*/
	printk("%s", num_one_str);
 	sscanf(num_one_str, "%d", &num_one);
	printk("num_one=%d", num_one);
	sscanf(num_two_str, "%d", &num_two);
	result = num_one + num_two;
	sprintf(result_str, "%d", result);
	/*if ((sign = result) < 0)  
		result = -result;
	i = 0;
	do {
		result_str[i++] = result % 10 + '0';
	} while ((result /= 10) > 0);
	if (sign < 0)
	    result_str[i++] = '-';
	result_str[i] = '\0';
	for (i = 0, j = strlen(result_str)-1; i < j; i++, j--) {
		c = result_str[i];
		result_str[i] = result_str[j];
		result_str[j] = c;
	}*/
	
	len = strlen(result_str);
	if (buffer_length < len)
		return -EINVAL;
	if (offset > 0)
		return 0;
	strcpy(buffer, result_str);
	*eof = 1;
	return len;
}


int num_one_write(struct file *file, const char *buffer, unsigned long count,
		   void *data)
{
	int len = count;
	if ( copy_from_user(num_one_str, buffer, len) ) {
		return -EFAULT;
	}
	return len;
}

int num_two_write(struct file *file, const char *buffer, unsigned long count,
		   void *data)
{
	int len = count;
	if ( copy_from_user(num_two_str, buffer, len) ) {
		return -EFAULT;
	}
	return len;
}

int operation_write(struct file *file, const char *buffer, unsigned long count,
		   void *data)
{
	int len = count;
	if ( copy_from_user(operation_str, buffer, len) ) {
		return -EFAULT;
	}
	return len;
}

int result_write(struct file *file, const char *buffer, unsigned long count,
		   void *data)
{
	int len = count;
	if (copy_from_user(result_str, buffer, len))
		return -EFAULT;
	return len;
}

int init_module()
{
	Num_One = create_proc_entry("num_one", 0666, NULL);
	if (Num_One == NULL) {
		remove_proc_entry("num_one", NULL);
		printk(KERN_ALERT "Error: Could not initialize /proc/num_one\n");
		return -ENOMEM;
	}
	Num_One->read_proc  = num_one_read;
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
	Num_Two->read_proc  = num_two_read;
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
	Operation->read_proc  = operation_read;
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
	
	return 0;
}

void cleanup_module()
{
	remove_proc_entry("num_one", NULL);
	remove_proc_entry("num_two", NULL);
	remove_proc_entry("operation", NULL);
	remove_proc_entry("result", NULL);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("max ");
MODULE_DESCRIPTION("\"Calculator\" minimal module");
MODULE_VERSION("proc");