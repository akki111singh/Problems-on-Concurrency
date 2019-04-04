#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

int new_player=-1;
int waiting_player=-2;
int assigned_player=+1;
int completed_player=+2;
int count=0;

typedef struct Academy Academy;
typedef struct REFREE REFREE;
typedef struct PLAYER PLAYER;

struct REFREE
{
	int idx;
	pthread_t ref_thread_id;
	Academy * acad;
	int number_of_slots;
	int flag;

};

struct PLAYER
{
	int idx;
	pthread_t player_thread_id;
	Academy * acad;
	REFREE * refree;
	int status;
};


struct Academy
{
	int idx;
	pthread_t acad_thread_id;
	pthread_mutex_t mutex;
	pthread_cond_t cv_1;
	pthread_cond_t cv_2;
	pthread_cond_t cv_3;
	int refree;
	int ref_slots;
	int player;
	int done_players;
	REFREE ** refrees;
	PLAYER ** players;
};

PLAYER* player_init(PLAYER* player,Academy* acad,int idx)
{
	player = (PLAYER*)malloc(sizeof(PLAYER));
	player->refree = NULL;
	player->acad = acad;
	player->status = new_player;
	player->idx = idx;
	return player;
}

void * ref_thread(void * args){

	int i, j, max_slts=2;

	REFREE * refree = (REFREE*)args;
	Academy * acad = refree->acad;

	while(1)
	{
		pthread_mutex_t * mutex_ptr = &(acad->mutex);
		pthread_cond_t * cv_3_ptr = &(acad->cv_3);
		if(acad->done_players == acad->player)
		{
      count=0;
      pthread_cond_broadcast(cv_3_ptr);
			pthread_mutex_unlock(mutex_ptr);
			break;
		}
		else
		{
			printf(GRN"Refree %d entered the academy\n"RESET,refree->idx);
			j = 0;
			pthread_mutex_lock(mutex_ptr);

      count=0;
      pthread_cond_broadcast(cv_3_ptr);
			refree->flag = 0;
			refree->number_of_slots = max_slts;
      pthread_mutex_unlock(mutex_ptr);

			while(1)
			{
				if(j==2)
				{
          count=0;
          pthread_cond_broadcast(cv_3_ptr);
          printf(YEL "Refree %d met the Organiser\n"RESET ,refree->idx);
          		break;
				}
				else
				{
					pthread_mutex_lock(mutex_ptr);
					i = rand()%acad->player;
					if(acad->players[i]->status == waiting_player)
					{
						acad->players[i]->refree = refree;
						(acad->done_players)+=1;
            count++;
            while(1)
            {
              if(count<=2)
              {
                printf(BLU "Player %d met with the organiser\n"RESET,i);
                break;
              }
              else
              {
                pthread_cond_wait(cv_3_ptr,mutex_ptr);
              }
            }
						acad->players[i]->status = assigned_player;
						j++;
					}

					if(acad->done_players == acad->player)
					{
              count=0;
              pthread_cond_broadcast(cv_3_ptr);
              printf(BLU "Refree  %d met the Organiser\n" RESET,refree->idx);
						pthread_mutex_unlock(mutex_ptr);
						break;
					}
					pthread_mutex_unlock(mutex_ptr);
				}
			}

			if(j==0) break;
			pthread_cond_t * cv_1_ptr = &(acad->cv_1);
			pthread_cond_t * cv_2_ptr = &(acad->cv_2);

			pthread_mutex_lock(mutex_ptr);
			printf("Refree %d has entered the court\n",refree->idx);
			printf(RED "Refree %d started adjusting the equipments\n"RESET,refree->idx);
			sleep(.5);
			pthread_mutex_unlock(mutex_ptr);


			pthread_mutex_lock(mutex_ptr);

			refree->number_of_slots = j;
			refree->flag = 1;

			pthread_cond_broadcast(cv_1_ptr);

			while(1)
			{
				if(refree->number_of_slots==0)
				{
					break;
				}
				else
				{
					pthread_cond_wait(cv_2_ptr,mutex_ptr);
				}
			}
			pthread_mutex_unlock(mutex_ptr);

			printf(CYN "Refree %d done with its match.\n" RESET ,refree->idx);
		}
	}
	return NULL;
}
void * player_thread(void * args)
{
		PLAYER * player = (PLAYER*)args;

		printf(YEL "Player %d entered the academy\n" RESET ,player->idx);
		pthread_cond_t * cv_1_ptr = &(player->acad->cv_1);

		pthread_mutex_t * mutex_ptr = &(player->acad->mutex);

	pthread_mutex_lock(mutex_ptr);
	player->status = waiting_player;

	while(1)
	{
		if(player->status!=waiting_player)
		{
			break;
		}
		else
		{
			pthread_cond_wait(cv_1_ptr, mutex_ptr);
		}
	}

	pthread_mutex_unlock(mutex_ptr);

			pthread_cond_t * cv_2_ptr = &(player->acad->cv_2);
	REFREE * refree = player->refree;

	pthread_mutex_lock(mutex_ptr);
	printf("Player %d has entered the court\n",player->idx);
	printf(RED "Player %d has started warming up\n"RESET ,player->idx);
	sleep(1);
	pthread_mutex_unlock(mutex_ptr);

	while(1)
	{
		if(refree->flag == 0)
		{
			pthread_cond_wait(cv_1_ptr, mutex_ptr);
		}
		else
		{
			break;
		}
	}
		(refree->number_of_slots)-=1;
		pthread_cond_broadcast(cv_2_ptr);
		pthread_mutex_unlock(mutex_ptr);
}

