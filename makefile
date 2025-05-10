TARGET=red
RAYLIB_PREFIX="./raylib-5.5_linux_amd64"
CC=gcc
CFLAGS=-Wall -Wextra -O0 -I${RAYLIB_PREFIX}/include -DMAIN_WINDOW_TITLE="\"${TARGET}\""
# LDFLAGS=-v
LDLIBS=${RAYLIB_PREFIX}/lib/libraylib.a -lm
OBJS = main.o line.o editor.o

ifeq ($(DEBUG), 1)
CFLAGS += -DDEBUG -ggdb
endif

.PHONY=clean all

all: ${TARGET}

${TARGET}: ${OBJS}
	${CC} ${LDFLAGS} -o ${TARGET} ${OBJS} ${LDLIBS}

main.o: main.c line.h config.h vector.h editor.h slice.h makefile
	${CC} ${CFLAGS} -c $< -o $@

line.o: line.c config.h slice.h makefile
	${CC} ${CFLAGS} -c $< -o $@

editor.o: editor.c line.h slice.h makefile
	${CC} ${CFLAGS} -c $< -o $@

.PHONY+=test
TEST_TARGET=unittest
TEST_PREFIX=./test

TEST_CFLAGS = -ggdb -march=x86-64 -mtune=generic -fsanitize=address

test: ${TEST_TARGET}
	./${TEST_TARGET}

TEST_SRCS=test_vector.c test_line.c test_editor.c
TEST_OBJS=$(filter-out main.o, $(OBJS))

${TEST_TARGET}: ${TEST_SRCS} ${TEST_OBJS}
	${CC} ${CFLAGS} ${TEST_CFLAGS} ${TEST_SRCS} ${TEST_OBJS} -o $@ -lcriterion ${LDLIBS}

clean:
	rm -rf ${OBJS} ${TARGET} ${TEST_TARGET}
