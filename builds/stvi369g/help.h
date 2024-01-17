/* $Header: /nw/tony/src/stevie/src/RCS/help.c,v 1.9 89/08/06 09:50:09 tony Exp $
 *
 * Routine to display a command summary.
 * (Dave Tutelman note:
 *	I added the ability to page backwards and forwards through help.
 *	In order to minimize the abuse to the existing code, I used
 *	"goto"s and labeled each screen.  It's not the way I would have
 *	done help from scratch, but it's not TOO ugly.
 * )
 */

#ifndef HELP_H
#define HELP_H

#include "bool.h"

bool help(void);

#ifdef HELP
int helpkey(int n);
#endif

#endif
