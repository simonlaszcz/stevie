/* $Header: /nw/tony/src/stevie/src/RCS/linefunc.c,v 1.2 89/03/11 22:42:32 tony Exp $
 *
 * Basic line-oriented motions.
 */

#ifndef LINEFUNC_H
#define LINEFUNC_H

#include "stevie.h"

/*
 * nextline(curr)
 *
 * Return a pointer to the beginning of the next line after the one
 * referenced by 'curr'. Return NULL if there is no next line (at EOF).
 */
LPTR *nextline(LPTR *curr);

/*
 * prevline(curr)
 *
 * Return a pointer to the beginning of the line before the one
 * referenced by 'curr'. Return NULL if there is no prior line.
 */
LPTR *prevline(LPTR *curr);

/*
 * coladvance(p,col)
 *
 * Try to advance to the specified column, starting at p.
 */
LPTR *coladvance(LPTR *p, register int col);

/*
 * nextchar(curr)
 *
 * Return a line pointer to the next character after the
 * one referenced by 'curr'. Return NULL if there is no next one (at EOF).
 * NOTE: this COULD point to a \n or \0 character.
 */
LPTR *nextchar(LPTR *curr);


/*
 * prevchar(curr)
 *
 * Return a line pointer to the previous character before the
 * one referenced by 'curr'. Return NULL if there is no previous one.
 * Note: this COULD point to a \n or \0 character.
 */
LPTR *prevchar(LPTR *curr);

#endif
