#include<stdio.h>
#include<pthread.h>
#include<time.h>
#include<unistd.h>
#include<stdlib.h>
pthread_t tid[100010],sid[200101],rid[101010];
pthread_mutex_t mutex_rob,mutex_student,mutex_table;
long long m,n,w,k,p,r,arrival_time,current=0;
typedef struct table
{
	long long  table_id;
	long long  slots;
	long long  state;
	long long  capacity;
}tables;

typedef struct students
{
	long long  student_id;
	long long  state;
}students;

typedef struct robots
{
	long long  robot_id;
	long long  vessels;
}robots;

robots robot[10010];
students student[20010];
tables table[201010];
long long  waiting=0;

void student_init()
{
	for(long long  i=0;i<k;i++)
	{
		student[i].state=-1;
		student[i].student_id=i;
	}
}
void robot_init()
{
	for(long long  i=0;i<m;i++)
	{
		robot[i].robot_id=i;
		robot[i].vessels=0;
	}
}
void table_init()
{
	for(long long  i=0;i<n;i++)
	{
		table[i].table_id=i;
		table[i].slots=0;
		table[i].capacity=0;
	}
}
long long min(long long  a,long long  b)
{
	if(a>b)
		return b;
	else
		return a;
}


//Robots
void biryani_ready(long long  id)
{
	long long  a=0;
	while(1)
	{
		pthread_mutex_lock(&mutex_rob);
		if(robot[id].vessels==0)
		{
			printf("All the Vessels prepared by Robot %lld are emptied.Resume cooking now\n",id);
			pthread_mutex_unlock(&mutex_rob);
			return;
		}
		pthread_mutex_unlock(&mutex_rob);
	}
}
void *make(void *arg)
{
	robots *rb = (robots *)arg;
	while(1)
	{
		printf("Robot %lld started cooking vessel\n",rb->robot_id);
		sleep(w);
		printf("Robot %lld finished cooking %lld vessels\n",rb->robot_id,r);
		pthread_mutex_lock(&mutex_rob);
		robot[rb->robot_id].vessels=r;
		pthread_mutex_unlock(&mutex_rob);
		sleep(1);
		biryani_ready(rb->robot_id);
	}
}


//Students
void student_in_slot(long long ta,long long id)
{
	sleep(2);
	printf("Student %lld served the food on table %lld\n",id,ta);
	pthread_mutex_lock(&mutex_table);
	current--;
	k--;
	table[ta].capacity--;
	waiting--;
	pthread_mutex_unlock(&mutex_table);
	return;
}
void *wait_for_slot(void *arg)
{
	sleep(arrival_time);
	long long flag=0,i;
	students *st  = (students *)arg;
	printf("Student %lld arrived\n",st->student_id);
	pthread_mutex_lock(&mutex_student);
	current++;
	pthread_mutex_unlock(&mutex_student);
	int a=0;
	while(1)
	{
		pthread_mutex_lock(&mutex_student);
		for(i=0;i<n;i++)
		{
			if(table[i].slots > 0)
			{
				flag=1;
				table[i].slots--;
				printf("Student %lld allocated the table %lld\n",st->student_id,i);
				break;
			}
		}
		pthread_mutex_unlock(&mutex_student);
		if(flag==1)
			break;
		else if(a==0)
		{
			waiting++;
			printf("Student %lld waiting for Slot\n",st->student_id);
		}
		a++;
	}
	student_in_slot(i,st->student_id);
}


//Serving Tables
void ready_to_serve_table(long long id,long long num)
{
	while(1)
	{
		if(table[id].slots==0 || table[id].capacity==0 || current==0 || waiting==0)
			return;
	}
}
void *get(void *arg)
{
	long long flag=0,flag2=0,i;
	tables *tab = (tables *)arg;
	while(1)
	{
		while(1)
		{
			pthread_mutex_lock(&mutex_rob);
			for(i=0;i<m;i++)
			{
				if(robot[i].vessels > 0 && table[tab->table_id].slots==0)
				{
					printf("Robot %lld is filling Service container of Table %lld\n",i,tab->table_id);
					robot[i].vessels--;
					sleep(2);
					pthread_mutex_lock(&mutex_table);
					//table[tab->table_id].state = i;
					table[tab->table_id].capacity=p;
					printf("Table %lld enters into serving mode\n",tab->table_id);
					pthread_mutex_unlock(&mutex_table);
					flag=1;
					break;
				}
			}
			pthread_mutex_unlock(&mutex_rob);
			if(flag==1)
				break;
		}
		while(table[tab->table_id].capacity > 0)
		{
			pthread_mutex_lock(&mutex_table);
			long long num=min(table[tab->table_id].capacity,(rand()%10)+1);
			table[tab->table_id].slots=num;
			printf("Table %lld ready to serve with %lld slots\n",tab->table_id,num);
			pthread_mutex_unlock(&mutex_table);
			ready_to_serve_table(tab->table_id,num);
			//pthread_mutex_lock(&mutex_table);
			if(waiting == 0 || current==0 || k==0)
			{
				flag2=1;
				printf("No waiting Students\n");
				table[tab->table_id].capacity=0;
				printf("Table %lld waiting to get refilled\n",tab->table_id);
				break;
			}
			if(table[tab->table_id].capacity==0)
			{
				printf("Table %lld waiting to get refilled\n",tab->table_id);
				break;
			}
			//pthread_mutex_unlock(&mutex_table);
		}
		if(flag2==1)
			break;
		//printf("MEHJ");
	}
}


int main()
{
	srand(time(0));
	w=rand()%4+2;
	p=rand()%26+25;
	r=rand()%10+1;
	printf("Enter number of robot chefs,serving tables and studetns\n");
	scanf("%lld%lld%lld",&m,&n,&k);
	robot_init(m);
	student_init(k);
	table_init(n);
	for(long long i=0;i<m;i++)
		pthread_create(&rid[i],NULL,make,&robot[i]);
	for(long long  i=0;i<n;i++)
		pthread_create(&tid[i],NULL,get,&table[i]);
	for(long long  i=0;i<k;i++)
	{
		arrival_time=rand()%10+1;
		pthread_create(&sid[i],NULL,wait_for_slot,&student[i]);
	}
	for(long long  i=0;i<k;i++)
		pthread_join(sid[i],NULL);
}
