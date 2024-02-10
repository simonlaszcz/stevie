/* $Header: /nw/tony/src/stevie/src/RCS/mark.c,v 1.3 89/03/11 22:42:39 tony Exp $
 *
 * Routines to save and retrieve marks.
 */

#ifndef MARK_H
#define MARK_H

#include "bool.h"
#include "stevie.h"

/**
 * setmark(c) - set mark 'c' at current cursor position
 *
 * Returns true on success, false if no room for mark or bad name given.
 */
bool setmark(register char c);

/**
 * setpcmark() - set the previous context mark to the current position
 */
void setpcmark(void);

/**
 * getmark(c) - find mark for char 'c'
 *
 * Return pointer to LPTR or NULL if no such mark.
 */
LPTR *getmark(register char c);

/**
 * clrall() - clear all marks
 *
 * Used mainly when trashing the entire buffer during ":e" type commands
 */
void clrall(void);

/**
 * clrmark(line) - clear any marks for 'line'
 *
 * Used any time a line is deleted so we don't have marks pointing to
 * non-existent lines.
 */
void clrmark(register EDLINE *line);

#endif
