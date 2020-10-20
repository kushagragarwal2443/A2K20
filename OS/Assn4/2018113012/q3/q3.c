#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <wait.h>
#include <assert.h>
#include <signal.h>
#include <sys/shm.h>

sem_t acoustic_stages, electric_stages, coordinators;

pthread_mutex_t mutex;

int k,a,e,c,t1,t2,t;

typedef struct performer
{
    int index;
    char name[100];
	char type;
    int arrival_time;
    int got_stage;

}performer;

performer performers[100];


void *check_coordinator(char name[])
{

    sem_wait(&coordinators);

    printf("%s tshirt collection process begins\n", name);
    sleep(2);
    printf("%s says Good-bye!!!\n", name);
    sem_post(&coordinators);

    return NULL;

}

void *check_electric_stage(void *p)
{   
    performer* artist = (performer*) p;

    pthread_mutex_lock(&mutex);
    int artist_index = artist->index;
    int artist_arrival_time = artist->arrival_time;
    int artist_got_stage = artist->got_stage;
    char artist_type = artist->type;
    char artist_name[100];
    strcpy(artist_name,"");
    strcpy(artist_name, artist->name);
    pthread_mutex_unlock(&mutex);

    sleep(artist_arrival_time);

    pthread_mutex_lock(&mutex);
    artist_got_stage = artist->got_stage;
    pthread_mutex_unlock(&mutex);

    if(artist_got_stage == 0)
    {
        printf("%s has arrived at Srujana and is looking for an electric stage to play %c\n", artist_name, artist_type);
    }
    else
    {
        return NULL;
    }
    
    struct timespec ts;
	if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
	{
	    exit(0);
	}
	ts.tv_sec += t;

    int s = sem_timedwait(&electric_stages, &ts);
    if (s == -1)
    {
        if (errno == ETIMEDOUT)	
        {   
            printf("%s left in frustation after %d seconds due to a long waiting time\n", artist_name, t);
            pthread_mutex_lock(&mutex);
            artist->got_stage = 1;
            pthread_mutex_unlock(&mutex);
            return NULL;
        }

    }
        
    // sem_wait(&electric_stages);
    
    //Check if the artist got stage anywhere if yes then terminate
    pthread_mutex_lock(&mutex);
    artist_got_stage = artist->got_stage;
    pthread_mutex_unlock(&mutex);

    if(artist_got_stage == 1)
    {
        return NULL;
    }
    else
    {
        if(artist_type == 'b') // Type 2
        {
            int performance_time = rand()%(t2-t1) + t1;

            if(artist_got_stage == 1)
            {
                return NULL;
            }
            
            pthread_mutex_lock(&mutex);
            artist->got_stage = 1;
            pthread_mutex_unlock(&mutex);

            printf("%s has started playing %c on an electric stage for %d seconds\n", artist_name, artist_type, performance_time);
            sleep(performance_time);
            printf("%s has finished the performance at an electric stage\n", artist_name);

            sem_post(&electric_stages);
            check_coordinator(artist_name);
            return NULL; 

        }
        else if(artist_type == 'p' || artist_type == 'g')
        {
            int performance_time = rand()%(t2-t1) + t1;
            
            pthread_mutex_lock(&mutex);
            artist->got_stage = 1;
            pthread_mutex_unlock(&mutex);
            printf("%s has started playing %c on an electric stage for %d seconds\n", artist_name, artist_type, performance_time);
            sleep(performance_time);
            printf("%s has finished the performance at an electric stage\n", artist_name);

            sem_post(&electric_stages);
            check_coordinator(artist_name);
            return NULL; 
        }
        else
        {
            int performance_time = rand()%(t2-t1) + t1;
            
            pthread_mutex_lock(&mutex);
            artist->got_stage = 1;
            pthread_mutex_unlock(&mutex);

            printf("%s has started playing %c on an electric stage for %d seconds\n", artist_name, artist_type, performance_time);
            sleep(performance_time);
            printf("%s has finished the performance at an electric stage\n", artist_name);   

            sem_post(&electric_stages);
            check_coordinator(artist_name);
            return NULL;          
        }

        sem_post(&electric_stages);
        return NULL;    
    }

}

