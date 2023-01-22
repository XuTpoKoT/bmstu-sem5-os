#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

#define CNT_CHILDS 2
#define STR_LEN 100

int flag = 0;

void check_status(int status)
{
    if (WIFEXITED(status)) 
        printf("child process has finished, code: %d\n", WEXITSTATUS(status));
    if (WIFSIGNALED(status))
        printf("child process has finished by unhandlable signal, signum: %d\n", WTERMSIG(status));
    if (WIFSTOPPED(status))
        printf("child process has finished by signal, signum: %d\n", WSTOPSIG(status));
}

void signal_handler(int signal)
{
    flag = 1;
    printf("\nCatch signal = %d\n", signal);
}

int main()
{
    int child_pid[CNT_CHILDS];
    int fd[2];
    char mes[CNT_CHILDS][STR_LEN] = {"AAA\n", "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB\n"};
    int status;
    
    if (signal(SIGINT, signal_handler) == SIG_ERR)
    {
    	perror("Can't signal\n");
		exit(1);
    }
    sleep(2);
    

	if (pipe(fd) == -1)
	{
	    perror("Can't pipe\n");
	    exit(1);
	}
    
    for (int i = 0; i < CNT_CHILDS; i++)
    {
    	child_pid[i] = fork();

        switch (child_pid[i])
        {
		    case -1:
		    {
		        perror("Can\'t fork()\n");
		        exit(1);
		    }
		    case 0:
		    {
		    	close(fd[0]);
	        	if (write(fd[1], mes[i], strlen(mes[i]) + 1) == -1)
				{
					perror("Can\'t write\n");
					exit(1);
				}
                return 0;
            }
		    default:
		    	printf("Parent %d, GROUP: %d, child %d\n", getpid(), getpgrp(), child_pid[i]);				
        }
    }
    
    if (!flag)
    {
		perror("Can\'t catch signal\n");
		exit(1);
	}
    close(fd[1]);
      
    for (int i = 0; i < 2; i++)
    {  	
    	pid_t child = waitpid(child_pid[i], &status, 0);
    	if (child == -1)
        {
            perror("Can't wait.");
            exit(1);
        }
     	check_status(status);
     	
        if (read(fd[0], mes[i], STR_LEN) == -1)
        {
            perror("Can't read.");
            exit(1);
        }
        printf("Recieved message:\n%s\n", mes[i]);   
    }

    return 0;
}
