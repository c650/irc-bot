CFLAGS = -Wall -Werror

# for libcurl
LDLIBS= -lcurl
LDFLAGS= -L/usr/lib/x86_64-linux-gnu
# Sources for bot.o
SRC= ./src/arr.c ./src/bot.c ./src/connect.c ./src/helper-functions.c ./src/irc-datatypes.c ./src/parsers.c ./src/privmsg-funcs.c
OUT= ./bin/bot.o

build: ${SRC}
	${CC} ${CFLAGS} -o ${OUT} ${SRC} ${LDFLAGS} ${LDLIBS}

.PHONY: clean

clean:
	rm ./bin/bot.o