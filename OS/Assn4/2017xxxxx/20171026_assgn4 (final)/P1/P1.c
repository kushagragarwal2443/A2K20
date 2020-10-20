#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// Input 
int n;

// Entered academy variables

int ref_count = 0;
int player_count = 0;

// State of player and referee threads

pthread_t t_player[200];
pthread_t t_ref[100];

// Global State Arrays
int i = 0, j = 0;
int pstate[200];
int rstate[100];

pthread_mutex_t r_lock[100];
pthread_mutex_t p_lock[200];

pthread_cond_t r_cond[100];
pthread_cond_t p_cond[200];

pthread_t p_queuer;
pthread_t organizer;

typedef struct person
{
    int type; // 0 for ref, 1 for player
    int index;
} person;

typedef struct game
{
    int r;
    int p1;
    int p2;
} game;


int playercheck()
{
    for(int c = 0; c < 2*n; c++)
        if(pstate[c] < 5) return 1;
    return 0;
}

int refcheck()
{
    for(int c = 0; c < n; c++)
        if(rstate[c] < 6) return 1;
    return 0;
}

void enterAcademy(int index, int type)
{
    if(type == 0)
    {
        printf("Referee %d has entered academy\n", index);
        rstate[index]++;
    }
    else 
    {
        printf("Player %d has entered academy\n", index);
        pstate[index]++;
      //  printf("eA %d : %d\n", index, pstate[index]);
    }

}

void joinTrio(int index, int type)
{
    return;
    if(type == 0)
    {
        printf("Referee %d has joined the trio\n", index);
    }
    else 
    {
        printf("Player %d has joined the trio\n", index);
       // printf("jT %d : %d\n", index, pstate[index]);

    }

}
void meetOrganizer(int index, int type)
{
    if(type == 0)
    {
        printf("Referee %d has met organizer\n", index);
    }
    else 
    {
        printf("Player %d has met organizer\n", index);
   // printf("mO %d : %d\n", index, pstate[index]);

    }
}


void enterCourt(int index, int type)
{
    if(type == 0)
    {
        printf("Referee %d has entered court\n", index);
    }
    else 
    {
        printf("Player %d has entered court\n", index);
   // printf("eC %d : %d\n", index, pstate[index]);

    }
}

void adjEq(int index)
{
    sleep(0.5);
    printf("Referee %d has adjusted equipment\n", index);
}

void warmUp(int index)
{
    sleep(1);
    //printf("W %d : %d\n", index, pstate[index]);
    printf("Player %d has warmed up\n", index);
}

void startGame(int index)
{
    printf("Referee %d has started game\n", index);
}

void* ref_func(void* args)
{
    person* argv = (person*)args;

    int i1 = argv->index;
    pthread_mutex_init(&r_lock[i1], NULL);
    pthread_cond_init(&r_cond[i1], NULL);

    pthread_mutex_lock(&r_lock[i1]);
    if(rstate[i1] == 0)
        enterAcademy(i1,0);
    pthread_mutex_unlock(&r_lock[i1]);

    // pthread_mutex_lock(&r_lock[i1]);
    while(rstate[i1] == 1)
        pthread_cond_wait(&r_cond[i1], &r_lock[i1]);
    meetOrganizer(i1,0);
    // pthread_mutex_unlock(&r_lock[i1]);

    // pthread_mutex_lock(&r_lock[i1]);
    while(rstate[i1] == 2)
        pthread_cond_wait(&r_cond[i1], &r_lock[i1]);
    joinTrio(i1,0);
    // pthread_mutex_unlock(&r_lock[i1]);

    // pthread_mutex_lock(&r_lock[i1]);
    while(rstate[i1] == 3)
        pthread_cond_wait(&r_cond[i1], &r_lock[i1]);
    enterCourt(i1,0);
    // pthread_mutex_unlock(&r_lock[i1]);

    // pthread_mutex_lock(&r_lock[i1]);
    while(rstate[i1] == 4)
        pthread_cond_wait(&r_cond[i1], &r_lock[i1]);
    adjEq(i1);
    // pthread_mutex_unlock(&r_lock[i1]);

    // pthread_mutex_lock(&r_lock[i1]);
    while(rstate[i1] == 5)
        pthread_cond_wait(&r_cond[i1], &r_lock[i1]);
    startGame(i1);
    // pthread_mutex_unlock(&r_lock[i1]);

    pthread_exit(NULL);

}

void* pla_func(void* args)
{
    person* argv = (person*)args;

    int i1 = argv->index;
    pthread_mutex_init(&p_lock[i1], NULL);
    pthread_cond_init(&p_cond[i1], NULL);

    pthread_mutex_lock(&p_lock[i1]);
    if(pstate[i1] == 0)
        enterAcademy(i1,1);
    pthread_mutex_unlock(&p_lock[i1]);

    //pthread_mutex_lock(&p_lock[i1]);
    while(pstate[i1] == 1)
        pthread_cond_wait(&p_cond[i1], &p_lock[i1]);
    meetOrganizer(i1,1);
    //pthread_mutex_unlock(&p_lock[i1]);

    // pthread_mutex_lock(&p_lock[i1]);
    while(pstate[i1] == 2)
        pthread_cond_wait(&p_cond[i1], &p_lock[i1]);
    joinTrio(i1,1);
    // pthread_mutex_unlock(&p_lock[i1]);

    // pthread_mutex_lock(&p_lock[i1]);
    while(pstate[i1] == 3)
        pthread_cond_wait(&p_cond[i1], &p_lock[i1]);
    enterCourt(i1,1);
    // pthread_mutex_unlock(&p_lock[i1]);
    
    // pthread_mutex_lock(&p_lock[i1]);
    while(pstate[i1] == 4)
        pthread_cond_wait(&p_cond[i1], &p_lock[i1]);
    warmUp(i1);
    // pthread_mutex_unlock(&p_lock[i1]);

    pthread_exit(NULL);
    return NULL;
}

