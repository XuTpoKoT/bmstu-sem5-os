#include "producer.h"
#include "consumer.h"

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
    const int rd_count = 3;
    const int cn_count = 3;
    int shmid;
    struct buffer *buf;
    int perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

    key_t shmkey = ftok("/tmp", 1);
    if (shmkey == -1)
        log_exit("cant ftok");
    if ((shmid = shmget(shmkey, sizeof(char*)*4 + (1 + BUFFER_LEN) * sizeof(char), IPC_CREAT | perms)) == -1)
        log_exit("cant allocate_buffer");
    if (init_buffer(&buf, shmid, perms) == -1)
        log_exit("cant init_buffer");

    key_t semkey = ftok("/tmp", 2);
    if (semkey == -1)
        log_exit("cant ftok");
    int semid = semget(semkey, 3, IPC_CREAT | perms);
    if (semid == -1)
        log_exit("cant semget");

    if (semctl(semid, SEM_BIN, SETVAL, 1) == -1)
        log_exit("cant semctl");
    if (semctl(semid, SEM_EMPTY, SETVAL, BUFFER_LEN) == -1)
        log_exit("cant semctl");
    if (semctl(semid, SEM_FULL, SETVAL, 0) == -1)
        log_exit("cant semctl");

    pid_t chpid[rd_count + cn_count];
    for (int i = 0; i < rd_count; i++)
    {
        if ((chpid[i] = fork()) == -1)
            log_exit("cant fork");
        if (chpid[i] == 0) 
            producer(semid, &buf->paddr, &buf->cur_char);
    }

    for (int i = rd_count; i < cn_count + rd_count; i++)
    {
        if ((chpid[i] = fork()) == -1)
            log_exit("cant fork");
        if (chpid[i] == 0)
            consumer(semid, &buf->caddr);
    }

    for (int i = 0; i < rd_count + cn_count; i++)
    {
        int status;
        if (waitpid(chpid[i], &status, WUNTRACED) == -1)
            log_exit("cant wait");
        log_status_info(chpid[i], status);
    }

    if (free_buffer_and_sem(buf, shmid, semid) == -1)
        log_exit("cant free_buffer");

    return EXIT_SUCCESS;
}
