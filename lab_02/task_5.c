#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>

int flag = 0;

void sigint_handler(int signum)
{
    flag = 1;
}

int main(void) 
{
    int status = 0;
    char msgs[2][128] = {"dddd", "aaaaaaaa"};
    int fd[2];
    pid_t childpid[2];
    if (pipe(fd) == -1)
    {
        perror("Can't pipe.\n");
        exit(1);
    }
    if (signal(SIGTSTP, sigint_handler) == SIG_ERR)
    {
        perror("Can't signal.\n");
        exit(1);
    }
    sleep(2);
    for (int i = 0; i < 2; i++)
    {
        if ((childpid[i] = fork()) == -1) 
        {
            perror("Can't fork.");
            exit(1);
        } 
        else if (childpid[i] == 0) 
        {
            if (flag)
            {
            	close(fd[0]);
            	write(fd[1], msgs[i], strlen(msgs[i]));
            }
            printf("Child process %d. PID = %d, ParentPID = %d, GROUP_ID = %d.\n", i, getpid(), getppid(), getpgrp());
            return 0;
        }
        else 
        {
            printf("Parent process. PID = %d, GROUP_ID = %d, child PID = %d.\n", getpid(), getpgrp(), childpid[i]);
        }
    }
    for (int i = 0; i < 2; i++)
    {
     	if ((childpid[i] = waitpid(childpid[i], &status, 0)) == -1)
        {
            perror("Can't waitpid.\n");
            exit(1);
        }
    	printf("Child process with PID = %d has finished. Status = %d.\n", childpid[i], status);
	if (WIFEXITED(status))
	{
	printf("Child process with PID = %d has finished with code = %d.\n", childpid[i], WEXITSTATUS(status));
	}
	else if (WIFSIGNALED(status))
	{
	printf("Child process with PID = %d has finished from signal with code = %d.\n", childpid[i], WTERMSIG(status));
	}
	else if (WIFSTOPPED(status))
	{
	printf("Child process with PID = %d has stopped with code = %d.\n", childpid[i], WSTOPSIG(status));
	}
    }
    msgs[0][0] = '\0';
    close(fd[1]);
    read(fd[0], msgs[0], 128);
    if (flag)
    {
        printf("String from pipe:\n%s\n", msgs[0]);
    }
    else
    {
    	printf("No signal caught.\n");	
    }
    return 0;
}
