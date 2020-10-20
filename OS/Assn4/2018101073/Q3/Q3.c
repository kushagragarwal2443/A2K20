#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <errno.h>
//waitstate-0, onridepremier->1, onridepoolfull->2, onridepoolone->3
pthread_mutex_t lockforsem;
pthread_mutex_t lockforserver;
pthread_mutex_t lock;
typedef struct cabs
{
	int cabid;
	int cabstate;
	pthread_mutex_t lock;
} cabs ; 

//cabtype->0 if pool

typedef struct rider
{
	int riderid;
	int maxwaittime;
	int ridetime;
	int cabtype;
	int servertopay;
	pthread_t riderthread;
} rider;
typedef struct server
{
	int serverid;
	int status;
	pthread_t serverthread;	
} server;

int numofcabs, numofrider, numofservers;
cabs cabsarr[50000];
rider riderarr[50000];
server serverarr[50000];
sem_t freecabs, freecabsforpool, paymentserver;

void accept_payment(void * arg)
{
	rider *r=(rider*) arg;
	printf("Rider %d initiated paying via server %d\n", r->riderid, r->servertopay);
	sleep(2);
	printf("Rider %d paid via server %d\n", r->riderid, r->servertopay);
	return;
}

void *make_payment(void * arg)
{
	rider *r=(rider*) arg;
	int s = sem_wait(&paymentserver);
	int serverused=0;
	pthread_mutex_lock(&lockforserver);
	for (int i = 1; i <= numofservers ; ++i)
	{
		if (serverarr[i].status==0)
		{
			serverarr[i].status=1;
			r->servertopay=i;
			break;
		}
	}

	pthread_mutex_unlock(&lockforserver);
	accept_payment(r);
	serverarr[r->servertopay].status=0;
	sem_post(&paymentserver);
}

