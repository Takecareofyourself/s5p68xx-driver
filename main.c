
#include<stdio.h>
#include<fcntl.h>
#include<sys/ioctl.h>

/*定义指令宏，控制开关的，不直接传递字符*/
#define CMD_LED_ON 0x10001
#define CMD_LED_OFF 0x10002


int main(int argc,char *argv[])
{
	int fd;
	int index=0;
	int cmd=0;
	fd=open("/dev/LED_DRV",O_RDWR);
	if(fd<0){
		perror(fd);
		return -1;
	}
	/*控制哪个led灯的开和关*/
	/*设计成 ./led 1 on  但数字1实际传递的是字符串*/
	index=strtoull(argv[1],NULL,10);
	if(index>0||index<5){
	
		/*获取第三个参数的值是什么*/
		if(strcmp(argv[2],"on")==0){
			/*设置指令cmd的值*/
			cmd=CMD_LED_ON;
		}
		else if(strcmp(argv[2],"off")==0){
			cmd=CMD_LED_OFF;
		}
		else {
			printf("Unkown Commad\n");
		}
	}
	else{
		printf("%d is not 0<%d<5",index,index);
	}
	/*传递指令给驱动模块*/
	ioctl(fd,cmd,&index);//传递&index是为了使index参数改变后能传递回来，此处没有该作用，只是练习
	printf("used is finished\n");
	close(fd);
	return 0;
}
