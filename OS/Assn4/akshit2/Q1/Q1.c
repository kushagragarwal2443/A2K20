#define _POSIX_C_SOURCE 199309L //required for clock
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <limits.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <inttypes.h>
#include <math.h>
int * shareMem(size_t size){
	key_t mem_key = IPC_PRIVATE;
	int shm_id = shmget(mem_key, size, IPC_CREAT | 0666);
	return (int*)shmat(shm_id, NULL, 0);
}


int arr[1000000];
int brr[1000000];
int crr[1000000];
int  part(int *arr,int l,int r)
{ int i,j=l,t;
	for(i=l;i<r;i++)
	{
		if(arr[i]<arr[r])
		{	 t=arr[i];
			arr[i]=arr[j];
			arr[j]=t;
			j++;
		}
	}

	t=arr[j];
	arr[j]=arr[r];
	arr[r]=t;

	return j;

}
void quick(int *arr,int l,int r)
{ int n=r-l+1;
	if(n==1||n==0)
		return;	
	    if(n<=5){
        for(int i=l;i<r;i++)
        {
            int j=i+1;
            for(;j<=r;j++)
                if(arr[j]<arr[i] && j<=r)
                {
                    int temp = arr[i];
                    arr[i] = arr[j];
                    arr[j] = temp;
                }
        }
        return ;
    }
	int num=part(arr,l,r),i,j;
	quick(arr,l,num-1);
	quick(arr,num+1,r);

}
	void multiq(int *arr,int l,int r)
{       int n=r-l+1;
     	if(n==1||n==0)
	return;

	    if(n<=5){
        for(int i=l;i<r;i++)
        {
            int j=i+1;
            for(;j<=r;j++)
                if(arr[j]<arr[i] && j<=r)
                {
                    int temp = arr[i];
                    arr[i] = arr[j];
                    arr[j] = temp;
                }
        }
        return ;
    }


	int num=part(arr,l,r),i,j;
	int x=fork();
	if(x==0)
	{
		multiq(arr,l,num-1);
	        
		_exit(1);
	}
	else
	{
		int y=fork();
			if(y==0)		
			{multiq(arr,num+1,r);
				_exit(1);
			}
			else
			{    int status;
				waitpid(x, &status, 0);
				waitpid(y, &status, 0);


			}

	}
}
struct box
{int l;
 int r;	
 int *arr;	

};
void * threadq(void* in ){
	
    struct box *val = (struct box*) in;

    int l = val->l;
    int r =val->r;
    int *arr = val->arr;
    int n=r-l+1;
    if(n==1||n==0)
	   return NULL; 

    
    if(n<=5){
        for(int i=l;i<r;i++)
        {
            int j=i+1; 
            for(;j<=r;j++)            
                if(arr[j]<arr[i] && j<=r) 
                {
                    int temp = arr[i];
                    arr[i] = arr[j];
                    arr[j] = temp;
                }
        }
        return NULL;
    }
int num=part(arr,l,r),i,j;


    struct box a1;
    a1.r = num-1;
    a1.l=l;
    a1.arr = arr;
    pthread_t tid1;
    pthread_create(&tid1, NULL, threadq, &a1);
    
    struct box a2;
    a2.r = r;
    a2.l=num+1;
    a2.arr = arr;
    pthread_t tid2;
    pthread_create(&tid2, NULL, threadq, &a2);
    
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    
}
int main()
{int n,i;
	scanf("%d",&n);
	struct timespec ts;

	int *arr = shareMem(sizeof(int)*(n+1));
	for(int i=0;i<n;i++) 
	{scanf("%d",arr+i);
	brr[i]=arr[i];
        crr[i]=arr[i];
	}	


    printf("Running concurrent_quicksort\n");
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	long double st = ts.tv_nsec/(1e9)+ts.tv_sec;

	multiq(arr,0,n-1);

	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	long double en = ts.tv_nsec/(1e9)+ts.tv_sec;
	printf("time = %Lf\n\n", en -  st);
	long double t1 = en-st;


    pthread_t tid;
    struct box a;
    a.l = 0;
    a.r = n-1;
    a.arr = brr;
    printf("Running threaded_concurrent_quicksort\n");
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    st = ts.tv_nsec/(1e9)+ts.tv_sec;

    pthread_create(&tid, NULL, threadq,&a);
    pthread_join(tid, NULL);

    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    en = ts.tv_nsec/(1e9)+ts.tv_sec;
    printf("time = %Lf\n\n", en - st);
    long double t2 = en-st;


    printf("Running normal_quicksort \n");
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	st = ts.tv_nsec/(1e9)+ts.tv_sec;

	quick(crr,0,n-1);


	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	en = ts.tv_nsec/(1e9)+ts.tv_sec;
	printf("time = %Lf\n\n", en - st);
	long double t3 = en - st;

    printf("normal_quicksort ran:\n\t[ %Lf ] times faster than concurrent_quicksort\n\t[ %Lf ] times faster than threaded_concurrent_quicksort\n", t1/t3, t2/t3);
	shmdt(arr);
	
}

