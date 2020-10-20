// #define _POSIX_C_SOURCE 199309L //required for clock
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <limits.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <inttypes.h>
#include <math.h>

void swap(int* a, int* b) 
{ 
    int c = *a; 
    *a = *b; 
    *b = c; 
} 
int * shareMem(size_t size){
    key_t mem_key = IPC_PRIVATE;
    int shm_id = shmget(mem_key, size, IPC_CREAT | 0666);
    return (int*)shmat(shm_id, NULL, 0);
}


int partition (int arr[], int low, int high) 
{ 
    int pivotindex=rand()%(high-low+1)+low;
    swap(&arr[pivotindex], &arr[high]);
    int pivot = arr[high]; 
    int i = (low - 1); 
    for (int j = low; j < high; j++) 
    { 
        if (arr[j] < pivot) 
        { 
            i++;  
            swap(&arr[i], &arr[j]); 
        } 
    } 
    swap(&arr[i + 1], &arr[high]); 
    return (i + 1); 
} 

void normal_quicksort(int *arr, int l, int r){
    if(l>r) return;
     if(r-l+1<5){
        int a[5], mi=INT_MAX, mid=-1, key=0, j=0;
        for (int i = l; i <= r; i++) 
        {  
            key = arr[i];  
            j = i - 1;   
            while (j >= 0 && arr[j] > key) 
            {  
                arr[j + 1] = arr[j];  
                j = j - 1;  
            }  
            arr[j + 1] = key;  
        }  
        return;
    }

    int pi=partition(arr, l, r);
    normal_quicksort(arr, l, pi-1);
    normal_quicksort(arr, pi+1, r);
}

void quicksort(int *arr, int l, int r){
    if(l>=r) _exit(1);    
    
    if(r-l+1<5){
        int a[5], mi=INT_MAX, mid=-1, key=0, j=0;
        for (int i = l; i <= r; i++) 
        {  
            key = arr[i];  
            j = i - 1;   
            while (j >= 0 && arr[j] > key) 
            {  
                arr[j + 1] = arr[j];  
                j = j - 1;  
            }  
            arr[j + 1] = key;  
        }  
        return;
    } 

    int pi=partition(arr, l, r);
    int pid1 = fork();
    int pid2;
    if(pid1==0){
        //sort left half array
        quicksort(arr, l, pi-1);
        _exit(1);
    }
    else{
        pid2 = fork();
        if(pid2==0){
            //sort right half array
            quicksort(arr,pi+1,r);
            _exit(1);
        }
        else{
            //wait for the right and the left half to get sorted
            int status;
            waitpid(pid1, &status, 0);
            waitpid(pid2, &status, 0);
        }
    }
    return;
}

struct arg{
    int l;
    int r;
    int* arr;    
};

void *threaded_quicksort(void* a){
    //note that we are passing a struct to the threads for simplicity.
    struct arg *args = (struct arg*) a;

    int l = args->l;
    int r = args->r;
    int *arr = args->arr;
    if(l>r) return NULL;    
    
    if(r-l+1<5){
        int a[5], mi=INT_MAX, mid=-1, key=0, j=0;
        for (int i = l; i <= r; i++) 
        {  
            key = arr[i];  
            j = i - 1;   
            while (j >= 0 && arr[j] > key) 
            {  
                arr[j + 1] = arr[j];  
                j = j - 1;  
            }  
            arr[j + 1] = key;  
        }  
        return NULL;
    }

    //sort left half array
    int pi=partition(arr,l,r);
    struct arg a1;
    a1.l = l;
    a1.r = pi-1;
    a1.arr = arr;
    pthread_t tid1;
    pthread_create(&tid1, NULL, threaded_quicksort, &a1);
    
    //sort right half array
    struct arg a2;
    a2.l = pi+1;
    a2.r = r;
    a2.arr = arr;
    pthread_t tid2;
    pthread_create(&tid2, NULL, threaded_quicksort, &a2);
    
    //wait for the two halves to get sorted
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    
}



int main(){

    long long int n;
    printf("Enter number of elemnets in array\n");
    scanf("%lld", &n);
    struct timespec ts;
    
    int *arr = shareMem(sizeof(int)*(n+1));
    printf("Enter each element in space seperated order\n");
    for(int i=0;i<n;i++) scanf("%d", arr+i);
    int brr[n+1], crr[n+1];
    for(int i=0;i<n;i++) brr[i] = arr[i];
    for(int i=0;i<n;i++) crr[i] = arr[i];


    printf("Running concurrent_quicksort for n = %lld\n", n);
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    long double st = ts.tv_nsec/(1e9)+ts.tv_sec;

    //multiprocess quicksort
    quicksort(arr, 0, n-1);
    
    for (int i = 0; i < n; ++i)
    {
        printf("%d ",arr[i]);
    }
    printf("\n");
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    long double en = ts.tv_nsec/(1e9)+ts.tv_sec;
    printf("time = %Lf\n", en - st);
    long double t1 = en-st;



    pthread_t tid;
    struct arg a;
    a.l = 0;
    a.r = n-1;
    a.arr = brr;
    printf("Running threaded_concurrent_quicksort for n = %lld\n", n);
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    st = ts.tv_nsec/(1e9)+ts.tv_sec;

    //multithreaded quicksort
    pthread_create(&tid, NULL, threaded_quicksort, &a);
    pthread_join(tid, NULL);    
    
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    en = ts.tv_nsec/(1e9)+ts.tv_sec;
    printf("time = %Lf\n", en - st);
   for (int i = 0; i < n; ++i)
    {
        printf("%d ",brr[i]);
    }
    printf("\n");
    long double t2 = en-st;

    printf("Running normal_quicksort for n = %lld\n", n);
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    st = ts.tv_nsec/(1e9)+ts.tv_sec;

    // normal quicksort
    normal_quicksort(crr, 0, n-1);    
    
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    en = ts.tv_nsec/(1e9)+ts.tv_sec;
    printf("time = %Lf\n", en - st);
    for (int i = 0; i < n; ++i)
    {
        printf("%d ",crr[i]);
    }
    printf("\n");
    long double t3 = en - st;

    printf("normal_quicksort ran:\n\t[ %Lf ] times faster than concurrent_quicksort\n\t[ %Lf ] times faster than threaded_concurrent_quicksort\n\n\n", t1/t3, t2/t3);
    shmdt(arr);
    // return;
    return 0;
}