TARGET=rayedit
RAYLIB_PREFIX="./raylib-5.5_linux_amd64"
CC=gcc
CFLAGS=-Wall -Wextra -O0 -I${RAYLIB_PREFIX}/include -DMAIN_WINDOW_TITLE="\"${TARGET}\"" -ggdb -DDEBUG
# LDFLAGS=-v
LDLIBS=${RAYLIB_PREFIX}/lib/libraylib.a -lm
OBJS = main.o line.o

.PHONY=clean all

all: ${TARGET}

${TARGET}: ${OBJS}
	${CC} ${LDFLAGS} -o ${TARGET} ${OBJS} ${LDLIBS}

main.o: main.c line.h config.h vector.h makefile
	${CC} ${CFLAGS} -c $< -o $@

line.o: line.c config.h makefile
	${CC} ${CFLAGS} -c $< -o $@

.PHONY+=test
TEST_TARGET=unittest
TEST_PREFIX=./test

test: ${TEST_TARGET}
	./${TEST_TARGET}

TEST_SRCS=test.c
TEST_OBJS=$(filter-out main.o, $(OBJS))

${TEST_TARGET}: ${TEST_SRCS} ${TEST_OBJS}
	${CC} ${CFLAGS} ${TEST_SRCS} ${TEST_OBJS} -o $@ -lcriterion ${LDLIBS}

clean:
	rm -rf ${OBJS} ${TARGET} ${TEST_TARGET}

