TARGET := red
RAYLIB_PREFIX := "./raylib-5.5_linux_amd64"
CC ?= gcc
CFLAGS ?= -Wall -Wextra -O0 -I$(RAYLIB_PREFIX)/include -DMAIN_WINDOW_TITLE="\"$(TARGET)\""
# LDFLAGS=-v
LDLIBS := $(RAYLIB_PREFIX)/lib/libraylib.a -lm
SRCS := $(filter-out test_%.c, $(wildcard *.c))
OBJS := $(SRCS:.c=.o)
DEPDIR := .deps
DEPS := $(addprefix $(DEPDIR)/, $(OBJS:.o=.d))

obj_rule = $(CC) $(1) -MMD -MF $(DEPDIR)/$(patsubst %.o,%.d,$@) -c $< -o $@

ifeq ($(DEBUG), 1)
CFLAGS += -DDEBUG -ggdb
endif

.PHONY: all clean test

all: $(TARGET)

$(DEPDIR):
	mkdir -p $(DEPDIR)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJS) $(LDLIBS)

%.o: %.c $(MAKEFILE_LIST) | $(DEPDIR)
	$(call obj_rule, $(CFLAGS))

-include $(DEPS)

TEST_TARGET := unittest
TEST_CFLAGS := -ggdb -march=x86-64 -mtune=generic -fsanitize=address

test: $(TEST_TARGET)
	./$(TEST_TARGET)

TEST_SRCS := $(wildcard test_*.c)
TEST_OBJS := $(patsubst %.o,%.test.o,$(filter-out main.o,$(OBJS)))
TEST_DEPS := $(addprefix $(DEPDIR)/, $(TEST_OBJS:.o=.d))

-include $(TEST_DEPS)

%.test.o: %.c $(MAKEFILE_LIST) | $(DEPDIR)
	$(call obj_rule, $(CFLAGS) $(TEST_CFLAGS))

$(TEST_TARGET): $(TEST_SRCS) $(TEST_OBJS)
	$(CC) $(CFLAGS) $(TEST_CFLAGS) $^ -o $@ -lcriterion $(LDLIBS)

clean:
	rm -rf $(DEPDIR) *.o $(TARGET) $(TEST_TARGET)
