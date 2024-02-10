#ifndef MACHINE_H
#define MACHINE_H

#include <stdio.h>

#ifdef __PUREC__
#define STATBUF int
#else
#define STATBUF	long
#endif

/**
 * inchar() - get a character from the keyboard
 *
 * Certain special keys are mapped to values above 0x80. These
 * mappings are defined in keymap.h. If the key has a non-zero
 * ascii value, it is simply returned. Otherwise it may be a
 * special key we want to map.
 */
int inchar(void);
void outchar(char c);
void outstr(register char *s);
void flushbuf(void);
void beep(void);
void windinit(void);
void windexit(int r);
void windgoto(int r, int c);
void sleep(int n);
void pause(void);
FILE *fopenb(char *fname, char *mode);

/*
 * mktemp() - quick hack since there isn't one here
 */
char *mktemp(char *name);

void doshell(char *cmd);

/*
 * fixname(s) - fix up a dos name
 *
 * Takes a name like:
 *
 *	\x\y\z\base.ext
 *
 * and trims 'base' to 8 characters, and 'ext' to 3.
 */
char *fixname(char *s);

/**
 * Set the color to c, using the local system convention for numbering
 * colors or video attributes.
 *
 * If you implement this, remember to note the original color in
 * windinit(), before you do any setcolor() commands, and
 * do a setcolor() back to the original as part of windexit().
 */
void setcolor(int c);

/**
 * Set the number of lines to r, if possible.  Otherwise
 * "do the right thing".  Return the number of lines actually set.
 *
 * If you implement this, remember to note the original number of rows
 * in windinit(), before you do any setrows() commands, and
 * do a setrows() back to the original as part of windexit().
 */
int setrows(int r);

/**
 * Returns the inverse video attribute or color of co.
 * The existing code below is VERY simple-minded.
 * Replace it with proper code for your system.
 */
int reverse_color(int co);

/* 0 on success */
int fstat(const char *path, STATBUF *buf);
/* 0 on success */
int fchmod(const char *path, STATBUF *buf);
/* 0 on success */
long syscall(const char *cmd);

#endif
