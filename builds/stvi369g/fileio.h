/* $Header: /nw/tony/src/stevie/src/RCS/fileio.c,v 1.12 89/08/06 09:50:01 tony Exp $
 *
 * Basic file I/O routines.
 */

#ifndef FILEIO_H
#define FILEIO_H

#include "bool.h"
#include "stevie.h"

void filemess(char *s);
void renum(void);

/**
 * Note that this will try to expand the file name using environment
 * variables.  For this reason, we copy it into an 80-byte buffer,
 * so that there's room to expand it.
 *
 * It uses the environment-variable convention of UNIX, even
 * under systems with other conventions.  That is, your home directory
 * would be called $HOME (even in DOS, where you might want to say %HOME%)
 */
bool readfile(char *fname, LPTR *fromp, bool nochangename);

/*
 * writeit - write to file 'fname' lines 'start' through 'end'
 *
 * If either 'start' or 'end' contain null line pointers, the default
 * is to use the start or end of the file respectively.
 */
bool writeit(char *fname, LPTR *start, LPTR *end);

#endif
