#!/bin/zsh
echo "\nConcurrent Mergesort ::\n" > report.txt
echo 10000 > testcase.txt
shuf -i 2000-65000 -n 10000 >> testcase.txt
gcc P3.c
{ /usr/bin/time -f "\n%E real\n%U user\n%S sys\n%c context switches\n%F/%R (major/minor) page faults\n%K avg. total memory\n" ./a.out < testcase.txt > check1.txt; } 2>> report.txt
echo "\nSequential Mergesort ::\n" >> report.txt
gcc P3_msort.c
{ /usr/bin/time -f "\n%E real\n%U user\n%S sys\n%c context switches\n%F/%R (major/minor) page faults\n%K avg. total memory\n" ./a.out < testcase.txt > check2.txt; } 2>> report.txt
echo "\nThreaded Mergesort ::\n" >> report.txt
gcc -pthread P3_thread.c
{ /usr/bin/time -f "\n%E real\n%U user\n%S sys\n%c context switches\n%F/%R (major/minor) page faults\n%K avg. total memory\n" ./a.out < testcase.txt > check3.txt; } 2>> report.txt

