#include "consumer.h"

struct sembuf sread_sbuf[] = {
  {SEM_FULL, -1, 0},
  {SEM_BIN, -1, 0},
};
struct sembuf eread_sbuf[] = {
  {SEM_BIN, 1, 0},
  {SEM_EMPTY, 1, 0},
};

int consumer(int semid, char **caddr)
{
  srand(getpid());
  for (int i = 0; i < 4; i++)
  {
    usleep(rand() % 100000);
    char sym;
    if (semop(semid, sread_sbuf, 2) == -1) {
      perror("cant semop");
      exit(EXIT_FAILURE);
    }
    printf("consumer read \'%c\'\n", read_from_buffer(caddr));
    if (semop(semid, eread_sbuf, 2) == -1) {
      perror("cant semop");
      exit(EXIT_FAILURE);
    }
  }
  exit(EXIT_SUCCESS);
}
