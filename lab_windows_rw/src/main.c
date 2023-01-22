#include <windows.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>

#define MIN_READER_DELAY 100
#define MIN_WRITER_DELAY 50
#define MAX_READER_DELAY 200
#define MAX_WRITER_DELAY 100

#define CNT_READERS 4
#define CNT_WRITERS 3

HANDLE canRead;
HANDLE canWrite;
HANDLE mutex;

LONG cntWaitingWriters = 0;
LONG cntWaitingReaders = 0;
LONG cntActiveReaders = 0;
bool writerIsActive = false;

HANDLE readerThreads[CNT_READERS];
HANDLE writerThreads[CNT_WRITERS];
int readersID[CNT_READERS];
int writersID[CNT_WRITERS];

LONG value = 0;

void StartRead()
{
	InterlockedIncrement(&cntWaitingReaders);

	if (writerIsActive || cntWaitingWriters > 0)
		WaitForSingleObject(canRead, INFINITE);
	
	InterlockedDecrement(&cntWaitingReaders);
	InterlockedIncrement(&cntActiveReaders);
	
	SetEvent(canRead);
}

void StopRead()
{
	InterlockedDecrement(&cntActiveReaders);

	if (!cntActiveReaders)
		SetEvent(canWrite);
}

DWORD WINAPI Reader(CONST LPVOID param)
{
	int id = *(int *)param;
	int sleepTime;
	for (int i = 0; i < CNT_READERS + CNT_WRITERS; i++)
	{
		sleepTime = rand() % MAX_READER_DELAY + MIN_READER_DELAY;
		Sleep(sleepTime);
		StartRead();
		printf("Reader %d; value = %d; sleep time = %d.\n", id, value, sleepTime);
		StopRead();
	}
}

void StartWrite()
{
	InterlockedIncrement(&cntWaitingWriters);

	if (writerIsActive  || cntActiveReaders > 0)
		WaitForSingleObject(canWrite, INFINITE);
	
	WaitForSingleObject(mutex, INFINITE);
	
	InterlockedDecrement(&cntWaitingWriters);
	//writerIsActive  = true;
}

void StopWrite()
{
	ResetEvent(canWrite);
	//writerIsActive  = false;
	ReleaseMutex(mutex);
	
	if (cntWaitingWriters)
		SetEvent(canWrite);
	else
		SetEvent(canRead);
}

DWORD WINAPI Writer(CONST LPVOID param)
{
	int id = *(int *)param;
	int sleepTime;	
	for (int i = 0; i < CNT_READERS + CNT_WRITERS; i++)
	{
		sleepTime = rand() % MAX_WRITER_DELAY + MIN_WRITER_DELAY;
		Sleep(sleepTime);
		StartWrite();
		InterlockedIncrement(&value);
		printf("Writer %d; value = %d; sleep time = %d.\n", id, value, sleepTime);
		StopWrite();
	}
}

int init()
{
	mutex = CreateMutex(NULL, FALSE, NULL);
	if (mutex == NULL)
	{
		perror("CreateMutex\n");
		return -1;
	}

	if ((canWrite = CreateEvent(NULL, TRUE, FALSE, NULL)) == NULL)
	{
		perror("CreateEvent (canWrite)");
		return -1;
	}
	
	if ((canRead = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL)
	{
		perror("CreateEvent (canRead)");
		return -1;
	}
	
	return 0;
}

int CreateThreads()
{
	for (short i = 0; i < CNT_WRITERS; i++)
	{
		writersID[i] = i;
		if ((writerThreads[i] = CreateThread(NULL, 0, &Writer, writersID + i, 0, NULL)) == NULL)
		{
			perror("CreateThread (writer)");
			return -1;
		}
	}
	
	for (short i = 0; i < CNT_READERS; i++)
	{
		readersID[i] = i;
		if ((readerThreads[i] = CreateThread(NULL, 0, &Reader, readersID + i, 0, NULL)) == NULL)
		{
			perror("CreateThread (reader)");
			return -1;
		}
	}

	return 0;
}

void Close()
{
	for (int i = 0; i < CNT_READERS; i++)
		CloseHandle(readerThreads[i]);

	for (int i = 0; i < CNT_WRITERS; i++)
		CloseHandle(writerThreads[i]);

	CloseHandle(canRead);
	CloseHandle(canWrite);
	CloseHandle(mutex);
}

int main(void)
{
	setbuf(stdout, NULL);
	srand(time(NULL));

	int rc = init();
	if (rc)
		return -1;

	rc = CreateThreads();
	if (rc)
		return -1;

	WaitForMultipleObjects(CNT_WRITERS, writerThreads, TRUE, INFINITE);
	WaitForMultipleObjects(CNT_READERS, readerThreads, TRUE, INFINITE);

	Close();
	return 0;
}