#include <stdio.h>
#include <time.h>
#include <windows.h>

HANDLE mutex, can_read, can_write;
HANDLE reader_threads[4], writer_threads[3];
LONG writers_in_queue = 0, readers_in_queue = 0, active_readers = 0;
int data = 0;

void start_read(void)
{   
	InterlockedIncrement(&readers_in_queue);
	if (WaitForSingleObject(can_write, 0) == WAIT_OBJECT_0)
		WaitForSingleObject(can_read, INFINITE);
	WaitForSingleObject(mutex, INFINITE);
	InterlockedDecrement(&readers_in_queue);
	InterlockedIncrement(&active_readers);
	SetEvent(can_read);
	ReleaseMutex(mutex);
}

void stop_read(void)
{
	InterlockedDecrement(&active_readers);
	if (active_readers == 0)
		SetEvent(can_write);
}

void start_write(void)
{
	InterlockedIncrement(&writers_in_queue);
	if (active_readers > 0)
		WaitForSingleObject(can_write, INFINITE);
	InterlockedDecrement(&writers_in_queue);
}

void stop_write(void)
{
	ResetEvent(can_write);
	if (readers_in_queue > 0)
		SetEvent(can_read);
	else
		SetEvent(can_write);
}

DWORD WINAPI read_data(LPVOID ptr)
{	
    int id = *(int *)ptr;
	srand(time(NULL) + id);
    for (int i = 0; i < 5; i++)
    {
		int stime = rand() % 600 + 500;
        Sleep(stime);
        start_read();
        printf("Reader #%d read:  %d.\n", id + 1, data);
        stop_read();
    }
    return 0;
}

DWORD WINAPI write_data(LPVOID ptr)
{
    int writer_id = *(int *)ptr;
	srand(time(NULL) + writer_id);
    for (int i = 0; i < 5; i++)
    {
        int stime = rand() % 300 + 500;
        Sleep(stime);
        start_write();
        printf("Writer #%d wrote: %d.\n", writer_id + 1, ++data);
        stop_write();
    }
    return 0;
}

int main(void)
{
	setbuf(stdout, NULL);
	int readers_id[4], writers_id[3];
    DWORD id = 0;
    if ((mutex = CreateMutex(NULL, FALSE, NULL)) == NULL)
    {
        perror("Can't CreateMutex.\n");
        exit(1);
    }
    if ((can_read = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL)
    {
        perror("Can't CreateEvent (can_read).\n");
        exit(1);
    }
    if ((can_write = CreateEvent(NULL, TRUE, FALSE, NULL)) == NULL)
    {
        perror("Can't CreateEvent (can_write).\n");
        exit(1);
    }
    for (int i = 0; i < 4; i++)
    {
        readers_id[i] = i;
        if ((reader_threads[i] = CreateThread(NULL, 0, read_data, readers_id + i, 0, &id)) == NULL)
        {
            perror("Can't CreateThread (reader).\n");
            exit(1);
        }
    }
    for (int i = 0; i < 3; i++)
    {
        writers_id[i] = i;
        if ((writer_threads[i] = CreateThread(NULL, 0, write_data, writers_id + i, 0, &id)) == NULL)
        {
            perror("Can't CreateThread (writer).\n");
            exit(1);
        }
    }
	WaitForMultipleObjects(4, reader_threads, TRUE, INFINITE);
	WaitForMultipleObjects(3, writer_threads, TRUE, INFINITE);
    for (int i = 0; i < 4; i++)
        CloseHandle(reader_threads[i]);
    for (int i = 0; i < 3; i++)
        CloseHandle(writer_threads[i]);
    CloseHandle(can_read);
    CloseHandle(can_write);
    CloseHandle(mutex);
	return 0;
}
