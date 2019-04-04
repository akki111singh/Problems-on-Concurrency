#include<stdio.h>
#include<stdlib.h>
#include<sys/shm.h>
#include<sys/types.h>
#include <unistd.h>
#include<sys/stat.h>
#include<sys/wait.h>

long long int *arr;

void selectionSort(long long int l, long long int r)
{
  long long int smallesti,temp,j;
  for(int i=l; i<=r; i++)
  {
    smallesti= i;
    for(j=i+1; j<=r; j++)
      if(arr[smallesti]>arr[j])
        smallesti= j;

    temp = arr[smallesti];
    arr[smallesti] = arr[i];
    arr[i] = temp;
  }
}


void merge(long long int start,long long int end,long long int mid)
{
	long long int aux[200005];
	long long int l=start,r=mid+1;
	long long int j,i=0;

	while(l<=mid && r<=end)
	{
		if(arr[l]<=arr[r])
			aux[i++]=arr[l++];
		else
			aux[i++]=arr[r++];
	}
	while(l<=mid) aux[i++]=arr[l++];
	while(r<=end) aux[i++]=arr[r++];

	for(j=0;j<i;j++)
		arr[start+j]=aux[j];
	return;
}
void mergesort(long long int start,long long int end)
{
	if(start>=end) return;
	long long int len;

	len=(end-start);
	if((len)<=5){
	 selectionSort(start,end);
	 return;
	}

	int status;
	pid_t l_pid,r_pid;
	long long int mid=(start+end)/2;

	l_pid=fork();
	if(l_pid<0)
	{
		perror("LEFT CHILD PROCESS NOT CREATED\n");
		_exit(-1);
	}
	else if(l_pid==0)
	{
		mergesort(start,mid);
		_exit(0);
	}
	else
	{
	r_pid=fork();
	if(r_pid<0)
	{
		perror("RIGHT CHILD PROCESS NOT CREATED\n");
		_exit(-1);
	}
	else if(r_pid==0){
		mergesort(mid+1,end);
		_exit(0);
	}
}
	 waitpid(l_pid, &status, 0);
	 waitpid(r_pid, &status, 0);
	 merge(start,end,mid);

}
int main(){
	long long int n,i,shmid;
	scanf("%lld",&n);

  if((shmid = shmget( IPC_PRIVATE, sizeof(long long int)*(n),  IPC_CREAT | 0666)) < 0){
    perror("shmget");
    _exit(1);
	}

  if(!(arr=shmat(shmid, NULL, 0))){
      perror("shmat");
      _exit(1);
		}

  for(int i=0; i<n; i++)
    scanf("%lld",&arr[i]);

  mergesort(0, n-1);

	for(int i=0; i<n; i++)
		printf("%d ",arr[i]);

  if (shmdt(arr) == -1){
       perror("shmdt");
       _exit(1);
		 }

  if (shmctl(shmid, IPC_RMID, NULL) == -1){
       perror("shmctl");
       _exit(1);
		 }
return 0;
}
