#ifndef CONSUMER_H
#define CONSUMER_H

#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "buffer.h"

int consumer(int semid, char **caddr);

#endif
