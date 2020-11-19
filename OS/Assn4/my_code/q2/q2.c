#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>

int pharmanum;
int zonenum;
int stunum;

struct indx
{
	int inx;
};
typedef struct indx indx;

typedef struct pharma_info
{
	int prob;

}pharma_info;

typedef struct center_info
{
    int which_pharma;

}center_info;

int student_visits[100];
center_info center_infos[100];
pharma_info pharma_infos[100];


int pharma[100];
int zone[100];
int zone_slot[100];


pthread_mutex_t mutex1;
pthread_mutex_t mutex2;

pthread_mutex_t mutex3;
pthread_mutex_t mutex4;


void vaccine_ready(int ind,int p,int r)
{
	int i;

	while(r>0)
	{ 
		pthread_mutex_lock(&mutex1);

        //Entering critical section

		for(i=0;i<zonenum;i++)
		{
            if(zone[i]==0)
			{
                zone[i] += p;
				r--;

				printf("Pharmaceutical Company %d is delivering a vaccine batch to Vaccination Zone %d which has success probability %d\n", ind, i+1, pharma_infos[ind-1].prob);
                
                pthread_mutex_lock(&mutex3);
                center_infos[i].which_pharma = ind;
                pthread_mutex_unlock(&mutex3);
               
                if(r<=0)
					break;

			}
		}

		pthread_mutex_unlock(&mutex1);

	}
}

void* produce_vaccines(void * input)
{
    indx * in=(indx*) input;
	int ind=in->inx;

	int p = rand()%11 + 10; // Between 10-20 ( size of batch )
	int r = rand()%5 + 1; // Between 1-5 ( num of batches )

    int sleeptime = rand()%4 + 2; // Between 2-5

	int num=r;
    int i;

	while(1)
	{
        printf("Pharmaceutical Company %d is preparing %d batches of vaccines which have success probability %d\n", ind, r, pharma_infos[ind-1].prob);
        sleep(sleeptime);
        printf("Pharmaceutical Company %d has prepared %d batches of vaccines which have success probability %d\n", ind, r, pharma_infos[ind-1].prob);
        num=r;
        vaccine_ready(ind,p,num);
        printf("All the vaccines prepared by Pharmaceutical Company %d are emptied. Resuming production now\n", ind);
	}

}

void vaccination_phase(int ind)
{

    printf("Vaccination Zone %d entering Vaccination Phase\n", ind);

	int flag = 0;

	while(flag != 1)
	{
		pthread_mutex_lock(&mutex2);

        // Entering Critical Section now

		if(zone_slot[ind-1]==0) // Do not let this zone get new students till this slot is finished
        {
			flag=1;
            printf("Vaccination Zone %d leaving Vaccination Phase\n", ind);
        }

		pthread_mutex_unlock(&mutex2);
	}
}

void* book_zones(void * input)
{
    indx * in=(indx*) input;
	int ind=in->inx;
    
    int range;
    int flag = 0;
    int i,slot;

	while(1)
	{
        flag = 0;
		while(flag!=1)
		{     
			pthread_mutex_lock(&mutex1);
			pthread_mutex_lock(&mutex2);

			if(zone[ind-1]>0) // If we find a Zone with vaccines
			{ 
                range = zone[ind-1]%8 + 1;

				flag = 1;
				slot = rand()%range+1;
				zone_slot[ind-1] += slot;
			}

			if(flag==1)
            {
                printf("Vaccination Zone %d is ready to vaccinate with %d slots\n", ind, slot);
            }
			
            pthread_mutex_unlock(&mutex2);
			pthread_mutex_unlock(&mutex1);
		}

		vaccination_phase(ind);

	}
}

void wait_for_slot(int ind)
{
	int flag = 0;
    int i;

    printf("Student %d is waiting to be allocated a slot on a Vaccination Zone\n", ind);

	while(flag!=1)
	{
		pthread_mutex_lock(&mutex1);
		pthread_mutex_lock(&mutex2);

        //Entering Crirical Section

		for(i = 0; i < zonenum; i++)
		{ 
            if(zone_slot[i]>0)
			{
				zone_slot[i]--;
				flag=1;

                printf("Student %d assigned a slot on the Vaccination Zone %d and waiting to be vaccinated\n", ind, i+1);
                sleep(0.1);

                int probability;

                pthread_mutex_lock(&mutex3);
                probability = pharma_infos[center_infos[i].which_pharma -1].prob;
                pthread_mutex_unlock(&mutex3);

				printf("Student %d on Vaccination Zone %d has been vaccinated which has success probability %d\n", ind, i+1, probability);
	            zone[i]--;

                pthread_mutex_lock(&mutex4);
                int visits_stud = student_visits[ind];
                pthread_mutex_unlock(&mutex4);
                
                int chance = rand()%100 + 1;
                if(chance <= probability)
                {
                    printf("Student %d has tested positive for antibodies\n", ind);
                    break;
                }
                else if(visits_stud == 3)
                {
                    printf("Student %d has tested negative for antibodies but completed 3rd attempt, hence being sent back home\n", ind);
                    break;
                }
                else
                {
                    printf("Student %d has tested negative for antibodies\n", ind);
                    flag = 0;

                    pthread_mutex_lock(&mutex4);
                    visits_stud = ++student_visits[ind];
                    pthread_mutex_unlock(&mutex4);

                    printf("Student %d has arrived for his round %d of Vaccination\n",ind, visits_stud);
                }
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

	int student_sleep = rand() % 5;
	sleep(student_sleep);

    pthread_mutex_lock(&mutex4);
    student_visits[ind]++;
	printf("Student %d has arrived for his round %d of Vaccination\n",ind, student_visits[ind]);
    pthread_mutex_unlock(&mutex4);

	wait_for_slot(ind);
}

int main()
{       
    scanf("%d %d %d",&pharmanum,&zonenum,&stunum);

    float input_probs;

    for(int i = 0; i<pharmanum; i++)
    {
        scanf("%f", &input_probs);
        pharma_infos[i].prob = (int)(input_probs*100);
    }

    pthread_t ch[pharmanum];
    pthread_t ta[zonenum];
	pthread_t stu[stunum];

    srand(time(0));

    pthread_mutex_init(&mutex1,NULL);
	pthread_mutex_init(&mutex2,NULL);
    pthread_mutex_init(&mutex3,NULL);
    pthread_mutex_init(&mutex4,NULL);
    
	indx cc[pharmanum];
    indx dd[zonenum];
	indx ee[stunum];

	int i;

	for(i=0;i<pharmanum;i++)
	{       
        cc[i].inx=i+1;
		pthread_create(&ch[i],NULL,produce_vaccines,(void*)&cc[i]);
	}

	for(i=0;i<zonenum;i++)
	{       
        dd[i].inx=i+1;
		pthread_create(&ta[i],NULL,book_zones,(void*)&dd[i]);
	}

	for(i=0;i<stunum;i++)
	{       
        student_visits[i] = 0;
        ee[i].inx=i+1;
		pthread_create(&stu[i],NULL,student,(void*)&ee[i]);
	}

	for(i=0;i<stunum;i++)
		pthread_join(stu[i],NULL);


	pthread_mutex_destroy(&mutex1);
	pthread_mutex_destroy(&mutex2);
    pthread_mutex_destroy(&mutex3);
    pthread_mutex_destroy(&mutex4);

	printf("Simulation over\n");
}