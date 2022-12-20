#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <syslog.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/resource.h>

#include "apue.h"

#define LOCKFILE "/var/run/daemon.pid"

sigset_t mask;

int lockfile(int fd)
{
	struct flock fl;
	fl.l_type = F_WRLCK;
	fl.l_start = 0;
	fl.l_whence = SEEK_SET;
	fl.l_len = 0;
	return(fcntl(fd, F_SETLK, &fl));
}

int already_running(void)
{
	int perms = S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH;
	int fd;
	char buf[16];
	fd = open(LOCKFILE, O_RDWR|O_CREAT, perms);
	if (fd == -1) 
	{
		syslog(LOG_ERR, "Невозможно открыть %s: %s.", LOCKFILE, strerror(errno));
		exit(1);
	}
	if (lockfile(fd) == -1) 
	{
		if (errno == EACCES || errno == EAGAIN) 
		{
			close(fd);
			return 1;
		}
		syslog(LOG_ERR, "Невозможно установить блокировку на %s: %s.", LOCKFILE, strerror(errno));
		exit(1);
	}
	ftruncate(fd, 0);
	sprintf(buf, "%ld", (long)getpid());
	write(fd, buf, strlen(buf) + 1);
	return 0;
}

void daemonize(const char *cmd)
{
	int i, fd0, fd1, fd2;
	pid_t pid;
	struct rlimit rl;
	struct sigaction sa;
	/*
	* Сбросить маску режима создания файла.
	*/
	umask(0);
	/*
	* Получить максимально возможный номер дескриптора файла.
	*/
	if (getrlimit(RLIMIT_NOFILE, &rl) == -1)
		err_quit("%s: невозможно получить максимальный номер дескриптора.", cmd);
	/*
	* Стать лидером нового сеанса, чтобы утратить управляющий терминал.
	*/
	if ((pid = fork()) == -1)
		err_quit("%s: ошибка вызова функции fork.", cmd);
	else if (pid > 0) /* родительский процесс */
		exit(0);
	/*
	* Обеспечить невозможность обретения управляющего терминала в будущем.
	*/
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGHUP, &sa, NULL) == -1)
		err_quit("%s: невозможно игнорировать сигнал SIGHUP.", cmd);
		
	if (setsid() == -1)
		err_quit("%s: ошибка вызова setsid.", cmd);
	/*
	* Назначить корневой каталог текущим рабочим каталогом,
	* чтобы впоследствии можно было отмонтировать файловую систему.
	*/
	if (chdir("/") == -1)
		err_quit("%s: невозможно сделать текущим рабочим каталогом /.", cmd);
	/*
	* Закрыть все открытые файловые дескрипторы.
	*/
	if (rl.rlim_max == RLIM_INFINITY)
		rl.rlim_max = 1024;
	for (i = 0; i < rl.rlim_max; i++)
		close(i);
	/*
	* Присоединить файловые дескрипторы 0, 1 и 2 к /dev/null.
	*/
	fd0 = open("/dev/null", O_RDWR);
	fd1 = dup(0);
	fd2 = dup(0);
	/*
	* Инициализировать файл журнала.
	*/
	openlog(cmd, LOG_CONS, LOG_DAEMON);
	if (fd0 != 0 || fd1 != 1 || fd2 != 2) 
	{
		syslog(LOG_ERR, "Ошибочные файловые дескрипторы %d %d %d.", fd0, fd1, fd2);
		exit(1);
	}
}

void *thr_fn(void *arg)
{
	int err, signo;
	for (;;) 
	{
		err = sigwait(&mask, &signo);
		if (err != 0) 
		{
			syslog(LOG_ERR, "Ошибка вызова функции sigwait.");
			exit(1);
		}		
		switch (signo) 
		{
			case SIGHUP:
				syslog(LOG_INFO, "Чтение конфигурационного файла.");
				break;
			case SIGTERM:
				syslog(LOG_INFO, "Получен сигнал SIGTERM; выход.");
				exit(0);
			default:
				syslog(LOG_INFO, "Получен непредвиденный сигнал %d\n.", signo);
		}
	}
	return 0;
}

int main()
{
	int err;
	pthread_t tid;
	char *cmd;
	struct sigaction sa;
	/*
	* Перейти в режим демона.
	*/
	daemonize(cmd);
	/*
	* Убедиться, что ранее не была запущена другая копия демона.
	*/
	if (already_running()) 
	{
		syslog(LOG_ERR, "Демон уже запущен.");
		exit(1);
	}
	/*
	* Восстановить действие по умолчанию для сигнала SIGHUP
	* и заблокировать все сигналы.
	*/
	sa.sa_handler = SIG_DFL;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGHUP, &sa, NULL) == -1)
		err_quit("%s: невозможно восстановить действие SIG_DFL для SIGHUP.");
	sigfillset(&mask);
	if ((err = pthread_sigmask(SIG_BLOCK, &mask, NULL)) != 0)
		err_exit(err, "Ошибка выполнения операции SIG_BLOCK.");
	/*
	* Создать поток для обработки SIGHUP и SIGTERM.
	*/
	err = pthread_create(&tid, NULL, thr_fn, 0);
	if (err != 0)
		err_exit(err, "Невозможно создать поток.");
	while(1)
	{
		long int cur_time = time(NULL);
		syslog(LOG_INFO, "Текущее время: %s.", ctime(&cur_time));
		sleep(3);
	}
	exit(0);
}
