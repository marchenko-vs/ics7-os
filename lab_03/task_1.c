#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main(void) 
{
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
            printf("Child before sleep. PID = %d, ParentPID = %d, GROUP_ID = %d.\n", getpid(), getppid(), getpgrp());
	    sleep(2);
            printf("Child after sleep. PID = %d, ParentPID = %d, GROUP_ID = %d.\n", getpid(), getppid(), getpgrp());
	    return 0;
        }
        else 
        {
            printf("Parent process. PID = %d, GROUP_ID = %d, child PID = %d.\n", getpid(), getpgrp(), childpid[i]);
        }
    } 
    return 0;
}
