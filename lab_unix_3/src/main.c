#include "mydaemon.h"

void logtime()
{
    time_t s;
    struct tm* current_time;
    s = time(NULL);
    current_time = localtime(&s);

    syslog(LOG_INFO, "%02d:%02d:%02d",
           current_time->tm_hour,
           current_time->tm_min,
           current_time->tm_sec);
}

int main(int argc, char *argv[])
{
	printf("daemon\n");
	int err;
	char *cmd;
	struct sigaction sa;
    pthread_t tid;
	if ((cmd = strrchr(argv[0], '/')) == NULL)
		cmd = argv[0];
	else
		cmd++;
		
	// Перейти в режим демона.
	daemonize(cmd);
	
	if (already_running())
	{
		syslog(LOG_ERR, "демон уже запущен");
		exit(1);
	}
	
	// Восстановить действие по умолчанию для сигнала SIGHUP1 и заблокировать все сигналы.
	sa.sa_handler = SIG_DFL;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;	
	if (sigaction(SIGHUP, &sa, NULL) == -1)
		err_quit("%s: невозможно восстановить действие SIG_DFL для SIGHUP");
	sigset_t mask;
	sigfillset(&mask);
	if ((err = pthread_sigmask(SIG_BLOCK, &mask, NULL)) != 0)
		err_exit(err, "ошибка выполнения операции SIG_BLOCK");
	
	// Создать поток для обработки SIGHUP и SIGTERM.
	err = pthread_create(&tid, NULL, thr_fn, &mask);
	if (err != 0)
		err_exit(err, "невозможно создать поток");
  
	for (;;) 
	{
		sleep(1);
		logtime();

	}
}
	
	
