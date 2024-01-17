/**
 * Routines to implement tags, and, especially, tag stacking.
 * Added by Dave Tutelman - 12/89.
 * The dotag() routine is a modification of the posted
 * version by Tony Andrews.
 * The untag() routine is new.
 */

#ifndef TAGCMD_H
#define TAGCMD_H

#include "bool.h"

/*
 * dotag(tag, force) - goto tag.  If force=true, dump pending changes.
 */
void dotag(char *tag, bool force);

/*
 * dountag (spec) - undo the last ':ta' command, popping the tag stack.
 *	spec is the appended character, giving specifics:
 *	  '!'	dump pending changes.
 *	  'e'	came from K_CCIRCM "shortcut".  do :e# if stack empty.
 *	  ' '	do normal untag.
 *	  else	bad command.
 */
void dountag(char spec);

#endif
