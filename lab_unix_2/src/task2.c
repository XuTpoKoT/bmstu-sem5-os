#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#define CNT_CHILDS 2

void check_status(int status)
{
    if (WIFEXITED(status)) 
        printf("child process has finished, code: %d\n", WEXITSTATUS(status));
    if (WIFSIGNALED(status))
        printf("child process has finished by unhandlable signal, signum: %d\n", WTERMSIG(status));
    if (WIFSTOPPED(status))
        printf("child process has finished by signal, signum: %d\n", WSTOPSIG(status));
}

int main()
{
    int child_pid[CNT_CHILDS];
    int status;

    for (int i = 0; i < CNT_CHILDS; i++)
    {
        child_pid[i] = fork();

        switch (child_pid[i])
        {
		    case -1:
		        perror("Can\'t fork()\n");
		        exit(-1);
		    case 0:
		        printf("Child %d, PPID: %d, GROUP: %d \n", getpid(), getppid(), getpgrp());
		        return 0;
		    default:
				printf("Parent %d, GROUP: %d, child %d\n", getpid(), getpgrp(), child_pid[i]);
        }
    }

	for (int i = 0; i < CNT_CHILDS; i++)
	{
    	pid_t child = waitpid(child_pid[i], &status, WUNTRACED);
    	if (child == -1) 
    	{
            perror("Can't wait child");
            exit(1);
        }
        
        printf("Child process %d has finished, status %d\n", child, status);
		check_status(status);
	}
        
    return 0;
}
