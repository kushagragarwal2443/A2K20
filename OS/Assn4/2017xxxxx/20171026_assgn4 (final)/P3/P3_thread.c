#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t lock;

int length;
int thread_count = 0;

typedef struct thread_args
{
    int *a;
    int left;
    int right;
} targs;

targs left_args, right_args;

void selSort(int arr[], int n)
{
    pthread_mutex_lock(&lock);

    for (int i = 0; i < n - 1; i++)
    {
        int m_index = i;
        for (int j = i + 1; j < n; j++)
            if (arr[j] < arr[m_index])
                m_index = j;

        int temp = arr[m_index];
        arr[m_index] = arr[i];
        arr[i] = temp;
    }
    pthread_mutex_unlock(&lock);
}

void merge(int a[], int l, int mid, int r)
{
    pthread_mutex_lock(&lock);
    int len = r - l + 1;
    int *temp = (int *)malloc(sizeof(int) * 2 * len);
    int i = l;
    int k = mid + 1;
    int j = 0;

    while (i <= mid && k <= r)
    {
        if (a[i] < a[k])
        {
            temp[j] = a[i];
            i++;
            j++;
        }
        else if (a[k] < a[i])
        {
            temp[j] = a[k];
            j++;
            k++;
        }
        else if (a[i] == a[k])
        {
            temp[j] = a[i];
            i++;
            j++;
            temp[j] = a[k];
            j++;
            k++;
        }
    }

    while (i <= mid)
        temp[j++] = a[i++];

    while (k <= r)
        temp[j++] = a[k++];

    j = l;

    for (i = 0; i < len; i++)
        a[j++] = temp[i];

    free(temp);

    pthread_mutex_unlock(&lock);
}

void *mergeSort(void *ptr)
{
    targs* t;
    pthread_t th1, th2;
    t = (targs*)ptr;

    int len = (t->right - t->left + 1);

    if (len <= 5)
    {
        selSort(t->a + t->left, len);
        return NULL;
    }

    targs rec_a;
    targs rec_b;

    rec_a.a = t->a;
    rec_a.left = t->left;
    rec_a.right = t->left + len / 2 - 1;

    printf("%d %d\n", rec_a.left, rec_a.right);

    int iret1 = pthread_create(&th1, NULL, mergeSort, (void*)(&rec_a));
    printf("%d\n", iret1);

    rec_b.a = t->a;
    rec_b.left = t->left + len / 2;
    rec_b.right = t->right;
    printf("%d %d\n", rec_b.left, rec_b.right);

    int iret2 = pthread_create(&th2, NULL, mergeSort, (void*)(&rec_b));
    printf("%d\n", iret2);

    pthread_join(th1, NULL);
    pthread_join(th2, NULL);

    merge(t->a, t->left, t->left + len / 2 - 1, t->right);
}

void isSorted(int arr[], int len)
{
    int i;
    for (i = 0; i < len; i++)
        printf("%d ", arr[i]);

    for (i = 1; i < len; i++)
    {
        if (arr[i] < arr[i - 1])
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
    int *sharedArray;
    pthread_mutex_init(&lock, NULL);

    scanf("%d", &length);

    sharedArray = (int*)malloc(sizeof(int)*length);


    for (int i = 0; i < length; i++)
        scanf("%d", &sharedArray[i]);

    targs* t;
    t->a = sharedArray;
    t->left = 0;
    t->right = length - 1;
    pthread_t mainthread;
    int iret1 = pthread_create(&mainthread, NULL, mergeSort, (void*)t);

    pthread_join(mainthread, NULL);

    isSorted(sharedArray, length);
    pthread_mutex_destroy(&lock);

    return 0;
}
