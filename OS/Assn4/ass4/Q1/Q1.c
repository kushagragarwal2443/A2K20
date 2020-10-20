#define _POSIX_C_SOURCE 199309L
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<wait.h>
#include<limits.h>
#include<fcntl.h>
#include<time.h>
#include<pthread.h>
#include<inttypes.h>
#include<math.h>

typedef struct node
{
	int lo;
	int hi;
}node;


int *shareMem(size_t size)
{
	key_t mem_key = IPC_PRIVATE;
	int shm_id = shmget(mem_key, size, IPC_CREAT | 0666);
	return (int*)shmat(shm_id, NULL, 0);
}

int *ar,br[100010],go[100010];

int part1(int lo,int hi)
{
	int piv=ar[hi];
	int i=lo-1,j;
	for(j=lo;j<=hi-1;j++)
	{
		if(ar[j]<piv)
		{
			++i;
			int temp=ar[i];
			ar[i]=ar[j];
			ar[j]=temp;
		}
	}
	int temp=ar[i+1];
	ar[i+1]=ar[hi];
	ar[hi]=temp;
	return (i+1);
}
int part2(int lo,int hi)
{
	int piv=br[hi];
	int i=lo-1,j;
	for(j=lo;j<=hi-1;j++)
	{
		if(br[j]<piv)
		{
			++i;
			int temp=br[i];
			br[i]=br[j];
			br[j]=temp;
		}
	}
	int temp=br[i+1];
	br[i+1]=br[hi];
	br[hi]=temp;
	return (i+1);
}
int part3(int lo,int hi)
{
	int piv=go[hi];
	int i=lo-1,j;
	for(j=lo;j<=hi-1;j++)
	{
		if(go[j]<piv)
		{
			++i;
			int temp=go[i];
			go[i]=go[j];
			go[j]=temp;
		}
	}
	int temp=go[i+1];
	go[i+1]=go[hi];
	go[hi]=temp;
	return (i+1);
}


void normal_sort(int lo,int hi)
{
	if(hi-lo+1<=5)
	{
		int i,j;
		for(i=lo;i<hi;i++)
		{
			for(j=i+1;j<=hi;j++)
			{
				if(br[j]<br[i] && j<=hi)
				{
					int temp = br[i];
					br[i] = br[j];
					br[j] = temp;
				}
			}
		}
		return;
	}
	else if(lo<hi)
	{
		int pi=part2(lo,hi);
		normal_sort(lo,pi-1);
		normal_sort(pi+1,hi);
	}
	return ;
}


void quicksort(int lo,int hi)
{
	if(hi-lo+1<=5)
	{
		int i,j;
		for(i=lo;i<hi;i++)
		{
			for(j=i+1;j<=hi;j++)
			{
				if(ar[j]<ar[i] && j<=hi)
				{
					int temp = ar[i];
					ar[i] = ar[j];
					ar[j] = temp;
				}
			}
		}
		return;
	}
	else if(lo<hi)
	{
		int pi=part1(lo,hi);
		pid_t p2,p1=fork();
		if(p1==0)
		{
			quicksort(lo,pi-1);
			exit(0);
		}
		else
		{
			p2=fork();
			if(p2==0)
			{
				quicksort(pi+1,hi);
				exit(0);
			}
			else
			{
				int st=0;
				waitpid(p1,&st,0);
				waitpid(p2,&st,0);
			}
		}
		return;
	}
	return ;
}


void *thread_quick(void *a)
{
	node *args = (node *) a;
	int lo = args->lo;
	int hi = args->hi;

	if(hi-lo+1<=5)
	{
		int i,j;
		for(int i=lo;i<hi;i++)
		{ 
			for(j=i+1;j<hi;j++)
			{            
				if(go[j]<go[i] && j<=hi) 
				{
					int temp = go[i];
					go[i] = go[j];
					go[j] = temp;
				}
			}
		}
		return NULL;
	}
	int pi=part3(lo,hi);

	node a1;
	a1.lo = lo;
	a1.hi = pi-1;

	pthread_t t1,t2;
	pthread_create(&t1, NULL, thread_quick, &a1);

	node a2;
	a2.lo = pi+1;
	a2.hi = hi;

	pthread_create(&t2, NULL, thread_quick, &a2);
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	return NULL;
}


int main()
{
	struct timespec ts;
	int n,i;
	scanf("%d",&n);
	ar = shareMem(sizeof(int)*(n+1));
	for(int i=0;i<n;i++) 
		scanf("%d",ar+i);


	printf("\nConcurrent Quicksort\n");
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	long double st = ts.tv_nsec/(1e9)+ts.tv_sec;

	quicksort(0,n-1);

	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	long double en = ts.tv_nsec/(1e9)+ts.tv_sec;
	printf("time = %Lf\n", en - st);
	long double t1=en-st;

	for(int i=0;i<n;i++)
	{
		br[i]=ar[i];
		go[i]=ar[i];
	}

	pthread_t t;
	node a;
	a.lo = 0;
	a.hi = n-1;

	printf("\nThreaded Concurrent Quicksort\n");
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	st = ts.tv_nsec/(1e9)+ts.tv_sec;

	pthread_create(&t, NULL, thread_quick, &a);
	pthread_join(t, NULL);    

	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	en = ts.tv_nsec/(1e9)+ts.tv_sec;
	printf("time = %Lf\n", en - st);
	long double t2=en-st;


	printf("\nNormal Quicksort\n");
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	st = ts.tv_nsec/(1e9)+ts.tv_sec;

	normal_sort(0,n-1);

	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	en = ts.tv_nsec/(1e9)+ts.tv_sec;
	printf("time = %Lf\n", en - st);
	long double t3=en-st;


	printf("\nSorted Array\n");
	for(i=0;i<n;i++)
		printf("%d ",ar[i]);
	printf("\n");
	printf("\nNormal Quicksort ran:\n[ %Lf ] times faster than Concurrent Quicksort\n[ %Lf ] times faster than Threaded Concurrent Quicksort\n", t1/t3, t2/t3);
	shmdt(ar);
	return 0;
}
