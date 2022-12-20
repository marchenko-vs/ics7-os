#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int main(void) 
{
    int status = 0;
    pid_t childpid[2];
    for (int i = 0; i < 2; i++)
    {
        if ((childpid[i] = fork()) == -1) 
        {
            perror("Can't fork.");
            exit(1);
        } 
        else if (childpid[i] == 0) 
        {
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
    return 0;
}
