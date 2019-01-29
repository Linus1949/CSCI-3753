# CSCI 3753, Programming Assignment 1

**Contact info**:
 - Satish Upadhyaya
 - saup0513@colorado.edu

**Files**:

- <b>add.c:</b> A new system call that I wrote given the directions. The system call is given two numbers (`int numOne, int numTwo`) and an address of where to store the results. Function prototype: `asmlinkage long sys_cs3753_add(int numOne, int numTwo, int* result)
`, Path: `/home/kernel/linux-hwe-4.15.0/arch/x86/kernel/add.c`
- <b>Makefile:</b> Makefile path: `/home/kernel/linux-hwe-4.15.0/arch/x86/kernel/Makefile`.
- <b>syscall_64.tbl:</b> System call number 333 is for the `helloworld` program and system call number 334 is for the `add` program. Path: `/home/kernel/linux-hwe-4.15.0/arch/x86/entry/entry/syscalls/syscall_64.tbl`
- <b>syscalls.h:</b> Header file for the system call function prototypes: `asmlinkage long sys_cs3753_add(int numOne, int numTwo, int* result)`. Path: `/home/kernel/linux-hwe-4.15.0/include/linux/syscalls.h`.
- <b>syslog:</b> This is where the `printk(KERN_ALERT)` function prints to log the messages. Command: `sudo tail /var/log/syslog`.
- <b>Source code for the test program:</b> `testTwo.c` tests the add function from the user space.

**Compiling the Kernel**:

Ensure that you have ccache first; Command: `sudo apt-get install ccache`

You should be in this directory to be able to compile -> `/home/kernel/linux-hwe-4.15.0`:

- `sudo make -j2 CC="ccache gcc` 
- `sudo make -j2 modules_install`
- `sudo make -j2 install`
- `sudo reboot`


**Testing The Program**:
- `gcc testTwo.c`
- `./a.out numOne numTwo`
- Result will be displayed in this format in the terminal: numOne + numTwo = result.
- Result will be displayed in this format in `/var/log/syslog`: Adding numOne and numTwo equals result

