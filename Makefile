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
AFLAGS=-felf64 -F dwarf
CFLAGS=-pedantic -O2 -g -fopenmp -fPIC -Wall
LIBS=-L$(CURDIR)
INCS=-I$(CURDIR) 

# QUIET MAKE
QUIET_AS         = @echo '   ' AS '   ' $@;
QUIET_CC         = @echo '   ' CC '   ' $@;
QUIET_LINK       = @echo '   ' LINK ' ' $@;
QUIET_AR         = @echo '   ' AR '   ' $@;
QUIET_SO         = @echo '   ' SO '   ' $@;

# QUIET DIST
QUIET_DISTBIN    = @echo '   ' DIST BINARY;
QUIET_DISTSRC    = @echo '   ' ARCH SOURCE;

# Assembly

ASFILES = $(wildcard arch/x86-64/*.asm)
ASOBJS  = $(patsubst %.asm, build/%.o, $(ASFILES))

# FILES
SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
OBJS = $(patsubst %.c, build/%.o, $(SOURCES))

TESTS = build/test/t-nn

BINARIES = $(TESTS)

# PHONY SPEC
.PHONY: all dist

# RULES
all: $(ASOBJS) $(OBJS) $(BINARIES)

clean:
	rm -f $(ASOBJS) $(OBJS) $(BINARIES)

check: $(OBJS) $(BINARIES)
	$(foreach prog, $(TESTS), $(prog);)	

strip:
	strip $(BINARIES)

build/arch/x86-64/%.o: arch/x86-64/%.asm
	$(QUIET_AS)$(AS)  $(AFLAGS) $< -o $@

build/%.o: %.c
	$(QUIET_CC)$(CC)  $(CFLAGS) $(INCS) -c $< -o $@

build/test/t-nn: build/nn.o build/t-nn.o build/arch/x86-64/nn_add_mc.o build/arch/x86-64/nn_sub_mc.o
	$(QUIET_LINK)$(CC) -o build/test/t-nn build/nn.o build/t-nn.o build/arch/x86-64/nn_add_mc.o  build/arch/x86-64/nn_sub_mc.o

	

