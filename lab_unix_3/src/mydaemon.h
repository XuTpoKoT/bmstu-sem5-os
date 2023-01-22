#include <pthread.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

//  ps -ajx | grep ./app_dem
#define	MAXLINE	4096
#define LOCKFILE "/var/run/mydaemon.pid"
#define LOCKMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

void daemonize(const char *cmd);
int already_running(void);
int lockfile(int fd);
void reread(void);
void err_quit(const char *fmt, ...);
void err_exit(int error, const char *fmt, ...);
void *thr_fn(void *arg);


	
