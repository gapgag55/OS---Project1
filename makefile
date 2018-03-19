# build an executable named myprog from myprog.c
all: scheduler.c
	clear
	$(RM) scheduler
	gcc -o scheduler scheduler.c
	./scheduler text.txt 4

run:
	./scheduler text.txt FCFS
