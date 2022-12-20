#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/sem.h>

#define BIN_SEM      0
#define BUFFER_FULL  1
#define BUFFER_EMPTY 2

struct sembuf lock_consumer[2]    = {{ BUFFER_FULL, -1, 0 }, { BIN_SEM, -1, 0 }};
struct sembuf release_consumer[2] = {{ BIN_SEM, 1, 0 }, { BUFFER_EMPTY, 1, 0 }};
struct sembuf lock_producer[2]    = {{ BUFFER_EMPTY, -1, 0 }, { BIN_SEM, -1, 0 }};
struct sembuf release_producer[2] = {{ BIN_SEM, 1, 0 }, { BUFFER_FULL, 1, 0 }};

struct cbuffer
{
    char *addr;
    char *paddr;
    char *caddr;
    char curr_symbol;
};

void consume(int shmid, int semid, int consid)
{
    struct cbuffer *buf = shmat(shmid, NULL, 0);
    if (buf ==(void *)-1)
    {
        perror("Can't shmat.\n");
        exit(1);
    }
    srand(time(NULL) + consid);
    for (int i = 0; i < 8; i++)
    {
        sleep(rand() % 5 + 1);
        if (semop(semid, lock_consumer, 2) == -1)
        {
	        perror("Can't semop (producer).\n");
	        exit(1);
	    }
        printf("Consumer #%d read:  %c.\n", consid + 1, *(buf->caddr++));
        if (semop(semid, release_consumer, 2) == -1)
        {
            perror("Can't semop (consumer).\n");
            exit(1);
	    }
    }
    if (shmdt(buf) == -1)
    {
        perror("Can't shmdt.\n");
        exit(1);
    }
}

void produce(int shmid, int semid, int prodid)
{
    struct cbuffer *buf = shmat(shmid, NULL, 0);
    if (buf == (void *)-1)
    {
        perror("Can't shmat.\n");
        exit(1);
    }
    srand(time(NULL) + prodid);
    for (int i = 0; i < 8; i++)
    {
        sleep(rand() % 3 + 1);
	    if (semop(semid, lock_producer, 2) == -1)
	    {
	        perror("Can't semop (producer).\n");
	        exit(1);
	    }
	    *(buf->paddr) = buf->curr_symbol++;
	    printf("Producer #%d wrote: %c.\n", prodid + 1, *(buf->paddr++));
	    if (semop(semid, release_producer, 2) == -1)
        {
	        perror("Can't semop (producer).\n");
	        exit(1);
	    }
    }
    if (shmdt(buf) == -1)
    {
        perror("Can't shmdt.\n");
        exit(1);
    }
}

int main(void)
{
    key_t shmkey = ftok("text.txt", 1);
    if (shmkey == -1)
    {
        perror("Can't ftok.\n");
        exit(1);
    }
    int perms = S_IRWXU | S_IRWXG | S_IRWXO;
    int shmid = shmget(shmkey, sizeof(struct cbuffer) + 64 * sizeof(char), perms | IPC_CREAT);
    if (shmid == -1)
    {
        perror("Can't shmget.\n");
        exit(1);
    }
    struct cbuffer *buffer = shmat(shmid, NULL, 0);
    if (buffer == (void *)-1)
    {
        perror("Can't shmat.\n");
        exit(1);
    }
    buffer->addr = (char *)buffer + sizeof(struct cbuffer);
    buffer->paddr = buffer->addr;
    buffer->caddr = buffer->addr;
    buffer->curr_symbol = 97; // ASCII 97 - 'a'
    key_t semkey = ftok("text.txt", 1);
    if (semkey == -1)
    {
        perror("Can't ftok.\n");
        exit(1);
    }
    int semid = semget(semkey, 3, perms | IPC_CREAT);
    if (semid == -1)
    {
        perror("Can't semget.\n");
        exit(1);
    }
    if (semctl(semid, BIN_SEM, SETVAL, 1) == -1)
    {
        perror("Can't semctl.\n");
        exit(1);
    }
    if (semctl(semid, BUFFER_FULL, SETVAL, 0) == -1)
    {
        perror("Can't semctl.\n");
        exit(1);
    }
    if (semctl(semid, BUFFER_EMPTY, SETVAL, 64) == -1)
    {
        perror("Can't semctl.\n");
        exit(1);
    }
    for (int i = 1; i < 3; i++)
    {
        int child_pid;
        if ((child_pid = fork()) == -1)
        {
            perror("Can't fork (producer).\n");
            exit(1);
        }
        else if (child_pid == 0)
        {
            produce(shmid, semid, i);
            return 0;
        }
    }
    for (int i = 0; i < 3; i++)
    {
        int child_pid;
        if ((child_pid = fork()) == -1)
        {
            perror("Can't fork (consumer).\n");
            exit(1);
        }
        else if (child_pid == 0)
        {
            consume(shmid, semid, i);
            return 0;
        }
    }
    produce(shmid, semid, 0);
    for (int i = 0; i < 5; i++)
    {
        int status;
        if (wait(&status) == -1)
            perror("Child proccess' error.\n");
        else if (WIFEXITED(status) == 0)
            fprintf(stderr, "Child process #%d terminated.\n", i + 2);
    }
    if (shmdt(buffer) == -1)
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
