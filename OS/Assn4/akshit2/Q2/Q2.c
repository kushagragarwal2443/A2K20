#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>

int tabslot[10];
int tabeating[10];
int table[10];
int chef[10];
int tablenum;
int robonum;
int stunum;

struct indx
{
	int inx;
};
typedef struct indx indx;


pthread_mutex_t mutex1;
pthread_mutex_t mutex2;
pthread_mutex_t mutex3;

void biryani_ready(int ind,int size,int num)
{
	int i;
	while(num>0)
	{ 
		pthread_mutex_lock(&mutex1);
		for(i=0;i<tablenum;i++)
		{if(table[i]==0)
			{table[i]+=size;
				num--;
				printf("serving conatiner of table %d is refilled by robot chef %d with vessel capcity %d\n",i+1,ind,size);
				if(num<=0)
					break;
			}
		}
		pthread_mutex_unlock(&mutex1);


	}

}

void ready_to_serve_table(int ind)
{
	int z=0;
	while(z!=1)
	{
		pthread_mutex_lock(&mutex2);
		if(tabslot[ind-1]==0)
			z=1;
		pthread_mutex_unlock(&mutex2);
	}
}

void* chf(void * input)
{	indx * in=(indx*) input;
	int ind=in->inx;

	int size=rand()%26+25;
	int a=rand()%5+1;
        int t=rand()%5 +1;

	int num=a,i;
	while(1)
	{printf("robot chef %d preparing biryani\n",ind);
	sleep(t);
	printf("robot chef %d has prepared %d vessels of biryani\n",ind,num);
	     num=a;
		biryani_ready(ind,size,num);
		printf("All the vessel prepared by robot chef %d are empty\n",ind);
	}
}

void* tble(void * input)
{	indx * in=(indx*) input;
	int ind=in->inx,num,max=10,z=0,i,slot;
	while(1)
	{       z=0;
		while(z!=1)
		{     
			pthread_mutex_lock(&mutex1);
			pthread_mutex_lock(&mutex2);
			if(table[ind-1]>0)
			{ 
			       	if(table[ind-1]>max)
				{
					num=max;
				}
				else
				{
				       	num=table[ind-1];
				}

				z=1;
				slot=rand()%num+1;
				tabslot[ind-1]+=slot;
			}
			if(z==1)
				printf("serving table %d is ready with %d slot\n",ind,slot);
			pthread_mutex_unlock(&mutex2);
			pthread_mutex_unlock(&mutex1);
		}
		ready_to_serve_table(ind);

	}
}

void student_in_slot(int ind,int i)
{       
	printf("student %d is served \n",ind);
	table[i]--;
}

void wait_for_slot(int ind)
{
	int z=0,i;
	while(z!=1)
	{
		pthread_mutex_lock(&mutex1);
		pthread_mutex_lock(&mutex2);
		for(i=0;i<tablenum;i++)
		{ if(tabslot[i]>0)
			{
				tabslot[i]--;
				z=1;
				printf("student %d is assigned a slot on table %d\n",ind,i+1);
				student_in_slot(ind,i);
				break;
			}
		}
		pthread_mutex_unlock(&mutex2);
		pthread_mutex_unlock(&mutex1);

	}

}
void* student(void* input)
{
	indx * in=(indx*) input;
	int ind=in->inx,z=0;

	int ss=rand()%15;
	sleep(ss);
	printf("student %d has arrived\n",ind);

	wait_for_slot(ind);


}

int main()
{       srand(time(0));
	scanf("%d%d%d",&robonum,&tablenum,&stunum);
	pthread_t stu[stunum];
	pthread_t ch[robonum];
	pthread_t ta[tablenum];

	indx dd[tablenum];
	indx cc[robonum];
	indx ee[stunum];

	pthread_mutex_init(&mutex1,NULL);
	pthread_mutex_init(&mutex2,NULL);
	int i;

	for(i=0;i<robonum;i++)
	{       cc[i].inx=i+1;
		pthread_create(&ch[i],NULL,chf,(void*)&cc[i]);
	}

	for(i=0;i<tablenum;i++)
	{       dd[i].inx=i+1;
		pthread_create(&ta[i],NULL,tble,(void*)&dd[i]);
	}

	for(i=0;i<stunum;i++)
	{       ee[i].inx=i+1;
		pthread_create(&stu[i],NULL,student,(void*)&ee[i]);
	}



	for(i=0;i<stunum;i++)
		pthread_join(stu[i],NULL);


	pthread_mutex_destroy(&mutex1);
	pthread_mutex_destroy(&mutex2);
	pthread_mutex_destroy(&mutex3);
	printf("Simulation over\n");
}



