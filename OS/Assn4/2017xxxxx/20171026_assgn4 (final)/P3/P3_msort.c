#include <stdio.h> 
#include <stdlib.h> 

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
    if (l < r) 
    {
        int m = l+(r-l)/2; 
        mergeSort(a, l, m); 
        mergeSort(a, m+1, r); 
        merge(a, l, m, r); 
    } 
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
            printf("Sorting Not Done :"); 
            return; 
        } 
    } 
    printf("Sorting Done Successfully\n"); 
    return; 
} 

int main()
{
    int length;
    scanf("%d",&length);

    int* Array = (int*)malloc(sizeof(int)*length);

    for(int i = 0; i<length; i++) scanf("%d", &Array[i]);
    mergeSort(Array, 0, length-1);

    isSorted(Array, length);
    free(Array);
    return 0;
}