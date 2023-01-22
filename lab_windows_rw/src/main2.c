// Надо использовать неделимые операции:
// InterLockedIncrement, InterLockedDecrement.
// В программе должно быть 3 счетчика:
// ждущих писателей, ждущих читателей и активных читателей.
// Активный писатель м.б. только один и это логический тип.

#include <windows.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>

#define MIN_READER_DELAY 100
#define MIN_WRITER_DELAY 100
#define MAX_READER_DELAY 200
#define MAX_WRITER_DELAY 300

#define CNT_READERS 5
#define CNT_WRITERS 3
#define ITERATIONS 8

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

int readersRand[CNT_READERS * ITERATIONS];
int writersRand[CNT_READERS * ITERATIONS];

int value = 0;

void StartRead()
{
	InterlockedIncrement(&cntWaitingReaders);

	if (writerIsActive  || WaitForSingleObject(canWrite, 0) == WAIT_OBJECT_0)
		WaitForSingleObject(canRead, INFINITE);

	WaitForSingleObject(mutex, INFINITE);
	
	InterlockedDecrement(&cntWaitingReaders);
	InterlockedIncrement(&cntActiveReaders);
	
	// Чтобы следующий читатель в очереди
	// Читателей смог начать чтение
	SetEvent(canRead);
	
	// освободить мьютекс. 
	ReleaseMutex(mutex);
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
	int begin = id * ITERATIONS;
	for (int i = 0; i < ITERATIONS; i++)
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

	InterlockedDecrement(&cntWaitingWriters);

	writerIsActive  = true;
	ResetEvent(canWrite);
}

void StopWrite()
{
	writerIsActive  = false;
	
	if (cntWaitingReaders)
		SetEvent(canRead);
	else
		SetEvent(canWrite);		
}

DWORD WINAPI Writer(CONST LPVOID param)
{
	int id = *(int *)param;
	int sleepTime;
	int begin = id * ITERATIONS;
	
	for (int i = 0; i < ITERATIONS; i++)
	{
		sleepTime = rand() % MAX_WRITER_DELAY + MIN_WRITER_DELAY;
		Sleep(sleepTime);

		StartWrite();
		++value;
		printf("Writer %d; value = %d; sleep time = %d.\n", id, value, sleepTime);
		StopWrite();
	}
}

int init()
{
	// 1 == NULL => the handle cannot be inherited by child processes.
	// 2 == false значит мьютекс свободный.
	// 3 имя мьютекса
	mutex = CreateMutex(NULL, FALSE, NULL);
	if (mutex == NULL)
	{
		perror("CreateMutex\n");
		return -1;
	}

	// 2 == FALSE значит автоматический сброс.
	// 3 == FALSE значит, что объект не в сигнальном состоянии.
	// 4 имя 
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
		// Параметры слева направо:
		// NULL - Атрибуты защиты определены по умолчанию;
		// 0 - размер стека устанавливается по умолчанию;
		// Reader - определяет адрес функции потока, с которой следует начать выполнение потока;
		// readersID + i - указатель на переменную, которая передается в поток;
		//  0 - исполнение потока начинается немедленно;
		// Последний - адрес переменной типа DWORD, в которую функция возвращает идентификатор потока.
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
	// Закрываем дескрипторы mutex, event и всех созданных потоков.
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

	// TRUE - функция не даст потоку возобновить свою работу, пока не освободятся все объекты.
	// INFINITE - указывает, сколько времени поток готов ждать освобождения объекта.
	WaitForMultipleObjects(CNT_WRITERS, writerThreads, TRUE, INFINITE);
	WaitForMultipleObjects(CNT_READERS, readerThreads, TRUE, INFINITE);

	Close();

	printf("\nFINISH\n");
	return 0;
}