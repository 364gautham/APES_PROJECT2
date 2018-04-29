all: log.c main.c uart.c
	gcc -o main.out main.c log.c uart.c -lrt -lpthread

clean:
	 find . -type f | xargs touch
	 rm *.out
