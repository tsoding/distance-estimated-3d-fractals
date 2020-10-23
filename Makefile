CFLAGS=-Wall -Werror -std=c11 -pedantic -ggdb
LIBS=

de3df: main.c
	$(CC) $(CFLAGS) -o de3df main.c $(LIBS)
