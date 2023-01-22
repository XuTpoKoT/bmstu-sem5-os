#include "reader.h"

void reader()
{
  srand(getpid());
  for (int i = 0; i < 4; i++)
  {
    usleep(rand() % 100000);
    int offset = rand() % 26;

    if (start_read() == -1)
    {
      perror("cant start_read");
      exit(EXIT_FAILURE);
    }
    printf("reader got value = \'%d\'\n", read_number());
    if (stop_read() == -1)
    {
      perror("cant stop_read");
      exit(EXIT_FAILURE);
    }
  }
  exit(EXIT_SUCCESS);
}
