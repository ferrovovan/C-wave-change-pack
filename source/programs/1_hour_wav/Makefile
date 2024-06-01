CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -o2

OS := $(shell uname)
postfix = 

ifeq ($(OS), Linux)
    # Команды для Linux
endif

ifeq ($(OS), Darwin)
    # Команды для macOS
endif

ifeq ($(OS), Windows_NT)
	postfix = .exe
endif


INCLUDE_LIBS = -I../../libraries


1_hour_wav:
	$(CC) $(CFLAGS) $(INCLUDE_LIBS) -o 1_hour_extender$(postfix) 1_hour_wav.c

