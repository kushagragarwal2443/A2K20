## Instructions to run the code

To run the code *cd* into the code directory and enter the following commands

1) cd server
2) gcc server.c
3) ./a.out
4) ../client
5) gcc client.c
6) ./a.out

So first we go to the server folder and setup the server. Then we go to the client folder and setup the client server


2 commands are allowed

## 1) exit
* This command terminates the connection between the server and the client
* Synchronous termination

## 2) get filename1 filename2 ...
* This command can take multiple filenames as arguement
* If the file exists in the server directory then it gets downloaded to the client directory
* If the file is not present in the server directory then appropriate error message is shown
* It sends files using chunks (packets) of 1024 characters at a time
* Also shows percentage of download completed








