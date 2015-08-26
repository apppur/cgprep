STANDALONE=main.exe

CFLAGS=-g -Wall
CC=gcc

LIBS:=-L/usr/local/lib -lglfw3 -lopengl32 -lgdi32


.PHONY : all clean

all : $(STANDALONE)

$(STANDALONE) : source/main.c
	$(CC) $(CFLAGS) -o $@  $< $(LIBS)

clean :
	rm -rf build