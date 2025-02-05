CC = gcc
CFLAGS = -Wall -g

SRCS = $(wildcard src/*.c)

SRCS_DEBUG = $(SRCS)
SRCS_RELEASE = $(filter-out src/debug.c, $(SRCS))

OBJS_DEBUG = $(SRCS_DEBUG:.c=.o)
OBJS_RELEASE = $(SRCS_RELEASE:.c=.o)

TARGET_DEBUG = src/hc.exe
TARGET_RELEASE = hc.exe

all: hc_debug

hc_debug: CFLAGS += -DDEBUG
hc_debug: $(TARGET_DEBUG)

hc_release: CFLAGS := -O2 -s
hc_release: $(TARGET_RELEASE)

$(TARGET_DEBUG): $(OBJS_DEBUG)
	$(CC) $(CFLAGS) -o $(TARGET_DEBUG) $(OBJS_DEBUG)

$(TARGET_RELEASE): $(OBJS_RELEASE)
	$(CC) $(CFLAGS) -o $(TARGET_RELEASE) $(OBJS_RELEASE)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
ifeq ($(OS),Windows_NT)
	del /F /Q src\*.o src\*.exe
else
	rm -f src/*.o src/*.exe
endif

.PHONY: all hc_debug hc_release clean
