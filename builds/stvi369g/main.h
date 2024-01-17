/* $Header: /nw/tony/src/stevie/src/RCS/main.c,v 1.12 89/08/02 19:53:27 tony Exp $
 *
 * The main routine and routines to deal with the input buffer.
 */

#ifndef MAIN_H
#define MAIN_H

#include "bool.h"

void stuffin(char *s);
void stuffnum(int n);
int vgetc(void);

/**
 * anyinput
 *
 * Return non-zero if input is pending.
 */
bool anyinput(void);

/*
 * do_mlines() - process mode lines for the current file
 *
 * Returns immediately if the "ml" parameter isn't set.
 */
void do_mlines(void);

#endif
