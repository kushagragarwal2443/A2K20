#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
int cnt;
typedef struct chef
{
	int chefid;
	int noofvesselsready;
	int status;
	int capacity;
	pthread_mutex_t lock;
	pthread_cond_t cond1;
	pthread_t chefthread;
} chef ; 
typedef struct table
{
	int tableid;
	int presentnoofslots;
	int eatingstudent;
	int totalnoofslots;
	int status;
	int var;
	int *stulist, top;
	chef *associatedchef;	
	pthread_mutex_t lock;
	pthread_cond_t cond1, cond2;
	pthread_t tablethread;	
} table;
typedef struct student
{
	int studentid;
	int studentslot;
	int studenttable;
	int status;
	pthread_t studentthread;	
} student;

pthread_mutex_t lock;
int numoftables, numofchefs, numofstudents;
chef chefarr[20000];
table tablearr[20000];
student studentsarr[20000];
void push(int* st, int top, int y){
	if(top >= 9){
		return;
	}
	st[++top] = y;
}

int pop(int *st, int top){
	if(top > -1){
		return st[top];
	}
	return 0;
}

void biryani_ready(void * arg)
{
	chef * c = (chef*)arg;
	pthread_mutex_lock(&c->lock);
	printf("Chef with id %d created %d vessels with %d capacity\n", c->chefid, c->noofvesselsready, c->capacity);
	fflush(stdout);
	c->status=1;
	pthread_cond_wait(&c->cond1, &c->lock);
	c->status=0;
	pthread_mutex_unlock(&c->lock);
	return;
}
void *prepare_food(void * arg)
{
	
	chef * c = (chef*)arg;
	while(numofstudents)
	{
		int timeforvesselcreation=rand()%4+2;
		int  noofvessels=	(rand()%10+1);
		printf("Robot Chef %d is preparing %d vessels of Biryani\n", c->chefid,noofvessels);
		fflush(stdout);
		sleep(timeforvesselcreation);
		// printf("dsjkl\n");
		pthread_mutex_lock(&c->lock);
		c->noofvesselsready=noofvessels;
		c->capacity=rand()%25+25;
		pthread_mutex_unlock(&c->lock);
		biryani_ready(c);
		printf("All the vessels prepared by Robot Chef %d are emptied. Resuming cooking now.\n", c->chefid);
		fflush(stdout);
	}

}

void ready_to_serve(void *arg)
{
	table * t = (table*)arg;
	while(t->totalnoofslots&&numofstudents)
	{
		pthread_mutex_lock(&t->lock);
		t->presentnoofslots=rand()%10+1;

		if((t->presentnoofslots)>(t->totalnoofslots))
		{
			t->presentnoofslots=t->totalnoofslots;
		}
		t->totalnoofslots-=t->presentnoofslots;
		t->eatingstudent=t->presentnoofslots;
		t->var=0;
		printf("Table with id %d is ready with %d slots\n", t->tableid, t->presentnoofslots);
		fflush(stdout);
		t->status=1;
		pthread_cond_wait(&t->cond1, &t->lock);

		// pthread_mutex_lock(&t->lock);
		t->status=0;
		pthread_mutex_unlock(&t->lock);
	}
}

void *want_food(void * arg)
{
	
	table * t = (table*)arg;
	while(numofstudents)
	{
		
		for (int i = 1; i <=numofchefs ; ++i)
		{
				pthread_mutex_lock(&chefarr[i].lock);
			if (chefarr[i].status==1)
			{
				// pthread_mutex_lock(&t->lock);
				t->totalnoofslots=chefarr[i].capacity;
				t->associatedchef=&chefarr[i];
				// pthread_mutex_unlock(&t->lock);
				chefarr[i].noofvesselsready-=1;
				if(!chefarr[i].noofvesselsready)
				{
					chefarr[i].status=0;
					pthread_cond_broadcast(&chefarr[i].cond1);
				}
				pthread_mutex_unlock(&chefarr[i].lock);
				printf("Robot Chef %d is refilling Serving Container of Serving Table %d\n", i, t->tableid);
				fflush(stdout);
				printf("Serving Container of Table %d is refilled by Robot Chef %d\n", t->tableid, i);
				fflush(stdout);
				ready_to_serve(t);
				if(numofstudents)
				printf("Serving Container of Table %d is empty, waiting refil\n", t->tableid);
				fflush(stdout);
				break;
			}
			else
			{	
				pthread_mutex_unlock(&chefarr[i].lock);
			}
		}
	}

}

