#include "monitor.h"

int *number;
int shmid;
int semid;

int allocate_monitor(int perms)
{
  key_t semkey = ftok("/", 1);
  if (semkey == -1)
    return EXIT_FAILURE;
  if ((semid = semget(semkey, 5, IPC_CREAT | perms)) == -1)
    return EXIT_FAILURE;
  key_t shmkey = ftok("/", 2);
  if (shmkey == -1)
    return EXIT_FAILURE;
  shmid = shmget(shmkey, sizeof(int), IPC_CREAT | perms);
  if (shmid == -1)
    return EXIT_FAILURE;
  number = shmat(shmid, 0, perms);
  if (number == (int *)-1)
    return EXIT_FAILURE;
  *number = 0;
  return EXIT_SUCCESS;
}

int init_monitor()
{
  if (semctl(semid, ACTIVE_READERS, SETVAL, 0) == -1)
		return EXIT_FAILURE;
	if (semctl(semid, WRITE_QUEUE, SETVAL, 0) == -1)
		return EXIT_FAILURE;
	if (semctl(semid, READ_QUEUE, SETVAL, 0) == -1)
		return EXIT_FAILURE;
	if (semctl(semid, ACTIVE_WRITER, SETVAL, 0) == -1)
		return EXIT_FAILURE;
	if (semctl(semid, BIN_WRITER, SETVAL, 1) == -1)
		return EXIT_FAILURE;
  return EXIT_SUCCESS;
}

void increment_number()
{
  (*number)++;
}

int read_number()
{
  return *number;
}

int free_monitor()
{
  if (semctl(semid, 1, IPC_RMID, NULL) == -1)
    return EXIT_FAILURE;
  if (shmdt(number) == -1)
    return EXIT_FAILURE;
  return shmctl(shmid, IPC_RMID, NULL) == -1;
}

struct sembuf start_read_sbuf[] = {
  {READ_QUEUE, 1, 0},
  {ACTIVE_WRITER, 0, 0},
  //{BIN_WRITER, 0, 0},
  {WRITE_QUEUE, 0, 0},
  {ACTIVE_READERS, 1, 0},
  {READ_QUEUE, -1, 0},
};
struct sembuf stop_read_sbuf[] = {
  {ACTIVE_READERS, -1, 0},
};
struct sembuf start_write_sbuf[] = {
  {WRITE_QUEUE, 1, 0},
  {ACTIVE_WRITER, 0, 0},
  {ACTIVE_WRITER, 1, 0},
  {ACTIVE_READERS, 0, 0},
  {BIN_WRITER, -1, 0},
  {WRITE_QUEUE, -1, 0},
};
struct sembuf stop_write_sbuf[] = {
  {ACTIVE_WRITER, -1, 0},
  {BIN_WRITER, 1, 0},
};

int start_read()
{
  return semop(semid, start_read_sbuf, 5);
}
int stop_read()
{
  return semop(semid, stop_read_sbuf, 1);
}
int start_write()
{
  return semop(semid, start_write_sbuf, 6);
}
int stop_write()
{
  return semop(semid, stop_write_sbuf, 2);
}