void * acad_thread(void* args)
{
	Academy * acad = (Academy*)args;

	for(int i=0; i<acad->refree; i++)
	{
		acad->refrees[i] = (REFREE*)malloc(sizeof(REFREE));
		acad->refrees[i]->number_of_slots = 0;
		acad->refrees[i]->acad = acad;
		acad->refrees[i]->flag = 0;
		acad->refrees[i]->idx = i;
	}

	for(int i=0; i<acad->player; i++)
	{
		acad->players[i] = (PLAYER*)malloc(sizeof(PLAYER));
		acad->players[i]->refree = NULL;
		acad->players[i]->acad = acad;
		acad->players[i]->status = new_player;
		acad->players[i]->idx = i;
	}

  int n_players=acad->player;
  int n_refree=acad->refree;
  int v=0,r=0;

  while(1)
  {

  float prob=(float)n_players/((float)n_players+(float)n_refree);

  if(n_players==0 && n_refree==0)
  {
    break;
  }

  if(prob>0.5)
  {
    pthread_create(&(acad->players[v]->player_thread_id),NULL, player_thread, acad->players[v]);
		sleep(rand()%3);
    v++;
    n_players--;
  }

    if(prob<0.5)
    {
      pthread_create(&(acad->refrees[r]->ref_thread_id),NULL, ref_thread, acad->refrees[r]);
			sleep(rand()%3);
      r++;
      n_refree--;
    }

     if(prob==0.5)
     {
       int k=rand()%10;
       if(k<5)
       {
				 pthread_create(&(acad->players[v]->player_thread_id),NULL, player_thread, acad->players[v]);
				 sleep(rand()%3);
         v++;
         n_players--;
       }
       else
       {
         pthread_create(&(acad->refrees[r]->ref_thread_id),NULL, ref_thread, acad->refrees[r]);
				 sleep(rand()%3);
         r++;
         n_refree--;
       }
     }
  }

	for(int i=0; i<acad->refree; i++)
		pthread_join(acad->refrees[i]->ref_thread_id, 0);

	for(int i=0; i<acad->player; i++)
		pthread_join(acad->players[i]->player_thread_id, 0);

	return NULL;
}

int main()
{

	int badminton_academy=1,total_refrees,total_players,max_ref_slots;
	printf("Enter number of Refree:");
	scanf("%d", &total_refrees);


	int * ref = (int*)malloc(sizeof(int)*badminton_academy);
	int * Player = (int*)malloc(sizeof(int)*badminton_academy);
	int * ref_slots = (int*)malloc(sizeof(int)*badminton_academy);


	for(int i=0; i<badminton_academy; i++)
	{
		total_players=2*total_refrees;
	  max_ref_slots=2;
		ref[i] = total_refrees;
		ref_slots[i] = max_ref_slots;
		Player[i] = total_players;
	}

	Academy ** acadmy = (Academy**)malloc(sizeof(Academy*)*badminton_academy);

	for(int i=0; i<badminton_academy; i++)
	{
		acadmy[i] = (Academy*)malloc(sizeof(Academy));
		pthread_mutex_init(&(acadmy[i]->mutex), NULL);

		acadmy[i]->refrees = (REFREE**)malloc(sizeof(REFREE*)*ref[i]);
			acadmy[i]->player = Player[i];
			acadmy[i]->refree = ref[i];
			acadmy[i]->idx = i;
			acadmy[i]->done_players = 0;
			acadmy[i]->ref_slots= ref_slots[i];
			pthread_cond_init(&(acadmy[i]->cv_1), NULL);
			pthread_cond_init(&(acadmy[i]->cv_2), NULL);
			pthread_cond_init(&(acadmy[i]->cv_3),NULL);
		acadmy[i]->players = (PLAYER**)malloc(sizeof(PLAYER*)*Player[i]);

	}

	for(int i=0; i<badminton_academy; i++)
		pthread_create(&(acadmy[i]->acad_thread_id),NULL, acad_thread, acadmy[i]);

	for(int i=0; i<badminton_academy; i++)
		pthread_join(acadmy[i]->acad_thread_id, 0);

	return 0;
}
