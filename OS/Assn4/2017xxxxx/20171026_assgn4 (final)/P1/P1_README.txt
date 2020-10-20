P1:
A thread have been created for the queuer, who brings in a player or referee. A random number 
is generated and (number % remaining number of people) is calculated. If that is < number of 
referees that have to enter, then a referee enters. If the number belongs to [n, 3n), a player 
is generated. This ensures the probability condition given in the question. The queuer is res-
ponsible for spawning the respective person threads.

A global state array has been created for the referees and players has been declared,which keeps
track of the states of the persons, which are listed below:

State no:
1. Person has entered academy
2. Person has met organiser
3. Person has been chosen for game
4. Person has entered court
5. Pregame preparations done (warm-up or adjusting equipment)
6. (Only for referee) The referee has started the game

The organizer is responsible for incrementing the states of the referees and players, and signalling 
the corresponding condition variable that the person is waiting on, so that the person can proceed on
with the following functions. The oraganiser thread waits for the player threads to complete (warmUp 
is their last function), before the referee can start the game. Hence it is equivalent to the referee
waiting for the players to finish warming up.

Usage:
Enter the number n as input and press enter. (1<=n<=100)
