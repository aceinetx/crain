CC = cc
CFLAGS = -std=c90 -Wall -Wextra -Wpedantic -ggdb -fsanitize=address
LDFLAGS = -lncursesw -fsanitize=address

all: crain
clean:
	rm -rf crain *.o

crain: crain.o
	$(CC) $(LDFLAGS) -o $@ $< 

crain.o: crain.c
	$(CC) $(CFLAGS) -c -o $@ $< 
