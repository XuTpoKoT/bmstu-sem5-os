#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

#define CNT_CHILDS 2
#define STR_LEN 100

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
	setbuf(stdout, NULL);
    int child_pids[CNT_CHILDS];
    int fd[CNT_CHILDS][2];
    char mes[][STR_LEN] = {"AAA\n", "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB\n"};

	for (int i = 0; i < CNT_CHILDS; i++)
		if (pipe(fd[i]) == -1)
		{
		    perror("Can't pipe\n");
		    exit(1);
		}
    
    for (int i = 0; i < CNT_CHILDS; i++)
    {
    	child_pids[i] = fork();

        switch (child_pids[i])
        {
		    case -1:
		    {
		        perror("Can\'t fork()\n");
		        exit(1);
		    }
		    case 0:
		    {
		        close(fd[i][0]);
            	if (write(fd[i][1], mes[i], strlen(mes[i]) + 1) == -1)
				{
					perror("Can\'t write\n");
					exit(1);
				}
                return 0;
            }
		    default:
		    {
		    	int status;
		    	printf("Parent %d, GROUP: %d\n", getpid(), getpgrp());
		    	pid_t child_pid = waitpid(child_pids[i], &status, 0);
				if (child_pid == -1)
				{
					perror("Couldn't wait for child\n");
					exit(1);
				}
				printf("Child process %d finished with status %d\n", child_pid, status);
				check_status(status);
				close(fd[i][1]);
				if (read(fd[i][0], mes[i], STR_LEN) == -1)
				{
					perror("Can\'t read\n");
					exit(1);
				}
				printf("Recieved message:\n%s", mes[i]);
			}
				
        }
    }
    
    

    return 0;
}
