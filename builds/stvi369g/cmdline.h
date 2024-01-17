/* $Header: /nw/tony/src/stevie/src/RCS/cmdline.c,v 1.20 89/08/13 11:41:23 tony Exp $
 * Routines to parse and execute "command line" commands, such as searches
 * or colon commands.
 */

#ifndef CMDLINE_H
#define CMDLINE_H

#include <stdarg.h>
#include "bool.h"

/**
 * getcmdln() - read a command line from the terminal
 *
 * Reads a command line started by typing '/', '?', '!', or ':'. Returns a
 * pointer to the string that was read. For searches, an optional trailing
 * '/' or '?' is removed.
 */
char *getcmdln(char firstc);

/**
 * docmdln() - handle a colon command
 *
 * Handles a colon command received interactively by getcmdln() or from
 * the environment variable "EXINIT" (or eventually .virc).
 */
void docmdln(char *cmdline);

void doxit(void);
void badcmd(void);
bool doecmd(char *arg, bool force);
void gotocmd(bool clr, char firstc);

/**
 * smsg(s) - displays the string 's' on the status line
 */
void smsg(char *s, ...);

/**
 * emsg() - display an error message
 *
 * Rings the bell, if appropriate, and calls message() to do the real work
 */
void emsg(char *s, ...);

void wait_return(void);

#endif
