/***
 * Various allocation routines and routines returning information about
 * allocated objects.
 */

#ifndef ALLOC_H
#define ALLOC_H

#include <stddef.h>
#include <string.h>
#include "stevie.h"
#include "bool.h"

char *alloc(size_t size);
char *strsave(char *string);
int screenalloc(void);

/**
 * Allocate and initialize a new line structure with room for
 * 'nchars'+1 characters. We add one to nchars here to allow for
 * null termination because all the callers would just do it otherwise.
 */
EDLINE *newline(size_t nchars);

/**
 * filealloc() - construct an initial empty file buffer
 */
void filealloc(void);

/**
 * freeall() - free the current buffer
 *
 * Free all lines in the current buffer.
 */
void freeall(void);

/**
 * bufempty() - return TRUE if the buffer is empty
 */
bool bufempty(void);

/**
 * buf1line() - return TRUE if there is only one line
 */
bool buf1line(void);

/**
 * lineempty() - return TRUE if the current line is empty
 */
bool lineempty(void);

/**
 * endofline() - return TRUE if the given position is at end of line
 *
 * This routine will probably never be called with a position resting
 * on the NUL byte, but handle it correctly in case it happens.
 */
bool endofline(register LPTR * p);

/**
 * canincrease(n) - returns TRUE if the current line can be increased 'n' bytes
 *
 * This routine returns immediately if the requested space is available.
 * If not, it attempts to allocate the space and adjust the data structures
 * accordingly. If everything fails it returns FALSE.
 */
bool canincrease(register size_t n);

char *mkstr(char c);

#endif
