#include "mydaemon.h"

void reread(){};

int lockfile(int fd)
{
	struct flock fl;

	fl.l_type = F_WRLCK;
	fl.l_start = 0;
	fl.l_whence = SEEK_SET;
	fl.l_len = 0;
	return(fcntl(fd, F_SETLK, &fl));
}

static void err_doit(int errnoflag, int error, const char *fmt, va_list ap)
{
	char buf[MAXLINE];

	vsnprintf(buf, MAXLINE-1, fmt, ap);
	if (errnoflag)
		snprintf(buf+strlen(buf), MAXLINE-strlen(buf)-1, ": %s",
		  strerror(error));
	strcat(buf, "\n");
	fflush(stdout);		/* in case stdout and stderr are the same */
	fputs(buf, stderr);
	fflush(NULL);		/* flushes all stdio output streams */
}

void err_quit(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    exit(EXIT_FAILURE);
}

/*
 * Fatal error unrelated to a system call.
 * Error code passed as explict parameter.
 * Print a message and terminate.
 */
void err_exit(int error, const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	err_doit(1, error, fmt, ap);
	va_end(ap);
	exit(1);
}

void daemonize(const char *cmd)
{
	int					fd0, fd1, fd2;
	pid_t				pid;
	struct rlimit		rl;
	struct sigaction	sa;

	// Clear file creation mask.
	umask(0);

	// Get maximum number of file descriptors.
	if (getrlimit(RLIMIT_NOFILE, &rl) == -1)
		err_quit("%s: can't get file limit", cmd);

	// Become a session leader to lose controlling TTY.
	if ((pid = fork()) == -1)
		err_quit("%s: can't fork", cmd);
	if (pid > 0) // parent
		exit(0);
		
	// Ensure future opens won't allocate controlling TTYs.
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGHUP, &sa, NULL) < 0)
		err_quit("%s: can't ignore SIGHUP", cmd);
		
	if (setsid() == -1)
		err_quit("%s: can't setsid", cmd);

	/*
	 * Change the current working directory to the root, so
	 * we won't prevent file systems from being unmounted.
	 */
	if (chdir("/") < 0)
		err_quit("%s: can't change directory to /", cmd);

	// Close all open file descriptors.
	if (rl.rlim_max == RLIM_INFINITY)
		rl.rlim_max = 1024;
	for (int i = 0; i < rl.rlim_max; i++)
		close(i);

	// Attach file descriptors 0, 1, and 2 to /dev/null.
	fd0 = open("/dev/null", O_RDWR);
	fd1 = dup(0);
	fd2 = dup(0);

	// Initialize the log file.
	openlog(cmd, LOG_CONS, LOG_DAEMON);
	if (fd0 != 0 || fd1 != 1 || fd2 != 2)
	{
		syslog(LOG_ERR, "Error stdin, stdout, stderr");
		exit(1);
	}
}

void *thr_fn(void *arg)
{
	int signo;
    sigset_t *mask = (sigset_t *)arg;
	for (;;)
    {
		if (sigwait(mask, &signo) == -1)
        {
			syslog(LOG_ERR, "ошибка вызова функции sigwait");
			exit(1);
		}
		switch (signo)
        {
            case SIGHUP:
                syslog(LOG_INFO, "Чтение конфигурационного файла");
                reread();
                break;
            case SIGTERM:
                syslog(LOG_INFO, "получен сигнал SIGTERM; выход");
                exit(0);
            default:
                syslog(LOG_INFO, "получен непредвиденный сигнал %d\n", signo);
		}
	}
}

int already_running(void)
{
	int	fd;
	char buf[16];
	fd = open(LOCKFILE, O_RDWR|O_CREAT, LOCKMODE);
	if (fd < 0) 
	{
		syslog(LOG_ERR, "невозможно открыть %s: %s",
		    LOCKFILE, strerror(errno));
		exit(1);
	}
	if (lockfile(fd) == -1) 
	{
		if (errno == EACCES || errno == EAGAIN)
        {
			close(fd);
			exit(1);
		}
		syslog(LOG_ERR, "невозможно установить блокировку на %s: %s",
		    LOCKFILE, strerror(errno));
		exit(1);
	}
	ftruncate(fd, 0);
	sprintf(buf, "%ld", (long)getpid());
	write(fd, buf, strlen(buf)+1);
	return 0;
}