void *check_acoustic_stage(void *p)
{   
    performer* artist = (performer*) p;

    pthread_mutex_lock(&mutex);
    int artist_index = artist->index;
    int artist_arrival_time = artist->arrival_time;
    int artist_got_stage = artist->got_stage;
    char artist_type = artist->type;
    char artist_name[100];
    strcpy(artist_name,"");
    strcpy(artist_name, artist->name);
    pthread_mutex_unlock(&mutex);

    sleep(artist_arrival_time);

    pthread_mutex_lock(&mutex);
    artist_got_stage = artist->got_stage;
    pthread_mutex_unlock(&mutex);

    if(artist_got_stage == 0)
    {
        printf("%s has arrived at Srujana and is looking for an acoustic stage to play %c\n", artist_name, artist_type);
    }

    struct timespec ts;
	if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
	{
	    exit(0);
	}
	ts.tv_sec += t;

    int s = sem_timedwait(&acoustic_stages, &ts);
    if (s == -1)
    {
        if (errno == ETIMEDOUT)	
        {   
            printf("%s left in frustation after %d seconds due to a long waiting time\n", artist_name, t);
            pthread_mutex_lock(&mutex);
            artist->got_stage = 1;
            pthread_mutex_unlock(&mutex);
            return NULL;
        }
    }

    // sem_wait(&acoustic_stages);
    
    //Check if the artist got stage anywhere if yes then terminate
    pthread_mutex_lock(&mutex);
    artist_got_stage = artist->got_stage;
    pthread_mutex_unlock(&mutex);

    if(artist_got_stage == 1)
    {
        return NULL;
    }
    else
    {
        if(artist_type == 'b') // Type 2
        {
            int performance_time = rand()%(t2-t1) + t1;
            
            pthread_mutex_lock(&mutex);
            artist->got_stage = 1;
            pthread_mutex_unlock(&mutex);

            printf("%s has started playing %c on an acoustic stage for %d seconds\n", artist_name, artist_type, performance_time);
            sleep(performance_time);
            printf("%s has finished the performance at an acoustic stage\n", artist_name);

            sem_post(&acoustic_stages);
            check_coordinator(artist_name);
            return NULL;

        }
        else if(artist_type == 'p' || artist_type == 'g')
        {
            int performance_time = rand()%(t2-t1) + t1;
            
            pthread_mutex_lock(&mutex);
            artist->got_stage = 1;
            pthread_mutex_unlock(&mutex);

            printf("%s has started playing %c on an acoustic stage for %d seconds\n", artist_name, artist_type, performance_time);
            sleep(performance_time);
            printf("%s has finished the performance at an acoustic stage\n", artist_name);

            sem_post(&acoustic_stages);
            check_coordinator(artist_name);
            return NULL;

        }
        else
        {
            int performance_time = rand()%(t2-t1) + t1;
            
            pthread_mutex_lock(&mutex);
            artist->got_stage = 1;
            pthread_mutex_unlock(&mutex);

            printf("%s has started playing %c on an acoustic stage for %d seconds\n", artist_name, artist_type, performance_time);
            sleep(performance_time);
            printf("%s has finished the performance at an acoustic stage\n", artist_name); 

            sem_post(&acoustic_stages);
            check_coordinator(artist_name);
            return NULL;           
        }

        sem_post(&acoustic_stages);
        return NULL;
         
    }

}

void *do_nothing(void *p)
{
    return NULL;
}

int main()
{
    srand(time(0));

    scanf("%d %d %d %d %d %d %d", &k, &a, &e, &c, &t1, &t2, &t);

    pthread_mutex_init(&mutex,NULL);    

    sem_init(&acoustic_stages, 0, a);
    sem_init(&electric_stages, 0, e);
    sem_init(&coordinators, 0, c);

    pthread_t electric[k];
    pthread_t acoustic[k];  

    for(int i=0;i<k;i++)
    {
        scanf("%s %c %d", performers[i].name, &performers[i].type, &performers[i].arrival_time ); 
        performers[i].got_stage = 0;
        performers[i].index = i;
    }

    //Creating threads for each performer

    for(int i=0;i<k;i++)
    {
        char type = performers[i].type;
        if(type == 'p' || type == 'g')
        {
            pthread_create(&electric[i],NULL,check_electric_stage,&performers[i]);
            pthread_create(&acoustic[i],NULL,check_acoustic_stage,&performers[i]);
        }

        else if(type == 's')
        {
            pthread_create(&electric[i],NULL,check_electric_stage,&performers[i]);
            pthread_create(&acoustic[i],NULL,check_acoustic_stage,&performers[i]);         
        }
        
        else if(type == 'v')
        {
            pthread_create(&acoustic[i],NULL,check_acoustic_stage,&performers[i]);
            pthread_create(&electric[i],NULL,do_nothing,&performers[i]);
        }

        else
        {
            pthread_create(&electric[i],NULL,check_electric_stage,&performers[i]);  
            pthread_create(&acoustic[i],NULL,do_nothing,&performers[i]);        
        }
        
    }

    for(int i=0; i<k; i++)
    {
		pthread_join(electric[i],NULL);
        pthread_join(acoustic[i],NULL);
    }


    sem_destroy(&acoustic_stages);
    sem_destroy(&electric_stages);
    sem_destroy(&coordinators);

    pthread_mutex_destroy(&mutex);

	printf("Simulation over\n");

    return 0;
    
}