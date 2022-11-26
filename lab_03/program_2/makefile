CC := gcc
CFLAGS := -std=c99 -Wall -Werror -Wvla
OBJS := out/main.o out/matrix.o
UNIT_OBJS := out/check_main.o out/check_m_find_min.o

app.exe : $(OBJS)
	$(CC) $(OBJS) -o ../program_2.out

debug.exe : CFLAGS += -g3 --coverage -ggdb

debug.exe : $(OBJS)
	mkdir -p out/
	$(CC) $(OBJS) --coverage -o debug.exe
    
unit_tests.exe : $(OBJS) $(UNIT_OBJS) 
	mkdir -p out/
	$(CC) out/matrix.o $(UNIT_OBJS) -lcheck -o unit_tests.exe

out/main.o : src/main.c inc/matrix.h
	mkdir -p out/
	$(CC) $(CFLAGS) src/main.c -I inc/ -c -o out/main.o

out/matrix.o : src/matrix.c inc/matrix.h
	$(CC) $(CFLAGS) -I inc/ src/matrix.c -c -o out/matrix.o
	
out/check_main.o : unit_tests/check_main.c inc/matrix.h
	$(CC) $(CFLAGS) -I inc/ unit_tests/check_main.c -c -o out/check_main.o
	
out/check_m_find_min.o : unit_tests/check_m_find_min.c inc/matrix.h
	$(CC) $(CFLAGS) -I inc/ unit_tests/check_m_find_min.c -c -o out/check_m_find_min.o

.PHONY : debug clean unit

debug : debug.exe

unit : unit_tests.exe
	./unit_tests.exe

clean :
	rm -rf out/ *.txt *.exe *.gcov
