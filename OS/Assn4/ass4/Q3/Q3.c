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

pthread_t p_t[100010],r_t[300010];
long long n,m,k,arrival_time;
typedef struct Cab
{
	long long state;
}cab;

typedef struct driver
{
	long long state;
	long long driver_id;
	cab c;
}driver;

typedef struct payment
{
	long long pay_id;
	long long state;
}server;

typedef struct rider
{
	long long ctype;
	long long rider_id;
	long long maxtime;
	long long ridetime;
	long long server;
}rider;

driver drivers[100100];
rider riders[200010];
server servers[201010];
long long fl[100010];

pthread_mutex_t mutex_r;
pthread_mutex_t mutex_p;
void driver_init(long long n)
{
	for(long long i=0;i<n;i++)
	{
		drivers[i].state=0;
		drivers[i].driver_id=i;
		if(i%2==0)
			drivers[i].c.state=1;
		else
			drivers[i].c.state=2;
	}
}
void rider_init(long long m)
{
	for(long long i=0;i<m;i++)
	{
		if(i%2==0)
			riders[i].ctype = 1;
		else
			riders[i].ctype = 2;
		riders[i].rider_id=i;
		riders[i].ridetime = (rand()%3+1);
		riders[i].maxtime = (rand()%10+1);
		riders[i].server=-1;
	}
}
void payment_init(long long k)
{
	for(long long i=0;i<k;i++)
	{
		servers[i].state=0;
		servers[i].pay_id=i;
	}
}

void *pay(void *arg)
{
	long long flag=0,i;
	server *t = (server *) arg;
	while(1)
	{
		pthread_mutex_lock(&mutex_p);
		for(i=0;i<m;i++)
		{
			if(riders[i].server!=-1 && t->state == 0)
			{
				flag=1;
				break;
			}
		}
		if(flag==1)
		{
			riders[i].server=-1;
			printf("Payment initialised for rider %lld at server %lld\n",i,t->pay_id);
			t->state=1;
		}
		pthread_mutex_unlock(&mutex_p);
		if(flag==1)
		{
			sleep(2);
			printf("Payment Done by rider %lld at server %lld\n",i,t->pay_id);
			fl[i]=1;
			t->state=0;
			flag=0;
		}
	}
	return NULL;

}
void *ride(void *arg)
{
	sleep(arrival_time);
	struct timespec ts;
	long long flag1=0,flag2=0,i;
	rider *riding = (rider *) arg;
	printf("Rider %lld arrived\n",riding->rider_id);
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	long double st = ts.tv_nsec/(1e9)+ts.tv_sec;
	while(1)
	{
		clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
		long double en = ts.tv_nsec/(1e9)+ts.tv_sec;
		if(en-st > riding->maxtime && flag1==0 && flag2==0)
		{
			printf("TIMEOUT Rider %lld gone\n",riding->rider_id);
			return NULL;
		}
		pthread_mutex_lock(&mutex_r);
		if(riding->ctype==1)
		{
			for(i=0;i<n;i++)
			{
				if(drivers[i].state == 0)
				{
					if(drivers[i].c.state == 1)
					{
						flag1=1;
						printf("Rider %lld with driver onRidePremier\n",riding->rider_id);
						drivers[i].state = 1;
						break;
					}
				}
			}
		}
		else
		{
			for(i=0;i<n;i++)
			{
				if(drivers[i].state == 0 || drivers[i].state == 2)
				{
					if(drivers[i].c.state == 2)
					{
						flag2=1;
						if(drivers[i].state == 0)
						{
							 drivers[i].state=2;
							 printf("Rider %lld with driver onRidePoolOne\n",riding->rider_id);
						}
						else
						{
							printf("Rider %lld with driver onRidePoolFull\n",riding->rider_id);
							drivers[i].state=3;
						}
						break;
					}
				}
			}
		}
		pthread_mutex_unlock(&mutex_r);
		if(flag1==1 || flag2==1)
			break;
	}
	sleep(riding->ridetime);
	pthread_mutex_lock(&mutex_r);
	printf("Ride time of rider %lld finished\n",riding->rider_id);
	riders[riding->rider_id].server=1;
	drivers[i].state=0;
	pthread_mutex_unlock(&mutex_r);
	while(fl[riding->rider_id]==0);
	return NULL;
}
int main()
{
	srand(time(0));
	printf("Enter Drivers Riders Payment servers\n");
	scanf("%lld%lld%lld",&n,&m,&k);
	driver_init(n);
	payment_init(k);
	rider_init(m);
	for(long long i=0;i<k;i++)
		pthread_create(&p_t[i],NULL,pay,&servers[i]);
	for(long long i=0;i<m;i++)
	{
		arrival_time=(rand()%10+1);
		pthread_create(&r_t[i],NULL,ride,&riders[i]);
	}
	for(long long i=0;i<m;i++)
		pthread_join(r_t[i],NULL);
}
