CC = gcc
CFLAGS = -Wall -Iinclude

SRCS = \
    src/main.c \
    src/error.c \
    src/parse/token.c \
    src/parse/lexer.c \
    src/parse/parser.c \
    src/parse/node.c \
    src/parse/file_lexer.c

SRCS_DEBUG = $(SRCS) src/debug.c

OBJS_DEBUG = \
    build/debug/main.o \
    build/debug/error.o \
    build/debug/token.o \
    build/debug/lexer.o \
    build/debug/parser.o \
    build/debug/node.o \
    build/debug/file_lexer.o \
    build/debug/debug.o

OBJS_RELEASE = \
    build/release/main.o \
    build/release/error.o \
    build/release/token.o \
    build/release/lexer.o \
    build/release/parser.o \
    build/release/node.o \
    build/release/file_lexer.o

TARGET_DEBUG = build/debug/hc.exe
TARGET_RELEASE = build/release/hc.exe

all: hc_debug

hc_debug: CFLAGS += -g -DDEBUG
hc_debug: $(TARGET_DEBUG)

hc_release: CFLAGS := -O2 -s -Iinclude
hc_release: $(TARGET_RELEASE)

$(TARGET_DEBUG): $(OBJS_DEBUG)
	$(CC) $(CFLAGS) -o $@ $^

$(TARGET_RELEASE): $(OBJS_RELEASE)
	$(CC) $(CFLAGS) -o $@ $^

ifeq ($(OS),Windows_NT)
	MKDIR = if not exist $(subst /,\,$(dir $@)) mkdir $(subst /,\,$(dir $@))
else
	MKDIR = mkdir -p $(dir $@)
endif

build/debug/%.o: src/%.c
	$(MKDIR)
	$(CC) $(CFLAGS) -c $< -o $@

build/debug/%.o: src/parse/%.c
	$(MKDIR)
	$(CC) $(CFLAGS) -c $< -o $@

build/release/%.o: src/%.c
	$(MKDIR)
	$(CC) $(CFLAGS) -c $< -o $@

build/release/%.o: src/parse/%.c
	$(MKDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
ifeq ($(OS),Windows_NT)
	del /F /Q build\debug\*.o build\release\*.o build\debug\*.exe build\release\*.exe
else
	rm -f build/debug/*.o build/release/*.o build/debug/*.exe build/release/*.exe
endif

.PHONY: all hc_debug hc_release clean
