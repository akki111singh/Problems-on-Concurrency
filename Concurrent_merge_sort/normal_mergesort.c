#include<stdio.h>
#include<stdlib.h>
long long int *arr;
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
void mergesort(long long int start,long long int end)
{
	long long int mid=(start+end)/2;
	if(start<end)
	{
		mergesort(start,mid);
		mergesort(mid+1,end);
		merge(start,end,mid);
	}
	return;
}
int main()
{
	long long int n,i;
	scanf("%lld",&n);
  arr = malloc(sizeof(long long int)*(n+5));

	for(i=0;i<n;i++)
		scanf("%lld",&arr[i]);
	mergesort(0,n-1);
  
  for(i=0;i<n;i++)
	printf("%lld ",arr[i]);
	return 0;
}