void student_in_slot(void * arg)
{
	student * s = (student*)arg;
	numofstudents--;
	
	pthread_mutex_lock(&tablearr[s->studenttable].lock);
	if(tablearr[s->studenttable].presentnoofslots == 0){
		printf("Serving table %d entering Serving Phase\n", s->studenttable);
		fflush(stdout);
		while(tablearr[s->studenttable].top > -1){
			printf("Student %d ate on Serving Table %d \n", pop(tablearr[s->studenttable].stulist, tablearr[s->studenttable].top), s->studenttable);
			tablearr[s->studenttable].top--;
			fflush(stdout);
		}
		pthread_cond_broadcast(&(tablearr[s->studenttable].cond1));
	}
	pthread_mutex_unlock(&tablearr[s->studenttable].lock);

	return;

}

void *wait_for_slot(void * arg)
{
	student * s = (student*)arg;
	sleep(rand()%5);
	printf("Student %d has arrived\n", s->studentid);
	fflush(stdout);
	printf("Student %d is waiting to be allocated a slot on the serving table\n", s->studentid);
	fflush(stdout);
	s->status=0;
	while(s->status==0)
	{
		for (int i = 1; i <=numoftables; ++i)
		{
			pthread_mutex_lock(&tablearr[i].lock);
			if (tablearr[i].status&& tablearr[i]. presentnoofslots!=0)
			{
				s->status=1;
				s->studenttable=tablearr[i].tableid;
				printf("Student with id %d is assigned slot %d at table %d\n", s->studentid,  tablearr[i].presentnoofslots ,s->studenttable);
				fflush(stdout);
				tablearr[i].presentnoofslots-=1;
				push(tablearr[i].stulist, tablearr[i].top, s->studentid);
				tablearr[i].top++;
				pthread_mutex_unlock(&tablearr[i].lock);
				student_in_slot(s);
				break;

			}
			pthread_mutex_unlock(&tablearr[i].lock);

		}	
	}
}
int main()
{
	srand(time(0));
	cnt =0;
	printf("Enter number of tables\n");
	scanf("%d", &numoftables);
	printf("Enter number of chefs\n");
	scanf("%d", &numofchefs);
	printf("Enter number of students\n");
	scanf("%d", &numofstudents);
	int newstudentnumber=numofstudents;
	printf("Serving begins\n");
	fflush(stdout);
	for (int i = 1; i <= numofchefs; ++i)
	{	
		chefarr[i].chefid=i;
		pthread_mutex_init(&(chefarr[i].lock), NULL);
		pthread_cond_init(&(chefarr[i].cond1), NULL);
	}
	for (int i = 1; i <= numoftables; ++i)
	{	
		tablearr[i].tableid=i;
		tablearr[i].stulist = (int*) malloc(sizeof(int)*11);
		tablearr[i].top = -1;
		pthread_mutex_init(&(tablearr[i].lock), NULL);
		pthread_cond_init(&(tablearr[i].cond1), NULL);
		pthread_cond_init(&(tablearr[i].cond2), NULL);
	}
	for (int i = 1; i <= numofstudents; ++i)
	{	
		studentsarr[i].studentid=i;
	}
	for(int i=1; i<=numofchefs; i++)
	{		
		if(pthread_create(&(chefarr[i].chefthread), NULL, prepare_food, &(chefarr[i])))
		{
			perror("Error in creating chef thread");
		}
	}
	for(int i=1; i<=numoftables; i++)
	{		
		if(pthread_create(&(tablearr[i].tablethread), NULL, want_food, &(tablearr[i])))
		{
			perror("Error in creating table thread");
		}
	}
	for(int i=1; i<=newstudentnumber; i++)
	{		
		if(pthread_create(&(studentsarr[i].studentthread), NULL, wait_for_slot, &(studentsarr[i])))
		{
			perror("Error in creating table thread");
		}
	}
	
	for(int i=1; i<=newstudentnumber; i++)
	{
		pthread_join(studentsarr[i].studentthread,NULL);
	}
	for(int i=1; i<=numoftables; i++){
		if(tablearr[i].top > -1) printf("Serving table %d entering Serving Phase\n", i);
		fflush(stdout);
		while(tablearr[i].top > -1){
			printf("Student %d ate on Serving Table %d\n", pop(tablearr[i].stulist, tablearr[i].top), i);
			fflush(stdout);
			tablearr[i].top--;
		}
	}
	printf("All the students have eaten\n");
	fflush(stdout);
	// printf("%d\n",cnt);
} 