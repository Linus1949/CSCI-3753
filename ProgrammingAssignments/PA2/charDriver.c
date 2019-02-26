// fs.h: Get the functions that are related to device driver coding
#include <linux/fs.h>
// uaccess.h: Get data from userspace to kernel and viceversa
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/module.h>
// slab.h: kmalloc() and kfree()
#include <linux/slab.h>

/*
Major number stuff:
/Documentation/admin-guide/devices.txt
https://github.com/torvalds/linux/blob/master/Documentation/admin-guide/devices.txt
240-254 block	LOCAL/EXPERIMENTAL USE
		Allocated for local/experimental use.  For devices not
		assigned official numbers, these ranges should be
		used in order to avoid conflicting with future assignments.
*/

#define DRIVER_AUTHOR "Satish Upadhyaya"
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_LICENSE("GPL and additional rights"); // GNU Public License v2 rights and more

#define SIZE_BUFF 1024 
#define MAJOR_NUMBER 240 
#define MY_DEVICE_NAME "my_device" 

// Global variables
int open = 0;
int close = 0;
char* buff_device;

/*
my_device_open and my_device_close:
struct inode *pinode: represents the actual physical file on the hard disk
struct file *pfile: the abstract open file that contains all the necessary 
                    file openrations in the file_operations struct

What the open function should do: print the number of times the device has been opened until now
What the close function should do: print the number of times the device has been closed until now
*/
// extern int simple_open(struct inode *inode, struct file *file);
int my_device_open(struct inode *pinode, struct file *pfile)
{
    open++;
    printk(KERN_ALERT "Device has been opened %d times until now \n", open);
    return 0;
}

// extern int dcache_dir_close(struct inode *, struct file *);
int my_device_close(struct inode *pinode, struct file *pfile)
{
    close++;
    printk(KERN_ALERT "Device has been closed %d times until now \n", close);
    return 0;
}

// extern ssize_t vfs_read(struct file *, char __user *, size_t, loff_t *);
ssize_t my_device_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
    // offset is the curr pos of the file manes
    // cpyToUser: dest->userspace, src->buff_device
    int to_read, totalBytes, totalRead;
	printk(KERN_ALERT "Inside %s function\n",__FUNCTION__);
	if (*offset >= SIZE_BUFF)
    {
		return 1;
	}

    totalBytes = SIZE_BUFF - (*offset);
    if (totalBytes > length)
    {
        to_read = length;
    }
    else
    {
        to_read = totalBytes;    
    }

    // to read, the data is in the kernel space, so it needs to be copied to the userspace
    totalRead = copy_to_user(buffer, *offset + buff_device, to_read);

    printk(KERN_ALERT "End of %s function\n", __FUNCTION__);
    return to_read - totalRead;
}

// extern ssize_t kernel_write(struct file *, const void *, size_t, loff_t *);
ssize_t my_device_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
    // pfile-> curr pos
    // cpyFrmUser: dest-> buff_device, src-> userspace
    int bytes_written, totalRead;
    bytes_written = 0;
    printk(KERN_ALERT "Inside %s function\n",__FUNCTION__);

    // handling edge cases here
    // too few or too much to write
	if((*offset >= SIZE_BUFF) || (length + *offset > SIZE_BUFF))
    {
		return 1;
	}
    // too much to write
    if(*offset < bytes_written)
    {
        *offset = bytes_written;
        if(length + *offset  > SIZE_BUFF){
            return 1;
        }
    }

    // to write, user writes in the userspace, so it needs to be copied from the userspace to the kernel space
    totalRead = copy_from_user(buff_device + *offset, buffer, length);
	bytes_written += length;
    printk(KERN_ALERT "End of %s function\n", __FUNCTION__);
	return length;
}


// extern loff_t default_llseek(struct file *file, loff_t offset, int whence);
loff_t my_device_llseek(struct file *pfile, loff_t offset, int whence)
{
    loff_t position;
    printk(KERN_ALERT "inside %s function\n",__FUNCTION__);
    position = 0;

    // 0 is SEEK_SET, 1 is SEEK_CUR, and 2 is SEEK_END
	switch(whence) 
    {
		case 0: 
			position = offset;
			break;
		case 1: 
			position = pfile->f_pos + offset;
			break;
		case 2: 
			position = SIZE_BUFF - offset;
			break;
        default:
            printk(KERN_ALERT "Whence is no good!");
            break;
	}

    // cant do less than 0 and more than the buffer size so to limit that 
    // if the position is < 0, then just set it to 0
    // and if the position is > SIZE_BUFF then set it to be SIZE_BUFF
	if(position < 0)
    {
		position = 0;
	}
	if(position > SIZE_BUFF)
    {
		position = SIZE_BUFF;
	}

	pfile->f_pos = position;
    printk(KERN_ALERT "NEW file position pointer: %d\n", position);

	return position;
}

// copied from fs.h
static const struct file_operations my_file_operations = {				
	.owner	 = THIS_MODULE,						
	.open	 = my_device_open,					
	.release = my_device_close,					
	.read	 = my_device_read,                    					
	.write	 = my_device_write,					
	.llseek	 = my_device_llseek,				
};

int my_device_init(void)
{
	printk(KERN_ALERT "inside %s function\n",__FUNCTION__);
    buff_device = kmalloc(SIZE_BUFF, GFP_KERNEL); // GFP_KERNEL: Allocate normal kernel ram. May sleep.
    // May sleep and swap to free memory. Only allowed in user context, but is the most reliable way to allocate memory.
    /* 
        register_chrdev() takes 3 parameters:
        the major number of the driver, the name of the driver
        and a pointer to the file operations structure
    */
    register_chrdev(MAJOR_NUMBER, MY_DEVICE_NAME, &my_file_operations);
	return 0;
}

void my_device_exit(void)
{
	printk(KERN_ALERT "inside %s function\n",__FUNCTION__);
    // unregister_chrdev() takes 2 parameters
    // the major number, and the name of the character driver
    kfree_link(buff_device);
    unregister_chrdev(MAJOR_NUMBER, MY_DEVICE_NAME);
}

module_init(my_device_init);
module_exit(my_device_exit);