void* bookcab(void *arg)
{
	rider *r=(rider*) arg;
	r->cabtype=rand()%2;
	sleep(rand()%8+2);
	struct timespec ts;
	int s;
	if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
	{
	    exit(0);
	}
	r->maxwaittime=rand()%9+2;
	ts.tv_sec += r->maxwaittime;
	if(!r->cabtype)
    printf("Rider %d asked for poolcab and will wait for %d seconds\n", r->riderid, r->maxwaittime);
	else
    printf("Rider %d asked for premiercab and will wait for %d seconds\n", r->riderid, r->maxwaittime);
	fflush(stdout);
	if (!r->cabtype)
	{
		pthread_mutex_lock(&lockforsem);
		int sval=0, f=0;
		sem_getvalue(&freecabsforpool, &sval);
		if (sval!=0)
		{
			s = sem_timedwait(&freecabsforpool, &ts);
			f=1;
			
		}
		pthread_mutex_unlock(&lockforsem);
		if(f==0)
		{
			s = sem_timedwait(&freecabs, &ts);
			if (s == -1)
			{
			    if (errno == ETIMEDOUT)	
				{   printf("Rider %d timed out after %d seconds\n", r->riderid, r->maxwaittime);
					fflush(stdout);
			    }
			    else
			        perror("sem_timedwait");
			} else
			{
					int cabtaken=-1;
			        pthread_mutex_lock(&lock);
			        for (int i = 1; i <= numofcabs; ++i)
			        {
			        	if(cabsarr[i].cabstate==0)
			        	{
			        		cabsarr[i].cabstate=3;
			        		// printf("Entered\n");
			        		fflush(stdout);
			        		sem_post(&freecabsforpool);
			        		cabtaken=i;
			        		break;
			        	}
			        }
			        pthread_mutex_unlock(&lock);
					r->ridetime=rand()%10+1;
					if(cabtaken==-1)
					{
						printf("Rider %d timed out after %d seconds\n", r->riderid, r->maxwaittime);
						return NULL;
					}
		    		printf("Rider %d started journey of %d seconds with cab %d and cab status onRidepoolone \n", r->riderid, r->ridetime, cabtaken);
					fflush(stdout);
					sleep(r->ridetime);
		    		printf("Rider %d completed journey with cab %d\n", r->riderid, cabtaken);
					fflush(stdout);
					if (cabsarr[cabtaken].cabstate==2)
					{
						cabsarr[cabtaken].cabstate=3;
						sem_post(&freecabsforpool);
					}
					else
					{
						cabsarr[cabtaken].cabstate=0;
						sem_post(&freecabs);
					}	// sem_post(&freecabs);
			}
		}
		else
		{
			if (s == -1)
			{
			    if (errno == ETIMEDOUT)	
				{   printf("Rider %d timed out after %d seconds\n", r->riderid, r->maxwaittime);
					fflush(stdout);
			    }
			    else
			        perror("sem_timedwait");
			} else
			{
					int cabtaken=-1;
			        pthread_mutex_lock(&lock);

			        for (int i = 1; i <= numofcabs; ++i)
			        {
			        	if(cabsarr[i].cabstate==3)
			        	{
			        		cabsarr[i].cabstate=2;
			        		fflush(stdout);
			        		cabtaken=i;
			        		break;
			        	}
			        }
			        pthread_mutex_unlock(&lock);

					r->ridetime=rand()%10+1;
					if(cabtaken==-1)
					{
						printf("Rider %d timed out after %d seconds\n", r->riderid, r->maxwaittime);
						return NULL;
					}
		    		printf("Rider %d started journey of %d seconds with cab %d and cab status onRidepoolfull \n", r->riderid, r->ridetime, cabtaken);
					fflush(stdout);
					sleep(r->ridetime);
		    		printf("Rider %d completed journey with cab %d \n", r->riderid, cabtaken);
					fflush(stdout);
					if (cabsarr[cabtaken].cabstate==2)
					{
						cabsarr[cabtaken].cabstate=3;
						sem_post(&freecabsforpool);
					}
					else
					{
						cabsarr[cabtaken].cabstate=0;
						sem_post(&freecabs);
					}
			}
		}

		
	}
	else
	{
			s = sem_timedwait(&freecabs, &ts);
			if (s == -1)
			{
			    if (errno == ETIMEDOUT)	
				{   printf("Rider %d timed out after %d seconds\n", r->riderid, r->maxwaittime);
					fflush(stdout);
			    }
			    else
			        perror("sem_timedwait");
			} else
			{
			        // printf("sem_timedwait() succeeded\n");
					int cabtaken=-1;
			        pthread_mutex_lock(&lock);
			        for (int i = 1; i <= numofcabs; ++i)
			        {
					// pthread_mutex_lock(&cabsarr[i].lock);
			        	if(cabsarr[i].cabstate==0)
			        	{
			        		cabsarr[i].cabstate=1;
			        		// printf("Entered\n");
			        		fflush(stdout);
			        		cabtaken=i;
			        		break;
			        	}
					// pthread_mutex_unlock(&cabsarr[i].lock);
			        }
			        pthread_mutex_unlock(&lock);
			        if(cabtaken==-1)
					{
						printf("Rider %d timed out after %d seconds\n", r->riderid, r->maxwaittime);
						return NULL;
					}
					r->ridetime=rand()%10+1;
		    		printf("Rider %d started journey of %d seconds with cab %d and cab status onRidepremier \n", r->riderid, r->ridetime,cabtaken);
					fflush(stdout);
					sleep(r->ridetime);
		    		printf("Rider %d completed journey with cab %d\n", r->riderid, cabtaken);
					fflush(stdout);
					cabsarr[cabtaken].cabstate=0;
					sem_post(&freecabs);
			}
	}
	pthread_t serverthread;

    pthread_create(&serverthread, NULL, make_payment, r);
	pthread_join(serverthread, NULL);
	// make_payment(r);
}

int main()
{
	srand(time(0)); 
	printf("Enter number of cabs\n");
	scanf("%d", &numofcabs);
	printf("Enter number of riders\n");
	scanf("%d", &numofrider);
	printf("Enter number of servers\n");
	scanf("%d", &numofservers);
	// scanf("%d%d%d", &numofcabs, &numofrider, &numofservers);
	
	if (sem_init(&freecabs, 0, numofcabs) == -1)
    	perror("sem_init");
    if (sem_init(&paymentserver, 0, numofservers) == -1)
    	perror("sem_init");
    if (sem_init(&freecabsforpool, 0, 0) == -1)
    	perror("sem_init");
    pthread_mutex_init(&(lock), NULL);
    pthread_mutex_init(&(lockforsem), NULL);
    for (int i = 1; i <= numofcabs; ++i)
    {
    	cabsarr[i].cabid=i;
    	cabsarr[i].cabstate=0;
    	pthread_mutex_init(&(cabsarr[i].lock), NULL);
    	
    }
	for (int i = 1; i <= numofrider; ++i)
	{	
		riderarr[i].riderid=i;
	}
	for(int i=1; i<=numofservers; i++)
	{
		serverarr[i].serverid=i;
		serverarr[i].status=0;
		
	}
	for(int i=1; i<=numofrider; i++)
	{		
		if(pthread_create(&(riderarr[i].riderthread), NULL, bookcab, &(riderarr[i])))
		{
			perror("Error in creating rider thread");
		}
	}
	
	for(int i=1; i<=numofrider; i++)
	{
		pthread_join(riderarr[i].riderthread,NULL);
	}
} 