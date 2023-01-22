#include "writer.h"

void writer()
{
  srand(getpid());
  for (int i = 0; i < 4; i++)
  {
    usleep(rand() % 100000);
    int offset = rand() % 26;

    if (start_write() == -1)
    {
      perror("cant start_write");
      exit(EXIT_FAILURE);
    }
    increment_number();
    printf("writer incremented, current value = \'%d\'\n", read_number());
    if (stop_write() == -1)
    {
      perror("cant stop_write");
      exit(EXIT_FAILURE);
    }
  }
  exit(EXIT_SUCCESS);
}
