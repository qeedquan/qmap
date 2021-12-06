CC=cc
CFLAGS=`sdl2-config --cflags` -Wall -Wextra
CFLAGS+=-ggdb -g3 -fsanitize=address
LDFLAGS=`sdl2-config --libs` -lm

BIN=qmap

all:
	$(CC) -o $(BIN) src/*.c $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(BIN)

