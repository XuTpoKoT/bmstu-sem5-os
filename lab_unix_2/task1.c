#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define CNT_CHILDS 2

int main()
{
	pid_t child_pid[CNT_CHILDS];

    for (int i = 0; i < CNT_CHILDS; i++)
    {
        child_pid[i] = fork();

        switch (child_pid[i])
        {
		    case -1:
		        perror("Can\'t fork()\n");
		        exit(1);
		    case 0:
		        printf("Child %d, PPID: %d, GROUP: %d \n", getpid(), getppid(), getpgrp());
		        sleep(2);
		        printf("Child %d, PPID: %d, GROUP: %d\n", getpid(), getppid(), getpgrp());
		        return 0;
			default:
				printf("Parent %d, GROUP: %d, child %d\n", getpid(), getpgrp(), child_pid[i]);
        }        
    }

    return 0;
}
