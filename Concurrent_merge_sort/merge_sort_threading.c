#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

long long int* arr;

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

typedef struct array{
    long long int start;long long int end;
} ARRAY;

void merge(long long int start,long long int end,long long int mid)
{
  long long int aux[end-start+1];
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

void * mergesort(void *a)
{
	   long long int len;
     long long int i,mid;
     ARRAY *new_arr = (ARRAY *)a;
     mid=(new_arr->start+ new_arr->end)/2;

	    len=(new_arr->end-new_arr->start);
	     if((len)<=5){
	        selectionSort(new_arr->start,new_arr->end);
          return 0;
	      }

        ARRAY thread_arr[2];
        pthread_t thread[2];

        thread_arr[0].start = new_arr->start;
        thread_arr[0].end = mid;

        thread_arr[1].start = mid+1;
        thread_arr[1].end = new_arr->end;

        if (new_arr->start < new_arr->end){
        pthread_create(&thread[0], NULL, mergesort, &thread_arr[0]);
        pthread_create(&thread[1], NULL, mergesort, &thread_arr[1]);

        for(i=0;i<2;++i)
        pthread_join(thread[i],NULL);

        merge(new_arr->start,new_arr->end,mid);
      }
        pthread_exit(NULL);
        return 0;
}

int main()
{
        long long int n,i;
        scanf("%lld",&n);
        arr = malloc(sizeof(long long int)*(n+10));

        for(i=0;i<n;i++)
        scanf("%lld",&arr[i]);

        ARRAY ai;
        ai.start = 0;
        ai.end = n-1;
        pthread_t thread;

        pthread_create(&thread, NULL, mergesort, &ai);
        pthread_join(thread, NULL);


        for (i = 0; i <n; i++) printf ("%lld ", arr[i]);

        return 0;
}
