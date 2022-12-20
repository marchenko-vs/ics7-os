#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <wait.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define ACT_READER 0
#define ACT_WRITER 1
#define QUEUE_WRITER 2
#define QUEUE_READER 3
#define BIN_SEM 4

struct sembuf release_reader[1] = {{ ACT_READER, -1, 0 }};
struct sembuf release_writer[2] = {{ ACT_WRITER, -1, 0 }, { BIN_SEM, 1, 0 }};
struct sembuf lock_reader[5]    = {{ QUEUE_READER, 1, 0 }, { ACT_WRITER, 0, 0 }, { QUEUE_WRITER, 0, 0 }, { ACT_READER, 1, 0 }, 
                                   { QUEUE_READER, -1, 0 }};
struct sembuf lock_writer[6]    = {{ QUEUE_WRITER, 1, 0 }, { ACT_READER, 0, 0 }, { ACT_WRITER, 0, 0 }, 
                                   { ACT_WRITER, 1, 0 }, { BIN_SEM, -1, 0 }, { QUEUE_WRITER, -1, 0 }};

int read_data(int shmid, int semid, int readerid)
{
    int *addr = shmat(shmid, NULL, 0);
    if (addr == (void *)-1)
    {
        perror("Can't shmat.\n");
        exit(1);
    }
    srand(time(NULL) + readerid);
    for (int i = 0; i < 5; i++)
    {
        sleep(rand() % 6 + 1);
        if (semop(semid, lock_reader, 5) == -1)
            return 1;
        printf("Reader #%d read:  %d.\n", readerid + 1, *addr);
        if (semop(semid, release_reader, 1) == -1)
            return 1;
    }
    if (shmdt(addr) == -1)
    {
        perror("Can't shmdt.\n");
        exit(1);
    }
    return 0;
}

int write_data(int shmid, int semid, int writerid)
{
    int *addr = shmat(shmid, NULL, 0);
    if (addr == (void *)-1)
    {
        perror("Can't shmat.\n");
        exit(1);
    }
    for (int i = 0; i < 5; i++)
    {
        sleep(rand() % 3 + 1);
        if (semop(semid, lock_writer, 6) == -1)
            return 1;
        printf("Writer #%d wrote: %d.\n", writerid + 1, ++(*addr));
        if (semop(semid, release_writer, 2) == -1)
            return 1;
    }
    if (shmdt(addr) == -1)
    {
        perror("Can't shmdt.\n");
        exit(1);
    }
    return 0;
}

int main(void)
{
    key_t shmkey = ftok("text.txt", 3);
    if (shmkey == -1)
    {
        perror("Can't ftok (shmkey).\n");
        exit(1);
    }
    int perms = S_IRWXU | S_IRWXG | S_IRWXO;
    int shmid = shmget(shmkey, sizeof(int), perms | IPC_CREAT);
    if (shmid == -1)
    {
        perror("Can't shmget.\n");
        exit(1);
    }
    int *addr = shmat(shmid, NULL, 0);
    if (addr == (void *)-1)
    {
        perror("Can't shmat.\n");
        exit(1);
    }
    key_t semkey = ftok("text.txt", 3);
    if (semkey == -1)
    {
        perror("Can't ftok.\n");
        exit(1);
    }
    int semid = semget(semkey, 5, perms | IPC_CREAT);
    if (semid == -1)
    {
        perror("Can't semget.\n");
        exit(1);
    }
    if (semctl(semid, ACT_READER, SETVAL, 0) == -1)
    {
        perror("Can't semctl.\n");
        exit(1);
    }
    if (semctl(semid, ACT_WRITER, SETVAL, 0) == -1)
    {
        perror("Can't semctl.\n");
        exit(1);
    }
    if (semctl(semid, QUEUE_WRITER, SETVAL, 0) == -1)
    {
        perror("Can't semctl.\n");
        exit(1);
    }
    if (semctl(semid, QUEUE_READER, SETVAL, 0) == -1)
    {
        perror("Can't semctl.\n");
        exit(1);
    }
    if (semctl(semid, BIN_SEM, SETVAL, 1) == -1)
    {
        perror("Can't semctl.\n");
        exit(1);
    }
    for (int i = 1; i < 3; i++)
    {
        int child_pid;
        if ((child_pid = fork()) == -1)
        {
            perror("Can't fork (writer).");
            exit(1);
        }
        else if (child_pid == 0)
        {
            write_data(shmid, semid, i);
            return 0;
        }
    }
    for (int i = 0; i < 4; i++)
    {
        int child_pid;
        if ((child_pid = fork()) == -1)
        {
            perror("Can't fork (reader).");
            exit(1);
        }
        else if (child_pid == 0)
        {
            read_data(shmid, semid, i);
            return 0;
        }
    }
    write_data(shmid, semid, 0);
    for (int i = 0; i < 6; i++)
    {
        int status;
        if (wait(&status) == -1)
            perror("Child process error.\n");
        else if (!WIFEXITED(status))
            fprintf(stderr, "Children process %i terminated.\n", i + 1);
    }
    if (shmdt(addr) == -1)
    {
        perror("Can't shmdt.\n");
        exit(1);
    }
    if (shmctl(shmid, IPC_RMID, NULL) == -1)
    {
        perror("Can't shmctl.\n");
        exit(1);
    }
    if (semctl(semid, IPC_RMID, 0) == -1)
    {
        perror("Can't semctl.\n");
        exit(1);
    }
    return 0;
}
