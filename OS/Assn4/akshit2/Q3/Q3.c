#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<semaphore.h>
int lit[1000000][2];
int  count[1000000];
int n,m,k;
struct cab
{
	int rider;
};

typedef struct cab cab;

struct box
{
	int inx;
	cab* c;

};
typedef struct box box;

struct box1
{
	int inx;

};

typedef struct box1 box1;

pthread_mutex_t mutex;
pthread_mutex_t mutex2;

void* server(void* input)
{
	box1 * got =(box1*) input;	
	int ind=got->inx;
	while(1)
	{int i,z=0;
		while(z!=1)
		{pthread_mutex_lock(&mutex2);
			for(i=0;i<m;i++)
			{ if(lit[i][0]==1&&count[i]==0)
				{       z=1;
					count[i]=1;
					break;
				}
			}
			if(z==1)
				printf("payment initiated for rider %d in %d server\n", i+1,ind);
			pthread_mutex_unlock(&mutex2);
		}
		sleep(2);
		pthread_mutex_lock(&mutex2);
		printf("payment completed for rider %d in %d server\n", i+1,ind);
		lit[i][1]=1;
		pthread_mutex_unlock(&mutex2);


	}

}
int book_cab(void *input,int a,int w_time,int r_time)
{       
	struct timespec ts;
	box * got =(box*) input;	
	cab * in=got->c;
	int ind=got->inx;
	int z=0,i;
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    	long double st = ts.tv_nsec/(1e9)+ts.tv_sec;


  if(a==0)
        {
                while(z!=1)
  		{
			clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
			long double en = ts.tv_nsec/(1e9)+ts.tv_sec;
   			long double t1 = en-st;
			 
			if(t1 > w_time)
			return -1;	

                	pthread_mutex_lock(&mutex);
                        for(i=0;i<n;i++)
                        { if(in[i].rider==0)
                                {in[i].rider=2;
                                        z=1;
                                        break;
                                }
                        }

                        if(z==1)
                                printf("rider %d is in cab %d\n", ind,i+1);
                        pthread_mutex_unlock(&mutex);
                }
                sleep(r_time);

                pthread_mutex_lock(&mutex);

                in[i].rider=0;
                printf("rider %d ride over\n", ind);

                pthread_mutex_unlock(&mutex);



        }
  else if(a==1)
        {
                while(z!=1)
		{
			clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
			long double en = ts.tv_nsec/(1e9)+ts.tv_sec;
   			long double t1 = en-st;
			 
			if(t1 > w_time)
			return -1;	

               		pthread_mutex_lock(&mutex);
                        for(i=0;i<n;i++)
                        { if(in[i].rider==0 || in[i].rider==1)
                                {in[i].rider++;
                                        z=1;
                                        break;
                                }
                        }
                        if(z==1)
                                printf("rider %d is in cab %d\n", ind,i+1);
                        pthread_mutex_unlock(&mutex);
                }


                sleep(r_time);


                pthread_mutex_lock(&mutex);

                in[i].rider--;
                printf("rider %d ride over\n", ind);

                pthread_mutex_unlock(&mutex);




        }
  return 0;

}	
void make_payment(int ind)
{
		pthread_mutex_lock(&mutex2);
		lit[ind-1][0]=1;
		pthread_mutex_unlock(&mutex2);
		while(lit[ind-1][1]==0);

}
void* rider(void* input)
{
        	
	box * got =(box*) input;	
	cab * in=got->c;
	int ind=got->inx;
	int i,z=0,y=0;

	int a=rand()%2;
        int w_time=rand()%10+1;
	int r_time=rand()%5+1;
	int ss=rand()%10;

	sleep(ss);
	if(a==0)
	printf("rider %d max wait =  %d  ride time  = %d ride type = premium has arrived\n",ind,w_time,r_time);
	else
	printf("rider %d max wait =  %d  ride time  = %d ride type = pool has arrived\n",ind,w_time,r_time);

       int d=book_cab(input,a,w_time,r_time);
       if(d<0)
       printf("rider %d did not find ride thus exited\n",ind);	
       else       
       make_payment(ind);

	return NULL;

}
int main()
{	int i;
	srand(time(0));
	scanf("%d%d%d",&n,&m,&k);

	cab arr[n];
	for(i=0;i<n;i++)
		arr[i].rider=0;

	pthread_t rid[m];
	pthread_t ser[k];
	box dd[m];
	box1 cc[k];

	pthread_mutex_init(&mutex,NULL);
	pthread_mutex_init(&mutex2,NULL);

	for(i=0;i<k;i++)
	{       cc[i].inx=i+1;
		pthread_create(&ser[i],NULL,server,(void*)&cc[i]);
	}

	for(i=0;i<m;i++)
	{       dd[i].inx=i+1;
		dd[i].c=arr;
		pthread_create(&rid[i],NULL,rider,(void*)&dd[i]);
	}


	for(i=0;i<m;i++)
		pthread_join(rid[i],NULL);


	pthread_mutex_destroy(&mutex);
	pthread_mutex_destroy(&mutex2);


}
