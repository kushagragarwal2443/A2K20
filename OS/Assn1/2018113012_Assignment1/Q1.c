#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

int main(int a, char* input[])
{

    long long int n = 1000000;
    // File Descriptors created
    long long int fd1, fd2;
    
    //Checking input file
    fd1 = open(input[1], O_RDONLY);
    if(fd1 < 0)
    {
        perror("rf1");
        exit(1);
    }

    // Output Dir Created
    mkdir("Assignment1", 0700);
    
    // Get file name from the input path by the user
    long long int inlen = strlen(input[1]);
    long long int filelen = 0;
    char revname[1000];
    char forname[1000];
	
    for(long long int i= inlen-1; i>=0; i--)
    {   
        if(input[1][i]=='/')
            break;
        else
        {	      
            revname[filelen]=input[1][i];
            filelen++;
        }	
    }

    revname[filelen] = '\0';

    for(long long int i=0; i<filelen; i++)
        forname[i]=revname[filelen-1-i];

    forname[filelen] = '\0';
    
    char path2[] = "Assignment1/";
    strcat(path2,forname);

    // Create second file using path2 created right now
    fd2 = open(path2, O_WRONLY | O_TRUNC | O_CREAT, 0600);
    if(fd2 < 0)
    {
        perror("f2");
        exit(1);
    }

    // Number of characters in the file
    long long int filelength = lseek(fd1, 0 , SEEK_END) ;
    
    //Sliding window of size n used where n
    long long int offset = lseek(fd1, -n, SEEK_END);
    char buffervec[n +10]; // Slightly more than the window

    long long int bytesread = 0;
    // we iterate till offset is non negative
    for(; offset >=0 ;)
    {
        // Read the contents of the sliding window
        long long int size = read(fd1, buffervec, n);
        bytesread+=size;

        // Append \0 to complete the string
        buffervec[n] = '\0';

        //Reverse the vector buffervec
        char reversebuffervec[n];
        long long int index =0;
        for( long long int i= n-1; i>=0; i-- )
        {
            reversebuffervec[index] = buffervec[i];
            index=index+1;
        }
        reversebuffervec[n] = '\0';

        // Write the reversed string to fd2
        write(fd2, reversebuffervec, n);

        // Percentage finished
        float percentage = ((float)(bytesread)/ (float)(filelength)) * 100.0;

        // Convert percentage to 2 decimal places notation
        char stringpercentage[10];
		snprintf(stringpercentage, sizeof(stringpercentage), "%0.2f%%\r", percentage);

        // Write to standard output
		write(1,stringpercentage,sizeof(stringpercentage));

        // Shift offset by 2*window of buffer size from previous location
        offset = lseek(fd1, -2*n, SEEK_CUR);
    }
    
    // if first set of characters left which are less than n
    lseek(fd1, 0, SEEK_SET); // Set to start point
    long long int leftover = read(fd1, buffervec, filelength-bytesread); // Read till uncovered
    buffervec[filelength-bytesread] = '\0'; // Append to complete the string

    // Reverse buffervec
    char reversebuffervec[n+10];
    long long int index =0;
    for( long long int i= leftover-1; i>=0; i-- )
    {
        reversebuffervec[index] = buffervec[i];
        index=index+1;
    }

    reversebuffervec[leftover] = '\0';

    // Write the reversed string to fd2
    write(fd2, reversebuffervec, leftover);

    // Print percentage of argv[1] reversed
    char completion[] = "100.00%\0";
    write(1,completion,sizeof(completion));

    // Close the files
    close(fd2);
    close(fd1);  

    return 0;  
}
