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

SRC        = findport.c
OBJ        = findport.o
DEBUG_BIN  = findport

# Default: release object only (DEBUG=0)
all: release

# Release: compile object only, DEBUG=0
release: $(OBJ)

$(OBJ): $(SRC)
	$(CC) $(BASE_CFLAGS) -DDEBUG=0 -O2 -c $< -o $@

# Debug: recompile with DEBUG=1 and link binary
debug: $(SRC)
	$(CC) $(BASE_CFLAGS) -DDEBUG=1 -g -O0 -c $(SRC) -o $(OBJ)
	$(CC) $(BASE_CFLAGS) -DDEBUG=1 -g -O0 $(OBJ) $(LIBS) -o $(DEBUG_BIN)

clean:
	rm -f $(OBJ) $(DEBUG_BIN)

.PHONY: all release debug clean

