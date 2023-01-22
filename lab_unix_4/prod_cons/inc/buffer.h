#ifndef BUFFER_H
#define BUFFER_H

#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <limits.h>
#include <time.h>
#include <stdio.h>
#include <sys/sem.h>

#define BUFFER_LEN 26

struct buffer {
  char *addr;
  char *paddr;
  char *caddr;
  char *cur_char;
};

int init_buffer(struct buffer **buf, int shmid, int perms);
char read_from_buffer(char **caddr);
void write_in_buffer(char **paddr, char *cur_char);
int free_buffer_and_sem(struct buffer *buf, int shmid, int semid);

#define SEM_BIN 0
#define SEM_FULL 1
#define SEM_EMPTY 2

#endif