void* organize(void* args)
{
    int player1 = -1, player2 = -1;
    int referee = -1;

    while(playercheck() && refcheck())
    {    
        for(int c = 0; c < 2*n; c++)
        {
            if(pstate[c] == 2)
            {
                if(player1 == -1 && player2 != c)
                {
                    player1 = c;
                    // printf("Player1(%d) state before joining trio: %d\n", c, pstate[c]);
                    pthread_mutex_lock(&p_lock[c]);
                    pstate[c]++;
                    pthread_mutex_unlock(&p_lock[c]);
                    pthread_cond_signal(&p_cond[c]);
                   // printf("Player1(%d) state after joining trio: %d\n", c, pstate[c]);
                }
                if(player2 == -1 && player1 != c)
                {
                    // printf("Player2(%d) state before joining trio: %d\n", c, pstate[c]);
                    player2 = c;
                    pthread_mutex_lock(&p_lock[c]);       
                    pstate[c]++;
                    pthread_mutex_unlock(&p_lock[c]);

                    pthread_cond_signal(&p_cond[c]);
                    // printf("Player2(%d) state after joining trio: %d\n", c, pstate[c]);
                }
            }

            else if(pstate[c] == 1)
            {
                person targs;
                targs.type = 1;
                targs.index = c;
                //printf("%d about to meet organizer\n", c);
                pthread_mutex_lock(&p_lock[c]);             
                pstate[c]++;
                pthread_mutex_unlock(&p_lock[c]);
                pthread_cond_signal(&p_cond[c]);

            }
            if(player1 != -1 && player2 != -1 && referee != -1) break;
        }

        for(int c = 0; c < n; c++)
        {
            if(rstate[c] == 2)
            {
                if(referee == -1)
                {
                    referee = c;
                    pthread_mutex_lock(&r_lock[c]);    
                    rstate[c]++;
                    pthread_mutex_unlock(&r_lock[c]);
                    pthread_cond_signal(&r_cond[c]);

                }
            }

            else if(rstate[c] == 1)
            {
                person targs;
                targs.type = 0;
                targs.index = c;

                pthread_mutex_lock(&r_lock[c]);
                rstate[c]++;
                pthread_mutex_unlock(&r_lock[c]);
                pthread_cond_signal(&r_cond[c]);

            }

            if(player1 != -1 && player2 != -1 && referee != -1) break;
        }

        if(player1 != -1 && player2 != -1 && referee != -1)
        {
            pthread_mutex_lock(&p_lock[player1]);
            pstate[player1]++;
            pthread_mutex_unlock(&p_lock[player1]);
            pthread_cond_signal(&p_cond[player1]);

            pthread_mutex_lock(&p_lock[player2]);
            pstate[player2]++;
            pthread_mutex_unlock(&p_lock[player2]);
            pthread_cond_signal(&p_cond[player2]);

            pthread_mutex_lock(&r_lock[referee]);  
            rstate[referee]++;             
            pthread_mutex_unlock(&r_lock[referee]);
            pthread_cond_signal(&r_cond[referee]);

            pthread_mutex_lock(&p_lock[player1]);
            pstate[player1]++;
            pthread_mutex_unlock(&p_lock[player1]);
            pthread_cond_signal(&p_cond[player1]);

            pthread_mutex_lock(&p_lock[player2]);
            pstate[player2]++;
            pthread_mutex_unlock(&p_lock[player2]);
            pthread_cond_signal(&p_cond[player2]);

            pthread_mutex_lock(&r_lock[referee]);  
            rstate[referee]++;             
            pthread_mutex_unlock(&r_lock[referee]);
            pthread_cond_signal(&r_cond[referee]);
            
            
            int t2 = pthread_join(t_player[player2], NULL);
            int t1 = pthread_join(t_player[player1], NULL);
            
            pthread_mutex_lock(&r_lock[referee]);
            rstate[referee]++;             
            pthread_mutex_unlock(&r_lock[referee]);
            pthread_cond_signal(&r_cond[referee]);

            t1 = pthread_join(t_ref[referee], NULL);

            printf("***GAME: P%d vs P%d ; Ref %d***\n", player1, player2, referee);
            player1 = player2 = referee = -1;
        }
    }
}

void* pqueue_fill(void* args)
{
    while(1)
    {
        if(!(3*n - ref_count - player_count)) break;
        int u = rand()%(3*n - ref_count - player_count);

        if(u < n - ref_count && ref_count < n || player_count == 2*n && ref_count < n)    // Add a refree
        {
            ref_count++;
            person targs;
            targs.type = 0;
            targs.index = i;

            pthread_create(&t_ref[i], NULL, ref_func, (void*)(&targs));
            i++;
        }
        else if(player_count < 2*n)     //Add a player
        {
            player_count++;
            person targs;
            targs.type = 1;
            targs.index = j;
            pthread_create(&t_player[j], NULL, pla_func, (void*)(&targs));
            j++;
        }
        else break;

        sleep(rand()%3);
    }
    printf("> All persons have entered academy <\n");
}

int main()
{
    scanf("%d",&n);

    int ret1 = pthread_create(&p_queuer, NULL, pqueue_fill, NULL);
    int ret2 = pthread_create(&organizer, NULL, organize, NULL);

    pthread_join(p_queuer, NULL);
    pthread_join(organizer, NULL);
    printf("> Organizer is done for the day <\n");

    return 0;
}
