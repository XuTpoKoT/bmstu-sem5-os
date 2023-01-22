#ifndef MONITOR_H
#define MONITOR_H

#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#define ACTIVE_READERS 0
#define WRITE_QUEUE 1
#define READ_QUEUE 2
#define ACTIVE_WRITER 3
#define BIN_WRITER 4

void increment_number();
int read_number();

int allocate_monitor(int perms);
int init_monitor();
int free_monitor();

int start_read();
int stop_read();
int start_write();
int stop_write();

#endif
