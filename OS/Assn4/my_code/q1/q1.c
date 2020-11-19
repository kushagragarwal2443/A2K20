#define _POSIX_C_SOURCE 199309L //required for clock
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <limits.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <inttypes.h>
#include <math.h>

typedef struct arg{
     int l;
     int r;
     int* arr;
}arg;

pthread_mutex_t lock;

int a[10000];
// int b[10000];
// int c[10000];

int * shareMem(size_t size){
     key_t mem_key = IPC_PRIVATE;
     int shm_id = shmget(mem_key, size, IPC_CREAT | 0666);
     return (int*)shmat(shm_id, NULL, 0);
}


/////////////////////////////////// Normal Merge Sort Starts //////////////////////////////////////////////////
void Normal_SelectionSort(int a[],int l,int r)
{
    for(int i=l; i<r; i++)
    {
        int min_pos = i;
        for(int j=i+1;j<=r;j++)
        {
            if(a[j]<a[min_pos])
                min_pos = j;
        }

        int swap = a[min_pos];
        a[min_pos] = a[i];
        a[i] = swap;
    }
}
void Normal_Merge(int a[],int l,int m,int r)
{
	int size=r-l+1;
	int K[size];
	//temporary array used is K from indices 0 to size-1
	int i=l;
	int j=m+1;

	for(int z=0;z<size;z++)
	{
		if(i<=m&&j<=r)
		{
            if(a[i]<a[j])
            {
                K[z]=a[i];
                i++;
            }
            else
            {
                K[z]=a[j];
                j++;
            }
		}
		else
		{
			if(i>m)
			{
				for(int yo=j;yo<=r;yo++)
				{
					K[z]=a[yo];
					z++;
				}
				break;
			}
			else
			{
				for(int yo=i;yo<=m;yo++)
                {
                        K[z]=a[yo];
                        z++;
                }
                break;
			}
		}
	}
	for(int z=0;z<size;z++)
	{
		a[l+z]=K[z];
	}
}
void Normal_MergeSort(int a[],int l,int r)
{
	if(l<r)
	{
        if(r-l+1<5)
        {
            Normal_SelectionSort(a,l,r);
            return;
        }
		int m=l+(r-l)/2;
	    Normal_MergeSort(a,l,m);
		Normal_MergeSort(a,m+1,r);
		Normal_Merge(a,l,m,r);
	}
}
/////////////////////////////////// Normal Merge Sort Ends //////////////////////////////////////////////////

/////////////////////////////////// Concurrent Merge Sort Starts  //////////////////////////////////////////////////
void Concurrent_MergeSort(int arr[], int l, int r)  
{  
    if (l < r) 
    {   
		if(r-l+1<5)
		{
			Normal_SelectionSort(arr, l, r);
			return;
		}

        int m = (l+r)/2;  
		int pidr; 
        int pidl=fork();
    
        if(pidl == 0)
        {
            Normal_MergeSort(arr, l, m);
            _exit(1);
        }
        else
		{
            pidr = fork();
            if(pidr == 0)
            {
                Normal_MergeSort(arr, m + 1, r);
                _exit(1);
            }
            else
            {
                int status;
                waitpid(pidl, &status, 0);
                waitpid(pidr, &status, 0);
            }
        }      
        Normal_Merge(arr, l, m, r); 
    }  
} 
/////////////////////////////////// Concurrent Merge Sort Ends //////////////////////////////////////////////////

