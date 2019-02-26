# CSCI 3753, Programming Assignment 2

**Contact info**:
 - Satish Upadhyaya
 - saup0513@colorado.edu

**Files**:

- <b>charDriver.c:</b> This file contains the code for character driver. Function prototypes are: 

  -`int my_device_open(struct inode *pinode, struct file *pfile)`
  
  -`int my_device_close(struct inode *pinode, struct file *pfile)`
  
  -`ssize_t my_device_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset)`
  
  -`ssize_t my_device_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)`
  
  -`loff_t my_device_llseek(struct file *pfile, loff_t offset, int whence)`
  
  -`int my_device_init(void)`
  
  -`void my_device_exit(void)`
  
- <b>test.c:</b> This file contains the code to test the LKM and functionalities of the LKM like read, write, and seek.
- <b>Makefile:</b> Contains line: `obj-m:=charDriver.o` that tells the compiler to create a module named `charDriver.o`

**To compile the LKM**:
- Make sure you are in the right directory (directory with the Makefile)
- `make -C /lib/modules/$(uname -r)/build M=$PWD modules` 

**To install or uninstall the module**:
- `sudo insmod charDriver.ko` 
- `sudo rmmod charDriver`

**To compile and run the test Program**:
- `gcc test.c`
- `./a.out`

**To check the result**:
- `dmesg`  or, 
- `sudo tail /var/log/syslog` 