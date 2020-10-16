CC      = gcc
CFLAGS  = -g
# Fixing the issue of cleaning on windows
ifeq ($(OS),Windows_NT)
	RM      = del
else
	RM      = rm -r
endif

default:	all

all:	mir

# Build project
mir:	main.c fileio.c parser.c utils.c help.c config.c
	$(CC) $(CFLAGS) -o mir main.c fileio.c parser.c utils.c help.c config.c

# Clean target
clean:
	$(RM) *.o *.exe
