#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"


typedef struct Booth Booth;
typedef struct EVM EVM;
typedef struct Voter Voter;

struct EVM {
	long long int id;
	 Booth * booth;
	long long int number_of_slots;
	long long int check;
	pthread_t polling_evm_id;
};

struct Booth{
	long long int id;
	long long int evm;
	long long int evm_slots;
	long long int voter;
	long long int voters_completed;
	EVM ** evms;
	Voter ** voters;
	pthread_mutex_t mutex;
	pthread_cond_t cv_1;
	pthread_cond_t cv_2;
	pthread_t Polling_booth_id;
};

struct Voter{
	long long int id;
	pthread_t voter_wait_for_evm_id;
	Booth * booth;
	EVM * evm;
	char status;
};


void * voter_wait_for_evm(void * booth_voter)
{
	Voter * voter = (Voter*)booth_voter;
	pthread_mutex_t * mutex_ptr = &(voter->booth->mutex);
	printf(YEL "Voter %lld arrived and waiting for EVM\n"RESET,voter->id );

	pthread_mutex_lock(mutex_ptr);
	voter->status = 'w';

	while(1)
	{
		pthread_cond_t * cv_1_ptr = &(voter->booth->cv_1);
		if(voter->status!='w')
		{
			break;
		}
		else
		{
			pthread_cond_wait(cv_1_ptr, mutex_ptr);
		}
	}

	pthread_mutex_unlock(mutex_ptr);
	EVM * evm = voter->evm;

	printf(BLU"Voter %lld is in the slot waiting for EVM to start\n"RESET ,voter->id);

	pthread_mutex_lock(mutex_ptr);

	while(1)
	{
		pthread_cond_t * cv_1_ptr = &(voter->booth->cv_1);
		if(evm->check!= 0)break;
		else pthread_cond_wait(cv_1_ptr, mutex_ptr);
	}

	pthread_cond_t * cv_2_ptr = &(voter->booth->cv_2);
	evm->number_of_slots-=1;

	printf(CYN"voter  %lld has casted its vote in %lld evm of booth %lld \n"RESET,voter->id,evm->id,evm->booth->id);

	pthread_cond_broadcast(cv_2_ptr);
	pthread_mutex_unlock(mutex_ptr);
}

void * polling_evm(void * booth_evm){

	EVM * evm = (EVM*)booth_evm;
	Booth * booth = evm->booth;

	long long int i;
	long long int count;
	long long int evm_max_slots;
	evm_max_slots=rand()%(booth->evm_slots);

	while(1)
	{
		pthread_mutex_t * mutex_ptr = &(booth->mutex);

		if(booth->voters_completed == booth->voter)
		{
			pthread_mutex_unlock(mutex_ptr);
			break;
		}

		if(booth->voters_completed != booth->voter)
		{
			printf(BLU "EVM %lld of Booth %lld getting free slots to allot to user\n"RESET,evm->id,booth->id);
			count=0;
			pthread_mutex_lock(mutex_ptr);
			evm->number_of_slots = evm_max_slots;
			evm->check = 0;
			pthread_mutex_unlock(mutex_ptr);

			printf("%lld Booth's %lld EVM has %lld slots free.\n", booth->id, evm->id, evm_max_slots);

			while(1)
			{
				if(count<evm_max_slots){
					pthread_mutex_lock(mutex_ptr);
					i = rand() % booth->voter;

					if(booth->voters[i]->status == 'w')
					{
						booth->voters[i]->evm = evm;
						booth->voters_completed=booth->voters_completed+1;
						booth->voters[i]->status = 'a';
						printf(GRN" Voter %lld is alloted %lld EVM at Booth %lld \n"RESET,i,evm->id,booth->id);
						count=count+1;
					}

					if(booth->voters_completed == booth->voter)
					{
						pthread_mutex_unlock(mutex_ptr);
						break;
					}
					pthread_mutex_unlock(mutex_ptr);
				}
				else
				{
					break;
				}
			}
			if(count<=0) break;
			pthread_mutex_lock(mutex_ptr);
			printf(RED "EVM %lld of %lld Booth has started voting phase.\n"RESET,evm->id,booth->id);

			evm->number_of_slots = count;
			evm->check = 1;

			pthread_cond_t * cv_1_ptr = &(booth->cv_1);
			pthread_cond_broadcast(cv_1_ptr);

			while(1){
				pthread_cond_t * cv_2_ptr = &(booth->cv_2);
				if(evm->number_of_slots==0)
					break;
				if(evm->number_of_slots!=0)
					pthread_cond_wait(cv_2_ptr,mutex_ptr);
			}
			pthread_mutex_unlock(mutex_ptr);
			printf("EVM %lld of Booth %lld has finished voting phase.\n",evm->id,booth->id);
		}
	}
	return 0;
}

