#
# Makefile for the Atari ST - m68k gcc compiler
#

CC=m68k-atari-mint-gcc
AS=m68k-atari-mint-as -m68000
AR=m68k-atari-mint-ar

APP=stevie.ttp

CMINI_LIB=/opt/libcmini/lib
CMINI_INC=/opt/libcmini/include
CMINI_MINT_INC=/opt/libcmini/include/mint
REGEX_LIB=/opt/libregexp_spencer/lib
REGEX_INC=/opt/libregexp_spencer/include
POLYFILL_INC=./polyfill
# c89 to help pure-c compatibility
# TODO: add -Wextra and fix more errors
STANDARDS=-std=c89 \
	-O2 -pedantic -Werror -Wall \
	-Wno-long-long \
	-Wdeclaration-after-statement -Wmissing-prototypes -Wstrict-prototypes \
	-Wfloat-equal -Winit-self -Wshadow \
	-Wswitch-default -Wswitch-enum \
	-Wlogical-op \
	-Wformat=2 \
	-fno-common \
	-Wunused-macros
CFLAGS=$(STANDARDS) -s -DATARI -DTAGSTACK -DHELP -DTILDEOP

SRC = \
	alloc.c \
	cmdline.c \
	edit.c \
	enveval.c \
	fileio.c \
	help.c \
	hexchars.c \
	linefunc.c \
	main.c \
	mark.c \
	misccmds.c \
	normal.c \
	ops.c \
	param.c \
	ptrfunc.c \
	screen.c \
	search.c \
	sentence.c \
	tagcmd.c \
	tos.c \
	undo.c \
	version.c

# Using libcmini which produces much smaller binaries than the POSIX mintlib.
# We have to use a few polyfills but we want the code to compile with pure-c anyway.
mini: $(SRC)
	$(CC) -o $(APP) $(CFLAGS) \
	-DLIBCMINI_POLYFILL \
	-nostdinc -I$(REGEX_INC) -I$(CMINI_INC) -I$(CMINI_MINT_INC) -I$(POLYFILL_INC) \
	$(CMINI_LIB)/crt0.o $(SRC) \
	polyfill/strcspn.c \
	-nostdlib -L$(CMINI_LIB) -L$(REGEX_LIB) \
	-lcmini -lgcc -lregexp

# with mintlib
std: $(SRC)
	$(CC) -o $(APP) $(SRC) $(CFLAGS)

clean :
	$(RM) *.o
