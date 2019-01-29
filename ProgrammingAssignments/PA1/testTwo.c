#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h> 
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	if(argc == 3)
	{
		int temp = 0;
	    int* ptr = &temp;
		int res = syscall(334, atoi(argv[1]), atoi(argv[2]), ptr);
		printf("%d + %d = %d\n", atoi(argv[1]), atoi(argv[2]), *ptr);
	    	
		return res;
	}
	else
	{
		printf("Invalid input! \n");
        	return -1;
	}
}