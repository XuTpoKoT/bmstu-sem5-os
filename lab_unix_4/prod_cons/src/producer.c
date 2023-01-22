#include "producer.h"

struct sembuf swrite_sbuf[] = {
  {SEM_EMPTY, -1, 0},
  {SEM_BIN, -1, 0},
};
struct sembuf ewrite_sbuf[] = {
  {SEM_BIN, 1, 0},
  {SEM_FULL, 1, 0},
};

int producer(int semid, char **paddr, char **cur_char)
{
  srand(getpid());
  for (int i = 0; i < 4; i++)
  {
    usleep(rand() % 70000);
    if (semop(semid, swrite_sbuf, 2) == -1) {
      perror("cant semop");
      exit(EXIT_FAILURE);
    }
    write_in_buffer(paddr, *cur_char);
    printf("producer write \'%c\'\n", **cur_char);
    (**cur_char)++;
    if (semop(semid, ewrite_sbuf, 2) == -1) {
      perror("cant semop");
      exit(EXIT_FAILURE);
    }
  }
  exit(EXIT_SUCCESS);
}
