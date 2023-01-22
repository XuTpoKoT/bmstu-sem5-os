#include <sys/wait.h>

#include "writer.h"
#include "reader.h"

void log_status_info(pid_t pid, int status) {
  if (WIFEXITED(status)) 
    printf("child with pid %d has finished, code: %d\n", pid, WEXITSTATUS(status));
  else if (WIFSIGNALED(status))
    printf("child with pid %d has finished by unhandlable signal, signum: %d\n", pid, WTERMSIG(status));
  else if (WIFSTOPPED(status))
    printf("child with pid %d has finished by signal, signum: %d\n", pid, WSTOPSIG(status));
}

void log_exit(const char *msg)
{
  perror(msg);
  exit(EXIT_FAILURE);
}

int main(void) 
{
  const int wr_count = 3;
  const int rd_count = 4;
  const int perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

  if (allocate_monitor(perms))
    log_exit("cant allocate_number");
  if (init_monitor())
    log_exit("cant init_monitor");

  pid_t chpid[wr_count + rd_count];
  for (int i = 0; i < wr_count; i++) 
  {
    if ((chpid[i] = fork()) == -1)
      log_exit("cant fork");
    else if (chpid[i] == 0)
      writer();
  }

  for (int i = wr_count; i < rd_count + wr_count; i++) 
  {
    if ((chpid[i] = fork()) == -1)
      log_exit("cant fork");
    else if (chpid[i] == 0)
      reader();
  }

  for (int i = 0; i < rd_count + wr_count; i++)
  {
    int status;
    if (waitpid(chpid[i], &status, WUNTRACED) == -1)
      log_exit("cant wait");
    log_status_info(chpid[i], status);
  }

  if (free_monitor())
    log_exit("cant free_number");

  return EXIT_SUCCESS;
}
