#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

void menu();

int main()
{
	char input;
	int file = open("/dev/my_device", O_RDWR);
    // printf("%d\n", file);

	while(1)
    {
        menu();
        scanf(" %c", &input);
        getchar();
        if(input == 'r' || input == 'w' || input == 's' || input == 'e')
        {
            if(input == 'r')
            {
                int readData, i;
                printf("Enter the number of bytes you want to read:"); 
                scanf("%d", &readData); 
                char *allocMem = (char *)malloc(readData);
                read(file, allocMem, readData); 
                printf("Data read from the device:"); 
                for(i = 0; i < readData; i++)
                {
                    printf(" %c", *(allocMem + i));
                } 
                // printf("INPUT R MANE\n");
            }
            if(input == 'w')
            {
                int bufferSize;
                bufferSize = 1024;// 1024 bc buffer size mane
                char writeData[bufferSize];
                int lengthInput;
                printf("Enter data you want to write to the device:");
                // scanf("%s", writeData);
                fgets(writeData, bufferSize, stdin);
                lengthInput = strlen(writeData);
                write(file, writeData, lengthInput);
                // printf("INPUT W MANE\n");
            }
            if(input == 's')
            {
                int offset, whence;
                printf("Enter an offset value: ");
                scanf("%d", &offset);
                printf("Enter a value for whence: (0, 1, 2)");
                scanf("%d", &whence);
                lseek(file, offset, whence);
                // printf("INPUT S MANE\n");
            }
            if(input == 'e')
            {
                printf("\nQuitting ...\n");
                break;
            }
        }
        else 
        {
            printf("Invalid input!");
        }
	}
	close(file);
	return 0;
}

void menu()
{
    printf("\nPress 'r' to read from the device\n");
    printf("Press 'w' to write to the device\n");
    printf("Press 's' to seek into the device\n");
    printf("Press 'e' to exit from the device\n");
    printf("Press anything else to keep reading or writing from the device\n");
    printf("Enter command:");
}