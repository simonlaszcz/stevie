/**
 * The routines in this file attempt to imitate many of the operations
 * that used to be performed on simple character pointers and are now
 * performed on LPTR's. This makes it easier to modify other sections
 * of the code. Think of an LPTR as representing a position in the file.
 * Positions can be incremented, decremented, compared, etc. through
 * the functions implemented here.
 */

#ifndef PTRFUNC_H
#define PTRFUNC_H

#include "bool.h"
#include "stevie.h"

/*
 * inc(p)
 *
 * Increment the line pointer 'p' crossing line boundaries as necessary.
 * Return 1 when crossing a line, -1 when at end of file, 0 otherwise.
 */
int inc(register LPTR *lp);

/*
 * dec(p)
 *
 * Decrement the line pointer 'p' crossing line boundaries as necessary.
 * Return 1 when crossing a line, -1 when at start of file, 0 otherwise.
 */
int dec(register LPTR *lp);

/*
 * gchar(lp) - get the character at position "lp"
 */
int gchar(register LPTR *lp);

/*
 * pchar(lp, c) - put character 'c' at position 'lp'
 */
void pchar(register LPTR *lp, char c);

/*
 * pswap(a, b) - swap two position pointers
 */
void pswap(register LPTR *a, register LPTR *b);

/*
 * Position comparisons
 */
bool lt(register LPTR *a, register LPTR *b);

bool equal(register LPTR *a, register LPTR *b);
bool ltoreq(register LPTR *a, register LPTR *b);

#endif
