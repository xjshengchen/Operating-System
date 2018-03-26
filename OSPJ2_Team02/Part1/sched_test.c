#define _GNU_SOURCE 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>
#include <pthread.h>
#include <time.h>

void *thread_create(void *data){
	int* n=(int *)data;
	int i, j;
	for(i=0;i<3;i++){
		printf("Thread %d is running\n",n);
		for(j=0;j<100000000;j++)
			;
	}
}
int main(int argc,char *argv[])
{
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(0,&mask);
	sched_setaffinity(getpid(),sizeof(mask),&mask);

	if(argc==2){
		struct sched_param param;
		param.sched_priority=99;
		sched_setscheduler(getpid(),SCHED_FIFO,&param);
	}

	pthread_t t[2];
	int i;
	for(i=0;i<2;i++){
		pthread_create(&t[i],NULL,thread_create,(void *)(i+1));
		printf("Thread %d was created\n",i+1);
	}
	for(i=0;i<2;i++)
		pthread_join(t[i],NULL);
	return 0;
}
