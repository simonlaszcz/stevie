/* $Header: /nw/tony/src/stevie/src/RCS/misccmds.c,v 1.14 89/08/06 09:50:17 tony Exp $
 *
 * Various routines to perform specific editing operations or return
 * useful information about the file.
 */

#ifndef MISCCMDS_H
#define MISCCMDS_H

#include "bool.h"
#include "stevie.h"

/*
 * opencmd
 *
 * Add a blank line above or below the current line.
 */
void opencmd(int dir, bool can_ai);

int cntllines(register LPTR *pbegin, register LPTR *pend);

/*
 * plines(p) - return the number of physical screen lines taken by line 'p'
 */
int plines(LPTR *p);

void fileinfo(void);

/*
 * gotoline(n) - return a pointer to line 'n'
 *
 * Returns a pointer to the last line of the file if n is zero, or
 * beyond the end of the file.
 */
LPTR *gotoline(register int n);

void inschar(int c);
bool delchar(bool fixpos);
void delline(int nlines, bool can_update);

#endif
