#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/uaccess.h>

asmlinkage long sys_cs3753_add(int numOne, int numTwo, int* result)
{
	int addition = numOne + numTwo;
	int *newResPtr = &addition;
	// *result = numOne + numTwo;
	
	copy_to_user(result, newResPtr, 4);
	printk(KERN_ALERT "Adding %d and %d equals %d\n", numOne, numTwo, addition);
	return 0;
}