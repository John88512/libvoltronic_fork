# Makefile for findport.c
#
# Usage:
#   make release   → DEBUG=0, build findport.o only
#   make           → same as 'make release'
#   make debug     → DEBUG=1, build findport.o and link 'findport_debug'
#   make clean     → remove all objects and binaries

CC         = gcc
PKG_CONFIG = pkg-config

BASE_CFLAGS = -Wall $(shell $(PKG_CONFIG) --cflags libserialport)
LIBS        = $(shell $(PKG_CONFIG) --libs libserialport)

SRC        = test_findport.c findport.c
OBJ        = test_findport.o findport.o
DEBUG_BIN  = test_findport

# Default: release object only (DEBUG=0)
all: release

# Release: compile object only, DEBUG=0
#release: $(OBJ)
release : findport.o

#$(OBJ): $(SRC)
	$(CC) $(BASE_CFLAGS) -DDEBUG=0 -O2 -c findport.c -o findport.o

# Debug: recompile with DEBUG=1 and link binary
debug: test_findport.c findport.c
	$(CC) $(BASE_CFLAGS) -DDEBUG=1 -g -O0 -c findport.c -o findport.o
	$(CC) $(BASE_CFLAGS) -DDEBUG=1 -g -O0 -c test_findport.c -o test_findport.o
	$(CC) $(BASE_CFLAGS) -DDEBUG=1 -g -O0 $(OBJ) $(LIBS) -o $(DEBUG_BIN)

clean:
	rm -f $(OBJ) $(DEBUG_BIN)

.PHONY: all release debug clean

