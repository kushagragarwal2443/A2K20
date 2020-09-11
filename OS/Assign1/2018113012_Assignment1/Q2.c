#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int a, char* input[])
{
    struct stat file;
    long long int n = 1000000;

    if(stat(input[3], &file)== 0)
	    write(1,"Directory is created: Yes\n",sizeof("Directory is created: Yes\n"));
    else
	    write(1,"Directory is created: No\n",sizeof("Directory is created: No\n"));


    // Check if the newfile and oldfile are reversed
    long long int fd1, fd2;
    fd1 = open(input[1], O_RDONLY);
    fd2 = open(input[2], O_RDONLY);
    long long int notsame = 0;

    
    // Number of characters in the file
    long long int filelength = lseek(fd1, 0 , SEEK_END);
    
    //Sliding window of size n used where n
    long long int offset = lseek(fd1, -n, SEEK_END);
    char buffervec[n +10]; // Slightly more than the window
    char buffervec2[n +10]; // Slightly more than the window

    long long int bytesread = 0;
    // we iterate till offset is non negative
    for(; offset >=0 ;)
    {
        // Read the contents of the sliding window
        long long int size = read(fd1, buffervec, n);
        long long int size2 = read(fd2, buffervec2, n);
        bytesread+=size;

        // Append \0 to complete the string
        buffervec[n] = '\0';
        buffervec2[n] = '\0';

        //Reverse the vector buffervec
        char reversebuffervec[n+10];
        long long int index =0;
        for( long long int i= n-1; i>=0; i-- )
        {
            reversebuffervec[index] = buffervec[i];
            index=index+1;
        }
        reversebuffervec[n] = '\0';

        // Compare the two strings
        if(strcmp(reversebuffervec, buffervec2)!=0)
            notsame = 1;

        // Shift offset by 2*window of buffer size from previous location
        offset = lseek(fd1, -2*n, SEEK_CUR);
    }
    
    // if first set of characters left which are less than n
    lseek(fd1, 0, SEEK_SET); // Set to start point
    long long int leftover = read(fd1, buffervec, filelength-bytesread); // Read till uncovered
    long long int leftover2 = read(fd2, buffervec2, filelength-bytesread); // Read till uncovered
    buffervec[filelength-bytesread] = '\0'; // Append to complete the string
    buffervec2[filelength-bytesread] = '\0';

    // Reverse buffervec
    char reversebuffervec[n+10];
    long long int index =0;
    for( long long int i= leftover-1; i>=0; i-- )
    {
        reversebuffervec[index] = buffervec[i];
        index=index+1;
    }

    reversebuffervec[leftover] = '\0';

    if(strcmp(reversebuffervec, buffervec2)!=0)
            notsame = 1;

    write(1,"\n",sizeof("\n"));

    if( notsame==0)
        write(1,"Whether file contents are reversed in newfile: Yes\n",sizeof("Whether file contents are reversed in newfile: Yes\n"));
    else
        write(1,"Whether file contents are reversed in newfile: No\n",sizeof("Whether file contents are reversed in newfile: No\n"));

    write(1,"\n",sizeof("\n"));	

    write(1,"\n",sizeof("\n"));	

    if(stat(input[1],&file)<0)
        write(1,"New File not Found",sizeof("New File not Found"));
    else
    {
        if(file.st_mode & S_IRUSR)
            write(1,"User has read permissions on newfile: Yes\n",sizeof("User has read permissions on newfile: Yes\n"));
        else
            write(1,"User has read permissions on newfile: No\n",sizeof("User has read permissions on newfile: No\n"));

        if(file.st_mode & S_IWUSR)
            write(1,"User has write permission on newfile: Yes\n",sizeof("User has write permission on newfile: Yes\n"));
        else
            write(1,"User has write permissions on newfile: No\n",sizeof("User has read permissions on newfile: No\n"));

        if(file.st_mode & S_IXUSR)
            write(1,"User has execute permission on newfile: Yes\n",sizeof("User has execute permission on newfile: Yes\n"));
        else
            write(1,"User has execute permission on newfile: No\n",sizeof("User has execute permission on newfile: No\n"));
        
        if(file.st_mode & S_IRGRP)
            write(1,"Group has read permissions on newfile: Yes\n",sizeof("Group has read permissions on newfile: Yes\n"));
        else
            write(1,"Group has read permissions on newfile: No\n",sizeof("Group has read permissions on newfile: No\n"));

        if(file.st_mode & S_IWGRP)
            write(1,"Group has write permissions on newfile: Yes\n",sizeof("Group has read permissions on newfile: Yes\n"));
        else 
            write(1,"Group has write permissions on newfile: No\n",sizeof("Group has read permissions on newfile: No\n"));

        if(file.st_mode & S_IXGRP)
                write(1,"Group has execute permission on newfile: Yes\n",sizeof("Group has execute permission on newfile: Yes\n"));
        else
            write(1,"Group has execute permission on newfile: No\n",sizeof("Group has execute permission on newfile: No\n"));

        if(file.st_mode & S_IROTH)
            write(1,"Others has read permissions on newfile: Yes\n",sizeof("Others has read permissions on newfile: Yes\n"));
        else
            write(1,"Others has read permissions on newfile: No\n",sizeof("Others has read permissions on newfile: No\n"));

        if(file.st_mode & S_IWOTH)
            write(1,"Others has write permission on newfile: Yes\n",sizeof("Others has write permission on newfile: Yes\n"));
        else 
            write(1,"Others has write permission on newfile: No\n",sizeof("Others has write permission on newfile: No\n")); 

        if(file.st_mode & S_IXOTH)
            write(1,"Others has execute permission on newfile: Yes\n",sizeof("Others has execute permission on newfile: Yes\n"));
        else
            write(1,"Others has execute permission on newfile: No\n",sizeof("Others has execute permission on newfile: No\n"));	 
    }

    write(1,"\n",sizeof("\n"));	

    if(stat(input[2], &file) < 0)
        write(1,"Old File not Found",sizeof("Old File not Found"));
    else
    { 
        if(file.st_mode & S_IRUSR)
            write(1,"User has read permissions on oldfile: Yes\n",sizeof("User has read permissions on oldfile: Yes\n"));
        else
            write(1,"User has read permissions on oldfile: No\n",sizeof("User has read permissions on oldfile: No\n"));

        if(file.st_mode & S_IWUSR)
            write(1,"User has write permission on oldfile: Yes\n",sizeof("User has write permission on oldfile: Yes\n"));
        else
            write(1,"User has write permissions on oldfile: No\n",sizeof("User has read permissions on oldfile: No\n"));

        if(file.st_mode & S_IXUSR)
            write(1,"User has execute permission on oldfile: Yes\n",sizeof("User has execute permission on oldfile: Yes\n"));
        else
            write(1,"User has execute permission on oldfile: No\n",sizeof("User has execute permission on oldfile: No\n"));
        
        if(file.st_mode & S_IRGRP)
            write(1,"Group has read permissions on oldfile: Yes\n",sizeof("Group has read permissions on oldfile: Yes\n"));
        else
            write(1,"Group has read permissions on oldfile: No\n",sizeof("Group has read permissions on oldfile: No\n"));

        if(file.st_mode & S_IWGRP)
            write(1,"Group has write permissions on oldfile: Yes\n",sizeof("Group has read permissions on oldfile: Yes\n"));
        else 
            write(1,"Group has write permissions on oldfile: No\n",sizeof("Group has read permissions on oldfile: No\n"));

        if(file.st_mode & S_IXGRP)
                write(1,"Group has execute permission on oldfile: Yes\n",sizeof("Group has execute permission on oldfile: Yes\n"));
        else
            write(1,"Group has execute permission on oldfile: No\n",sizeof("Group has execute permission on oldfile: No\n"));

        if(file.st_mode & S_IROTH)
            write(1,"Others has read permissions on oldfile: Yes\n",sizeof("Others has read permissions on oldfile: Yes\n"));
        else
            write(1,"Others has read permissions on oldfile: No\n",sizeof("Others has read permissions on oldfile: No\n"));

        if(file.st_mode & S_IWOTH)
            write(1,"Others has write permission on oldfile: Yes\n",sizeof("Others has write permission on oldfile: Yes\n"));
        else 
            write(1,"Others has write permission on oldfile: No\n",sizeof("Others has write permission on oldfile: No\n")); 

        if(file.st_mode & S_IXOTH)
            write(1,"Others has execute permission on oldfile: Yes\n",sizeof("Others has execute permission on oldfile: Yes\n"));
        else
            write(1,"Others has execute permission on oldfile: No\n",sizeof("Others has execute permission on oldfile: No\n"));	 
    }
    
    write(1,"\n",sizeof("\n"));	

    if(stat(input[3],&file)<0)
        write(1,"Directory not Found",sizeof("Directory not Found"));
    else
    { 
        if(file.st_mode & S_IRUSR)
            write(1,"User has read permissions on directory: Yes\n",sizeof("User has read permissions on directory: Yes\n"));
        else
            write(1,"User has read permissions on directory: No\n",sizeof("User has read permissions on directory: No\n"));

        if(file.st_mode & S_IWUSR)
            write(1,"User has write permission on directory: Yes\n",sizeof("User has write permission on directory: Yes\n"));
        else
            write(1,"User has write permissions on directory: No\n",sizeof("User has read permissions on directory: No\n"));

        if(file.st_mode & S_IXUSR)
            write(1,"User has execute permission on directory: Yes\n",sizeof("User has execute permission on directory: Yes\n"));
        else
            write(1,"User has execute permission on directory: No\n",sizeof("User has execute permission on directory: No\n"));
        
        if(file.st_mode & S_IRGRP)
            write(1,"Group has read permissions on directory: Yes\n",sizeof("Group has read permissions on directory: Yes\n"));
        else
            write(1,"Group has read permissions on directory: No\n",sizeof("Group has read permissions on directory: No\n"));

        if(file.st_mode & S_IWGRP)
            write(1,"Group has write permissions on directory: Yes\n",sizeof("Group has read permissions on directory: Yes\n"));
        else 
            write(1,"Group has write permissions on directory: No\n",sizeof("Group has read permissions on directory: No\n"));

        if(file.st_mode & S_IXGRP)
                write(1,"Group has execute permission on directory: Yes\n",sizeof("Group has execute permission on directory: Yes\n"));
        else
            write(1,"Group has execute permission on directory: No\n",sizeof("Group has execute permission on directory: No\n"));

        if(file.st_mode & S_IROTH)
            write(1,"Others has read permissions on directory: Yes\n",sizeof("Others has read permissions on directory: Yes\n"));
        else
            write(1,"Others has read permissions on directory: No\n",sizeof("Others has read permissions on directory: No\n"));

        if(file.st_mode & S_IWOTH)
            write(1,"Others has write permission on directory: Yes\n",sizeof("Others has write permission on directory: Yes\n"));
        else 
            write(1,"Others has write permission on directory: No\n",sizeof("Others has write permission on directory: No\n")); 

        if(file.st_mode & S_IXOTH)
            write(1,"Others has execute permission on directory: Yes\n",sizeof("Others has execute permission on directory: Yes\n"));
        else
            write(1,"Others has execute permission on directory: No\n",sizeof("Others has execute permission on directory: No\n"));	 
    }

    close(fd1);
    close(fd2);  

    return 0; 
}