TARGET=rayedit
RAYLIB_PREFIX="./raylib-5.5_linux_amd64"
CC=gcc
CFLAGS=-Wall -O3 -I${RAYLIB_PREFIX}/include -DMAIN_WINDOW_TITLE="\"${TARGET}\""
# LDFLAGS=-v
LDLIBS=${RAYLIB_PREFIX}/lib/libraylib.a -lm
OBJS=main.o vec.o

.PHONY=clean all

all: ${TARGET}

${TARGET}: ${OBJS}
	${CC} ${LDFLAGS} -o ${TARGET} ${OBJS} ${LDLIBS}

%.o: %.c %.h makefile
	${CC} ${CFLAGS} -c $< -o $@

clean:
	rm -rf ${OBJS} ${TARGET}
