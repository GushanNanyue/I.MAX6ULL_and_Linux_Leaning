#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>


static char usrdata[]= {"user data!"};
/*
    argc:应用程序参数格式
    argv:具体的参数内容，字符串形式
    ./chrdevbaseAPP <filename> <1 or 2> 1表示读 2表示写
*/

int main(int argc, char* argv[]){

    int ret = 0;
    int fd = 0;
    char* filename;
    char readbuf[100];
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

    
    if (atoi(argv[2]) == 1)
    {
        ret = read(fd,readbuf,50);
        if(ret < 0){
            printf("read file %s failed!\r\n",filename);
        }else{
            printf("APP read data: %s \r\n",readbuf);
        }
        
    }

    if (atoi(argv[2]) == 2)
    {
        memcpy(writebuf,usrdata,sizeof(usrdata));
        ret = write(fd,writebuf,50);
        if(ret < 0){
            printf("write file %s failed!\r\n",filename);
        }
        
    }
    


    

    ret = close(fd);
    if (ret < 0)
    {
        printf("cole file %s failed!\r\n",filename);
    }
    
    
    return 0;
}