/////////////////////////////////// Threaded Merge Sort Starts  //////////////////////////////////////////////////
void Threaded_SelectionSort(int a[],int l,int r)
{
	pthread_mutex_lock(&lock);

    for(int i=l; i<r; i++)
    {
        int min_pos = i;
        for(int j=i+1;j<=r;j++)
        {
            if(a[j]<a[min_pos])
                min_pos = j;
        }

        int swap = a[min_pos];
        a[min_pos] = a[i];
        a[i] = swap;
    }

	pthread_mutex_unlock(&lock);  
}
void Threaded_Merge(int a[],int l,int m,int r)
{
	pthread_mutex_lock(&lock);

	int size=r-l+1;
	int K[size];
	//temporary array used is K from indices 0 to size-1
	int i=l;
	int j=m+1;

	for(int z=0;z<size;z++)
	{
		if(i<=m&&j<=r)
		{
            if(a[i]<a[j])
            {
                K[z]=a[i];
                i++;
            }
            else
            {
                K[z]=a[j];
                j++;
            }
		}
		else
		{
			if(i>m)
			{
				for(int yo=j;yo<=r;yo++)
				{
					K[z]=a[yo];
					z++;
				}
				break;
			}
			else
			{
				for(int yo=i;yo<=m;yo++)
                {
                        K[z]=a[yo];
                        z++;
                }
                break;
			}
		}
	}
	for(int z=0;z<size;z++)
	{
		a[l+z]=K[z];
	}

	pthread_mutex_unlock(&lock);  
}
void *Threaded_MergeSort(void *pointer)
{   
    arg* typecasted = (arg*) pointer;

	if(typecasted->l < typecasted-> r)
	{
		if(typecasted-> r - typecasted->l + 1 < 5)
		{
			Threaded_SelectionSort(typecasted->arr, typecasted->l, typecasted->r);
		}

		arg left_pointer;
		left_pointer.l = typecasted->l;
		left_pointer.r = (typecasted->l + typecasted->r)/2;
		left_pointer.arr = typecasted->arr;

		arg right_pointer;
		right_pointer.l = (typecasted->l + typecasted->r)/2 + 1;
		right_pointer.r = typecasted->r;
		right_pointer.arr = typecasted->arr;

		pthread_t left_thread;
		pthread_t right_thread;

		int th1,th2;
		th1 = pthread_create(&left_thread, NULL, Threaded_MergeSort, &left_pointer);
		th2 = pthread_create(&right_thread, NULL, Threaded_MergeSort, &right_pointer);
		pthread_join(left_thread, NULL);
		pthread_join(right_thread, NULL);

		Threaded_Merge(typecasted->arr, typecasted->l, (typecasted->l + typecasted->r)/2, typecasted->r);

	}

	return NULL;
}
/////////////////////////////////// Threaded Merge Sort Ends //////////////////////////////////////////////////

/////////////////////////////////// Run All Sorts //////////////////////////////////////////////////
void runSorts(int n){

	struct timespec ts;
	//getting shared memory
	int *arr = shareMem(sizeof(int)*(n+1));
	for(int i=0;i<n;i++) scanf("%d", arr+i);

	for(int i=0;i<n;i++) a[i] = arr[i];

	printf("\n\n");
	//multiprocess mergesort
	printf("Running concurrent mergesort.......\n");
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	long double st = ts.tv_nsec/(1e9)+ts.tv_sec;
	Concurrent_MergeSort(arr, 0, n-1);
	printf("Sorted Array: \n");
	for(int i=0; i<n; i++){
		printf("%d ",arr[i]);
	}
	printf("\n");
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	long double en = ts.tv_nsec/(1e9)+ts.tv_sec;
	printf("time = %Lf\n", en - st);
	long double t1 = en-st;


	printf("\n\n");
	//multithreaded mergesort
	pthread_t tid;
	struct arg ar;
	ar.l = 0;
	ar.r = n-1;
	ar.arr = a;
	printf("Running threaded concurrent mergesort......\n");
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	st = ts.tv_nsec/(1e9)+ts.tv_sec;
	pthread_create(&tid, NULL, Threaded_MergeSort, &ar);
	pthread_join(tid, NULL);
	printf("Sorted Array: \n");
	for(int i=0; i<n; i++){
		printf("%d ",ar.arr[i]);
	}
	printf("\n");
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	en = ts.tv_nsec/(1e9)+ts.tv_sec;
	printf("time = %Lf\n", en - st);
	long double t2 = en-st;

	printf("\n\n");
	// normal mergesort
	printf("Running normal_quicksort.......\n");
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	st = ts.tv_nsec/(1e9)+ts.tv_sec;
	Normal_MergeSort(a, 0, n-1);
	printf("Sorted Array: \n");
	for(int i=0; i<n; i++){
		printf("%d ",a[i]);
	}
	printf("\n");
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	en = ts.tv_nsec/(1e9)+ts.tv_sec;
	printf("time = %Lf\n", en - st);
	long double t3 = en - st;

	printf("\n\n\n");

	//Print Results
	printf("Normal mergesort ran:\n[ %Lf ] times faster than concurrent mergesort\n[ %Lf ] times faster than threaded concurrent merge sort\n\n\n", t1/t3, t2/t3);
	shmdt(arr);
	return;
}


int main()
{
    int length;
    scanf("%d", &length);

	runSorts(length);
      
	return 0;
}