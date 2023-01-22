#ifndef PRODUCER_H
#define PRODUCER_H

#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "buffer.h"

int producer(int semid, char **paddr, char **cur_char);

#endif
