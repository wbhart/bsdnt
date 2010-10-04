# Copyright (c):
# * Antony Vennard 2010. All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#  
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in
#     the documentation and/or other materials provided with the
#     distribution.
#  
# THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.


# BSDNT UNIX Build System.

# Build tools and environment
AS=nasm
AR=ar
CC=gcc
CFLAGS=-pedantic -O2 -masm=intel -g -fopenmp -fPIC -Wall
LIBS=-L$(CURDIR)
INCS=-I$(CURDIR)

# QUIET MAKE
QUIET_CC         = @echo '   ' CC '   ' $@;
QUIET_LINK       = @echo '   ' LINK ' ' $@;
QUIET_AR         = @echo '   ' AR '   ' $@;
QUIET_SO         = @echo '   ' SO '   ' $@;

# QUIET DIST
QUIET_DISTBIN    = @echo '   ' DIST BINARY;
QUIET_DISTSRC    = @echo '   ' ARCH SOURCE;

# FILES
SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
OBJS = $(patsubst %.c, build/%.o, $(SOURCES))

TESTS = $(wildcard test/*.c)
TEST_OBJS = $(patsubst %.c, build/%.o, $(TESTS))
BINARIES = $(patsubst %.c, build/%, $(TESTS))

# PHONY SPEC
.PHONY: all dist

# RULES
all: $(OBJS) $(TEST_OBJS)
	$(QUIET_LINK)$(foreach prog, $(BINARIES), $(CC) $(OBJS) $(prog).o -o $(prog);)
	
clean:
	rm -f $(OBJS) $(TEST_OBJS) $(BINARIES)
	rm -f cpuid

distclean: clean
	rm -f *_arch.h

check: all 
	$(foreach prog, $(BINARIES), $(prog);)	

strip:
	strip $(BINARIES)

build/%.o: %.c
	$(QUIET_CC)$(CC)  $(CFLAGS) $(INCS) -c $< -o $@

build/test/%.o: %.c
	$(QUIET_CC)$(CC)  $(CFLAGS) $(INCS) -c test/$< -o $@

