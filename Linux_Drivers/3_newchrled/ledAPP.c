#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>



/*
    argc:应用程序参数格式
    argv:具体的参数内容，字符串形式
    ./ledAPP <filename> <0 or 1> 0表示关灯 1表示开灯
*/
#define LEDOFF 0
#define LEDON 1


int main(int argc, char* argv[]){

    int ret = 0;
    int fd = 0;
    char* filename;
    unsigned char databuf[1];
    char writebuf[100];

    if (argc != 3)
        {
            printf("Error usage!\r\n");
            return -1;
        }


    filename = argv[1];
    fd = open(filename, O_RDWR);
    if (fd < 0)
    {
         printf("Can't open file %s\r\n", filename);
         return -1;
    }

    databuf[0] = atoi(argv[2]);
    ret = write(fd,databuf,sizeof(databuf));
    if (ret < 0)
    {
        printf("LED Control Failed!\r\n");
        close(fd);
        return -1;
    }

    close(fd);

    return 0;
}