void * Polling_booth(void* booths)
{
	long long int i;
	Booth * booth = (Booth*)booths;

	for(i=0; i<booth->evm; ++i){
		booth->evms[i] = (EVM*)malloc(sizeof(EVM));
		booth->evms[i]->number_of_slots = 0;
		booth->evms[i]->booth = booth;
		booth->evms[i]->check = 0;
		booth->evms[i]->id = i;
}

	for(i=0; i<booth->voter; ++i){
		booth->voters[i] = (Voter*)malloc(sizeof(Voter));
		booth->voters[i]->evm = NULL;
		booth->voters[i]->booth = booth;
		booth->voters[i]->status ='n';
		booth->voters[i]->id = i;
	}

	for(i=0; i<booth->evm;++i)
		pthread_create(&(booth->evms[i]->polling_evm_id),NULL,polling_evm,booth->evms[i]);

	for(i=0; i<booth->voter;++i)
		pthread_create(&(booth->voters[i]->voter_wait_for_evm_id),NULL, voter_wait_for_evm, booth->voters[i]);

	for(i=0; i<booth->evm;++i)
		pthread_join(booth->evms[i]->polling_evm_id, 0);

	for(i=0; i<booth->voter;++i)
		pthread_join(booth->voters[i]->voter_wait_for_evm_id, 0);

	printf("Booth %lld is Done.\n", booth->id);

	return 0;
}

int main(){
	long long int number_of_booths;
	long long int total_evms;
	long long int total_voters;
	long long int max_evm_slots;
	long long int i;

	printf("Enter number of booths:");
	scanf("%lld", &number_of_booths);

	long long int * evm = (long long int*)malloc(sizeof(long long int)*number_of_booths);
	long long int * voter = (long long int*)malloc(sizeof(long long int)*number_of_booths);
	long long int * evm_slots = (long long int*)malloc(sizeof(long long int)*number_of_booths);

	for(i=0;i<number_of_booths;++i)
	{
		printf("Enter total number of voters for booth no %lld\n",i);
		scanf("%lld", &total_voters);
		voter[i] = total_voters;

		printf("Enter total number of evms for booth no %lld\n",i);
		scanf("%lld", &total_evms);
	  evm[i] = total_evms;

		printf("Enter max slots evm for booth no %lld(<=10)\n",i);
		scanf("%lld", &max_evm_slots);
		evm_slots[i] = max_evm_slots;

	}

	Booth ** booths = (Booth**)malloc(sizeof(Booth*)*number_of_booths);
	printf(RED "\n############################## ELECTION STARTS ################################### \n" RESET);

	for(i=0; i<number_of_booths; ++i){
	booths[i] = (Booth*)malloc(sizeof(Booth));
	booths[i]->evms = (EVM**)malloc(sizeof(EVM*)*evm[i]);
	booths[i]->voters = (Voter**)malloc(sizeof(Voter*)*voter[i]);
	booths[i]->voters_completed = 0;
	booths[i]->evm_slots = evm_slots[i];
	booths[i]->voter = voter[i];
	booths[i]->evm = evm[i];
	booths[i]->id = i;

	pthread_mutex_init(&(booths[i]->mutex), NULL);
	pthread_cond_init(&(booths[i]->cv_1), NULL);
	pthread_cond_init(&(booths[i]->cv_2), NULL);
}

	for(i=0; i<number_of_booths; ++i)
		pthread_create(&(booths[i]->Polling_booth_id),NULL, Polling_booth, booths[i]);

	for(i=0; i<number_of_booths; ++i)
		pthread_join(booths[i]->Polling_booth_id, 0);

	printf(RED "\n########################### ELECTIONS END #################################################\n" RESET );
	return 0;
}
