CC=gcc

CFLAGS= -Wall -Werror

LDFLAGS=
SDL_CONF=`sdl2-config --cflags --libs`

SRCS=src
BUILDS=build

all: $(SRCS)/
	$(CC) -ggdb $(CFLAGS) $(SRCS)/life.c -o $(BUILDS)/life $(SDL_CONF) $(LDFLAGS)

clean:
	rm -rf $(BUILDS)/life
