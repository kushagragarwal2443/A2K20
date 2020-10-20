PROBLEM STATEMENT:
==================

+ Ober is a new cab service, which needs your help to implement the simple system.The requirements of the system are as 
  follows given N cabs, M riders and K payment servers you need to implement a working system which ensures correctness and idempotency. Each cab has one driver. In Ober, payments by the riders should be done in Payment Servers only. Ober provides two types of cab services namely pool and premier ride. In pool ride maximum of two riders can sharea cab whereas in premier ride only one rider. There are four states for a cab namely:
  waitState ​ (no rider in cab), ​ onRidePremier​ , ​ onRidePoolFull ​ (pool ride with two riders), ​ onRidePoolOne​ (pool ride with only one rider).
+ As a part of this system you need to implement the following functionalities.
+ #### Riders​ :
  1. BookCab: This function takes three inputs namely ​ cabType ​ ,maxWaitTime , ​ ​ RideTime ​ . If the rider doesn’t find any cab (all cabs are in usage) until ​ maxWaitTime ​ he exits from the system with ​ TimeOut message.
  2. MakePayment: This function should be called by rider only after the end of the ride. If all the K payment servers are busy, then the rider should wait for the payment servers to get free. After payment is done rider can exit from the system.
+ #### Drivers​ :
  1. AcceptRide: If ride is premier cab in ​ waitState ​ should accept ride and change its state to ​ onRidePremier . ​ If the ride is pool and there is a cab with state ​ onRidePoolOne ​ then that cab should accept the ride and changes its state to ​ onRidePoolFull ​ . If the ride is pool and there is no cab with the state ​ onRidePoolOne ​ then cab in wait state should accept ride and change its state to ​ onRidePoolOne . ​ Assume that there is no time gap between accept rider and pickup rider, i.e., ride starts immediately after accepting ride. If there are multiple cabs available with required state take any random cab.
  2. OnRide: When pool cab is on ride and state is ​ onRidePoolOne ​ then driver can accept another pool ride. No new rider is accepted when cab is on premier ride.
  3. EndRide: The driver ends the ride after completion of the ​ RideTime ​ of the rider. If the ride is a premier it goes to ​ waitState ​ (ready to accept new rides) once ride is done. If cab’s state is ​ onRidePoolOne ​ then driver ends the ride and goes to ​ waitState . ​ If cab’s state is onRidePoolFull ​ then it goes to ​ onRidePoolOne ​ state (Notably, it can accept another pool rider). Driver or cab need not wait for the payment to be done by the rider.

+ #### Payment Servers:
1. Accept payment: Payment servers accepts a payment and assume the payment time is constant for all the riders (assume 2sec).

+ #### Instructions​ :
1. Your code must not result in busy-waiting (deadlocks).
2. Use appropriate times for RideTime and the maxWaitTime.
3. You need not follow above mentioned functions you can create functions appropriately.
4. Use Semaphores and mutex accordingly.
5. Use separate threads for each of the riders and payment servers.
6. Use only four states of Cab as mentioned above.


SOLUTION EXPLAINED:
===================

1. cabs struct: Has id of Cab, Status (0 for ​ waitState ​, 1 for onRidePremier ​, 2 for onRidePoolfull, 3 for ​ onRidePoolone).
2. Rider struct: Has id of Rider,thread id of thread corresponding to that rider,status,corresponding cab type (1 for premier , 0 for pool),corresponding ride time, corresponding max wait time and server id which it uses.
3. Server: Has id of Server,thread id of thread corresponding to that server,status(0 when in use , 1 when idle),rider id for which it is being used and corresponding mutex.
4. Three semaphores 
  + freecabs - For all cabs (initialised to total number of cabs)
  + paymentservers - For servers (initialised to total number of servers)
  + freecabsforpool - For cabs in state OnRidePoolOne (initialised to zero)

IMPORTANT FUNCTIONS:
----------------------------------------------------------

### BookCab
+  
  ```c
    void* bookcab(void *arg)
    {
    }
  ```
+ If a rider demands a Premier cab(type 1) , it waits in the semaphore  semcabs for the maximum time it can wait for. If the time exceeds , it exits leaving the time out message. If the semaphore semcabs has a value was greater than 1 this means a car in WaitState is available and will then search for the car(in WaitState) (applies locks for cabs and unlocking it after analysing that particular cab) and changes the car state to 1 (i.e OnRidePremier).


  ```c
        s = sem_timedwait(&freecabs, &ts);
  ```      

+ If a rider demands a Pool cab(type 0), it first checks if the value of the semaphore sempoolcabs is zero or greater than zero. 
    
    ```c
          sem_getvalue(&freecabsforpool, &sval);
    ``` 
+ If sempoolcabs was zero (means no cab in OnRidePoolOne state) then it proceeds similar to the one finding a premier cab.

+ If sempoolcabs was positive (means there is atleast one cab in OnRidePoolOne), then it goes on to find a cab having status 3 (i.e OnRidePoolOne) (applies locks for each cab and unlocking them after analysing that particular cab) and changes its state to 2(i.e OnRidePoolFull).

+ On Finishing the ride the status of the car changes accordingly and the rider calls the MakePayment function.

### MakePayment
+ Once rider is done with ride it enters this function and finds for a payment server available(applies locks for each server and unlocking them after analysing that particular server). If the server is not available then it waits due the semaphore semservers.
```c
    int s = sem_wait(&paymentserver);

```    

+ On finding an idle server it changes the status of that server from 0 (idle) to 1 (busy in accepting payment) and calls Accept_payment.

### AcceptPayment
+ This function is called by the server threads initially.
+ It waits till a riders changes the status of the server to 1(busy) , and then corresponding server in this function accepts the payment and changes its status back to 0(idle).
+ It exits when all the riders are done with the payment. 


RUNNING IT:
-----------
1. gcc -pthread Q1.c
2. ./a.out
