
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int length;

void selSort(int arr[], int n)
{
    for(int i = 0; i < n-1; i++)
    {
        int m_index = i;
        for (int j = i+1; j < n; j++)
          if (arr[j] < arr[m_index])
            m_index = j;

        int temp = arr[m_index];
        arr[m_index] = arr[i];
        arr[i] = temp;
    }
}

void merge(int a[], int l, int mid, int r)
{
    int len = r-l+1;
    int* temp = (int*) malloc(sizeof(int)*len);
    int i = l;
    int k= mid+1;
    int j = 0;

    while (i <= mid && k <= r)
    {
        if (a[i]<a[k])
        {
            temp[j]=a[i];
            i++;
            j++;
        }
        else if (a[k]<a[i])
        {
            temp[j]=a[k];
            j++;
            k++;
        }
        else if (a[i]==a[k])
        {
            temp[j]=a[i];
            i++;
            j++;
            temp[j]=a[k];
            j++;
            k++;
        }
    }

    while (i<=mid)
        temp[j++]=a[i++];

    while (k<=r)
        temp[j++]=a[k++];

    j = l;
    for (i=0; i< len; i++)
       a[j++] = temp[i];

    free(temp);
}

void mergeSort(int a[], int l, int r)
{
    int len=(r-l+1);

    if (len<=5)
    {
        selSort(a+l, len);
        return;
    }

    int lpid, rpid;
    lpid = fork(); a[length]++;
    if (lpid<0)
    {
        perror("lFork error\n");
        exit(-1);
    }
    else if (!lpid)
    {
        mergeSort(a,l,l+len/2-1);
        exit(0);
    }
    else
    {
       // waitpid(lpid, NULL, WUNTRACED);
        rpid = fork(); a[length]++;
        if (rpid<0)
        {
            perror("rFork error\n");
            exit(-1);
        }
        else if(!rpid)
        {
            mergeSort(a,l+len/2,r);
            exit(0);
        }
    }

    int status;

    waitpid(lpid, &status, 0);
    waitpid(rpid, &status, 0);

    merge(a, l, l+len/2-1, r);
}

void isSorted(int arr[], int len)
{
    int i;
    for (i=0; i<len; i++)
        printf("%d ", arr[i]);

    for (i=1; i<len; i++)
    {
        if (arr[i]<arr[i-1])
        {
            printf("Sorting Not Done\n");
            return;
        }
    }
    printf("Sorting Done Successfully\n");
    return;
}

int main()
{
    int shmid;
    key_t key = IPC_PRIVATE;
    int *sharedArray;

    scanf("%d",&length);

    shmid = shmget(key, sizeof(int)*(length+1), IPC_CREAT| 0666);
	if(shmid == -1) {
		perror("Shmget failed");
		exit(1);
	}

    sharedArray = shmat(shmid, 0, 0);
	if(sharedArray == (void *)-1) {
		perror("Shmat failed");
		exit(1);
	}

    for(int i = 0; i<length; i++) scanf("%d", &sharedArray[i]);
    mergeSort(sharedArray, 0, length-1);

    isSorted(sharedArray, length);

    if (shmdt(sharedArray) == -1)
    {
        perror("shmdt");
        exit(1);
    }

    return 0;
}
