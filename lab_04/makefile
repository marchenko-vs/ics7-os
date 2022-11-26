CC := gcc

daemon.out : main.o error.o
	$(CC) main.o error.o -o daemon.out

main.o :
	$(CC) main.c -c

error.o : error.c apue.h
	$(CC) error.c -c

run : daemon.out
	sudo ./daemon.out

log :
	sudo cat /var/log/syslog

clean :
	rm -f *.out *.o
	
