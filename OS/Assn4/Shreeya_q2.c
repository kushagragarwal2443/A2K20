#include<pthread.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<time.h>

pthread_t pid[100000], zid[100000], sid[200000];
pthread_mutex_t mutex_pharm, mutex_student, mutex_zone;

typedef long long ll;

typedef struct pharms
{
	ll pharm_id;
	ll batches;
    float prob;
} pharms;

typedef struct zone
{
	ll zone_id;
	ll slots;
	ll capacity;
    ll company_id;
} zones;

typedef struct students
{
	ll student_id;
	ll state;
    ll vacc_round;
} students;

ll m, n, o;
ll w, p ,r;
ll arrival_time, current;
ll  waiting;

pharms pharm[10010];
students student[20010];
zones zone[201010];



ll min(ll  a,ll  b)
{
	if(a>b)
		return b;
	else
		return a;
}


// ================================= Pharmaceutical Companies ================================= 
void batch_ready(ll  id)
{
	while(1)
	{
		pthread_mutex_lock(&mutex_pharm);
		if(pharm[id].batches == 0)
		{
			printf("All the vaccines prepared by Pharmaceutical Company %lld are emptied. Resuming production now.\n", id);
			pthread_mutex_unlock(&mutex_pharm);
			return;
		}
        else{
            pthread_mutex_unlock(&mutex_pharm);
        }
	}
}
void *prepare(void *arg)
{
    pharms *pind = (pharms *) arg;

    w = rand()%4 + 2;
	r = rand()%5 + 1;

	while(1)
	{
		printf("Pharmaceutical Company %lld is preparing %lld batches of vaccines which have success probability %f\n", pind->pharm_id, r, pind->prob);
		sleep(w);
		printf("Pharmaceutical Company %lld has prepared %lld batchesof vaccines which have success probability %f.\nWaiting for all the vaccines to be used to resume production\n", pind->pharm_id, r, pind->prob);

		pthread_mutex_lock(&mutex_pharm);
		pharm[pind->pharm_id].batches=r;
		pthread_mutex_unlock(&mutex_pharm);

		sleep(1);
		batch_ready(pind->pharm_id);
	}
}


// ================================= Students ================================= 
int student_in_slot(ll ta,ll id)
{
    sleep(2);
    int flag = 0;

    printf("Student %lld on Vaccination Zone %lld has been vaccinated which has success probability %f\n", id, ta, pharm[zone[ta].company_id].prob);

    float chance = rand()%100 + 1;
    chance = chance/100;

    if(chance <= pharm[zone[ta].company_id].prob)
    {
        printf("Student %lld has tested POSITIVE for antibodies.\n", id);
        pthread_mutex_lock(&mutex_zone);
        current--;
        o--;
        zone[ta].capacity--;
        waiting--;
        pthread_mutex_unlock(&mutex_zone);
        return 1;
    }
    else if(student[ta].vacc_round >= 3)
    {
        printf("Student %lld has completed 3 rounds - not applicable.\n", id);
        pthread_mutex_lock(&mutex_zone);
        current--;
        o--;
        zone[ta].capacity--;
        waiting--;
        pthread_mutex_unlock(&mutex_zone);
        return 1;
    }
    else
    {
        printf("Student %lld has tested NEGATIVE for antibodies.\n", id);
        pthread_mutex_lock(&mutex_zone);
        zone[ta].capacity--;
        pthread_mutex_unlock(&mutex_zone);
        return 0;
    }
}
void *wait_for_slot(void *arg)
{
	sleep(arrival_time);
	ll flag=0,i;
    int f = 0;
	students *st  = (students *)arg;
	while(f!=1)
    {
        printf("Student %lld has arrived for his %lld round of Vaccination\n",st->student_id, st->vacc_round);
        pthread_mutex_lock(&mutex_student);
        st->vacc_round++;
        current++;
        pthread_mutex_unlock(&mutex_student);
        int a=0;
        while(1)
        {
            pthread_mutex_lock(&mutex_student);
            for(i=0;i<n;i++)
            {
                if(zone[i].slots > 0)
                {
                    flag=1;
                    zone[i].slots--;
                    printf("Student %lld allocated the zone %lld\n",st->student_id,i);
                    break;
                }
            }
            pthread_mutex_unlock(&mutex_student);
            if(flag==1)
                break;
            else if(a==0)
            {
                waiting++;
                printf("Student %lld is waiting to be allocated a slot on a Vaccination Zone\n",st->student_id);
            }
            a++;
        }
        f = student_in_slot(i,st->student_id);
    }
}


