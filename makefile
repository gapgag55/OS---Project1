# build an executable named myprog from myprog.c
all: scheduler.c
	clear
	$(RM) scheduler
	gcc -o scheduler scheduler.c
	./scheduler testcase.txt 4

run:
	./scheduler testcase.txt FCFS
