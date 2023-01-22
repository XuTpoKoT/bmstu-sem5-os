#include "buffer.h"

int init_buffer(struct buffer **buf, int shmid, int perms)
{
  *buf = shmat(shmid, 0, perms);
  if (*buf == (struct buffer *)-1)
    return EXIT_FAILURE;

  (*buf)->addr = ((char *)*buf + sizeof(struct buffer) + 1);
  char *tmp = (*buf)->addr;
  for (int i = 0; i < BUFFER_LEN; i++, tmp++)
    *tmp = 0;

  (*buf)->cur_char = (char *)*buf + sizeof(struct buffer);
  *((*buf)->cur_char) = 'a';
  (*buf)->paddr = (*buf)->addr;
  (*buf)->caddr = (*buf)->addr;
  return EXIT_SUCCESS;
}

char read_from_buffer(char **caddr)
{
	char s = **caddr;
	//**caddr = 0;
  //printf("caddr %p\n", *caddr);
	(*caddr)++;
	return s;
}

void write_in_buffer(char **paddr, char *cur_char)
{
	**paddr = *cur_char;
  //printf("paddr %p\n", *paddr);
	(*paddr)++;
}

int free_buffer_and_sem(struct buffer *buf, int shmid, int semid)
{
  if (shmdt((void *)buf->addr))
    return EXIT_FAILURE;
  if (semctl(semid, 1, IPC_RMID, NULL) == -1)
    return EXIT_FAILURE;
  return shmctl(shmid, IPC_RMID, NULL) == -1;
}