// ================================= Vaccination Zones ================================= 
void ready_to_serve_zone(ll id,ll num)
{
	while(1)
	{
		if(zone[id].slots==0 || zone[id].capacity==0 || current==0 || waiting==0)
			return;
	}
}
void *get(void *arg)
{
	ll flag=0,flag2=0,i;
	zones *curr_zone = (zones *)arg;
	while(1)
	{
		while(1)
		{
			pthread_mutex_lock(&mutex_pharm);
			for(i=0; i<m; i++)
			{
				if(pharm[i].batches > 0 && zone[curr_zone->zone_id].slots==0)
				{
					printf("Pharmaceutical Company - %lld is sending a batch to Zone - %lld\n",i,curr_zone->zone_id);
					pharm[i].batches--;
					sleep(2);
					pthread_mutex_lock(&mutex_zone);
                    zone[curr_zone->zone_id].company_id=i;
                    p = rand()%11 + 10;
					zone[curr_zone->zone_id].capacity=p;
					printf("Vaccination Zone %lld entering Vaccination Phase\n",curr_zone->zone_id);
					pthread_mutex_unlock(&mutex_zone);
					flag=1;
					break;
				}
			}
			pthread_mutex_unlock(&mutex_pharm);
			if(flag==1)
				break;
		}
		while(zone[curr_zone->zone_id].capacity > 0)
		{
			pthread_mutex_lock(&mutex_zone);
			ll num=min(zone[curr_zone->zone_id].capacity,(rand()%10)+1);
            num = min(num, waiting);
			zone[curr_zone->zone_id].slots=num;
			printf("Vaccination Zone %lld is ready to vaccinate with %lld slots\n",curr_zone->zone_id,num);
			pthread_mutex_unlock(&mutex_zone);
			ready_to_serve_zone(curr_zone->zone_id,num);
			//pthread_mutex_lock(&mutex_zone);
			if(waiting == 0 || current==0 || o==0)
			{
				flag2=1;
				printf("No waiting Students\n");
				zone[curr_zone->zone_id].capacity=0;
				printf("Vaccination Zone %lld has run out of vaccines\n",curr_zone->zone_id);
				break;
			}
			if(zone[curr_zone->zone_id].capacity==0)
			{
				printf("Vaccination Zone %lld has run out of vaccines\n",curr_zone->zone_id);
				break;
			}
			//pthread_mutex_unlock(&mutex_zone);
		}
		if(flag2==1)
			break;
	}
}


int main()
{
	srand(time(0));

	printf("Enter number of ‘n’ pharmaceutical companies, ‘m’ vaccination zones, ‘o’ students\n");
	scanf("%lld %lld %lld", &m, &n, &o);

	ll i;

	// Intialise all IDs and status
	for(i=0; i<m; i++)
	{
		pharm[i].pharm_id = i;
		pharm[i].batches = 0;
        pharm[i].prob = 0;
	}
	for(i=0; i<n; i++)
	{
		zone[i].zone_id = i;
		zone[i].slots = 0;
		zone[i].capacity = 0;
        zone[i].company_id = -1;
	}
	for(i=0; i<o; i++)
	{
		student[i].state = -1;
		student[i].student_id = i;
        student[i].vacc_round = 1;
	}

    printf("Enter ‘n’ probabilities of the success rate of each vaccine\n");
	float prob;
    for(i=0; i<m; i++){
        scanf("%f", &prob);
        pharm[i].prob = prob;
    }
    printf("\n");

    current = 0;
    waiting = 0;

	for(i=0; i<m; i++)
		pthread_create(&pid[i], NULL, prepare, &pharm[i]);

	for(i=0; i<n; i++)
		pthread_create(&zid[i], NULL, get, &zone[i]);

	for(i=0; i<o; i++)
	{
		arrival_time = rand()%11 + 1;
		pthread_create(&sid[i], NULL, wait_for_slot, &student[i]);
	}

	for(i=0; i<o; i++)
		pthread_join(sid[i],NULL);

    printf("Simulation Over.\n");

	return 0;
}
