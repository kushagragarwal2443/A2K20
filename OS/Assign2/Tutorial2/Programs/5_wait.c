#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
	int forkReturn, i, loopsize;

	loopsize = 3;
	if (forkReturn == 0)
	{
		// Child process
		for (i = 0; i < loopsize; i++)
		{
			printf("child: %d\n", i + 1);
		}
	}
	else
	{
		// Parent process

		// Wait for child to finish
		wait(NULL);

		for (i = 0; i < loopsize; i++)
		{
			printf("parent: %d\n", i + 1);
		}
	}
	return 0;
}
