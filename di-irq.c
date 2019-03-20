#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

pthread_mutex_t mutex;
int fd = 0;

unsigned char Tm_Floor=0;
unsigned char uptimestart=0;
unsigned char downtimestart = 0;
/*中断管脚依次是45,46,47,48,49*/
//转态值 上升下降、有无人、开关门、顶底
unsigned char floor = 1;
unsigned char floorMax = 20;
//unsigned char personState = 0;	//0:无人 	1：有人
//unsigned char doorState;		//0:关门	1：开关
unsigned char liftState;		//0:停止	1：运行    2:卡层	3:超速
unsigned char upOrDown;			//0:先上	1：先下
unsigned char upOrDownState;	//0:上升状态	1：下降转态  3:停止
unsigned char stopDoorNum = 0;
unsigned char topOrBottom;     	//0：正常转态	1：顶	2：底

unsigned char get_value;

unsigned char timeSleep(unsigned ss,unsigned int ms) 
{
	struct timeval tv;

	tv.tv_sec = ss;

	tv.tv_usec = ms*1000;
	return select(0, NULL, NULL, NULL, &tv);
}

void *Floor_Stop(void *ptr)
{
	unsigned char dat;
	struct timeval start,end,old;
	int interval;
	//	pthread_mutex_lock(&mutex);
	//timeSleep(3,0);
	read(fd,&dat,1);
	//timeSleep(1,500);
	//read(fd,&dat,1);
	printf("get thread dat=%d\n",dat);
	if(dat == 0)
		gettimeofday(&start,NULL);
	
	while(1){
		gettimeofday(&end,NULL);
		interval = (end.tv_sec-start.tv_sec) + (end.tv_usec-start.tv_usec)/1000000;
		read(fd,&dat,1);
		if(dat == 0){
			if(interval >= 10)
			{
				upOrDownState = 3;
				printf("the floor level is stop\n");
				break;
			}
		}else{
			
			if(upOrDownState == 0){
				//if(interval <= 1)
				//	break;
				floor++;
				printf("the floor up is %d\n",floor);
			}else{
				//if(interval <= 1)
				//	break;
				floor--;
				printf("the floor down is %d\n",floor);
			}
			break;
		}
		//printf("get while dat=%d\n",dat);
	}
	pthread_detach(pthread_self());
	printf("thread back\n");
	pthread_exit( NULL); //线程退出
}

int main(int argc,char *argv[])
{	
	int error = 0;
	unsigned char value = 0; 
	//int fd = 0;
	//fd_set fdset;
	//struct timeval tv;
	//struct timeval start,end,old;
	
//	pthread_t thread_up_time,thread_down_time,thread_send;
//	pthread_mutex_init(&mutex,NULL);
	pthread_t thread_stop_status;

	pthread_mutex_init(&mutex,NULL);
	fd = open("/dev/di",O_RDWR);

	if ( fd < 0 ){
		perror("open di device failed");
		return 0;
	}
	
	/*tv.tv_sec = 0;					///< frame timeout
    	tv.tv_usec = 300000;//response_time_out* 10000;
	FD_ZERO(&fdset);
	FD_SET( fd, &fdset);*/
/*	error = pthread_create(&thread_up_time, NULL, Floor_Up_Tm, NULL);
	if(error != 0)
	{
		printf("the up pthread is error!\n");

		return 0;//exit(1);
	}
	error = pthread_create(&thread_down_time, NULL, Floor_Down_Tm, NULL);
	if(error != 0)
	{
		printf("the down pthread is error!\n");

		return 0;//exit(1);
	}
*/	
	//error = pthread_create(&thread_send, NULL, Send_Alarm, NULL);
	if(error != 0)
	{
		printf("the down pthread is error!\n");

		return 0;//exit(1);
	}
	
	while(1)
	{
		//select( fd+ 1, &fdset, NULL, NULL, &tv);
//		select( fd+ 1, &fdset, NULL, NULL, NULL);
		//select( fd+ 1, &fdset, NULL, NULL, 0);
//		if (FD_ISSET(fd,&fdset))
//		{
		//pthread_mutex_lock(&mutex);
		//get_value = ioctl(fd,0xa00);
		value = ioctl(fd,0xa00);
		//value = ioctl(fd,0xa00);
		//value = get_value;
		//pthread_mutex_unlock(&mutex);
		printf("the value = 0x%x\n",value);
			//read(fd,&rd_val,1);
			
		//	switch(value) {
		//		case 0x1e:   //上升状态
		if(value == 1){
			//printf("the floor level is up\n");
			upOrDownState = 0;
			//timeSleep(0,400);
		}
		if(value==2){
			//printf("the floor level is down\n");
			upOrDownState = 1;
			//timeSleep(0,400);
		}
		if(value==0){
			//printf("the floor level is stop\n");
			error = pthread_create(&thread_stop_status, NULL, Floor_Stop, NULL);
			if(error != 0)
			{
				printf("the down pthread is error!\n");
				return 0;//exit(1);
			}
			pthread_join(thread_stop_status,NULL);
		}
	}

	close(fd);
	return 0;
}
