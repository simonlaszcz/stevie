/* $Header: /nw/tony/src/stevie/src/RCS/edit.c,v 1.11 89/08/02 19:57:12 tony Exp $
 *
 * The main edit loop as well as some other simple cursor movement routines.
 */

#ifndef EDIT_H
#define EDIT_H

#include "bool.h"

void edit(void);
void insertchar(int c);
void getout(void);
void scrolldown(int nlines);
void scrollup(int nlines);

/**
 * oneright
 * oneleft
 * onedown
 * oneup
 *
 * Move one char {right,left,down,up}.  Return true when
 * sucessful, false when we hit a boundary (of a line, or the file).
 */
bool oneright(void);
bool oneleft(void);
void beginline(bool flag);
bool oneup(int n);
bool onedown(int n);

#endif
