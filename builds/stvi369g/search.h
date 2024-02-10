/**
 * This file contains various searching-related routines. These fall into
 * three groups: string searches (for /, ?, n, and N), character searches
 * within a single line (for f, F, t, T, etc), and "other" kinds of searches
 * like the '%' command, and 'word' searches.
 */

#ifndef SEARCH_H
#define SEARCH_H

#include "bool.h"
#include "stevie.h"

bool dosearch(int dir, char *str);
bool repsearch(int flag);

/**
 * regerror - called by regexp routines when errors are detected.
 */
void regerror(char const *s);

/**
 * dosub(lp, up, cmd)
 *
 * Perform a substitution from line 'lp' to line 'up' using the
 * command pointed to by 'cmd' which should be of the form:
 *
 * /pattern/substitution/g
 *
 * The trailing 'g' is optional and, if present, indicates that multiple
 * substitutions should be performed on each line, if applicable.
 * The usual escapes are supported as described in the regexp docs.
 */
void dosub(LPTR *lp, LPTR *up, char *cmd);

/**
 * doglob(cmd)
 *
 * Execute a global command of the form:
 *
 * g/pattern/X
 *
 * where 'x' is a command character, currently one of the following:
 *
 * d	Delete all matching lines
 * p	Print all matching lines
 *
 * The command character (as well as the trailing slash) is optional, and
 * is assumed to be 'p' if missing.
 */
void doglob(LPTR *lp, LPTR *up, char *cmd);

/**
 * searchc(c, dir, type)
 *
 * Search for character 'c', in direction 'dir'. If type is 0, move to
 * the position of the character, otherwise move to just before the char.
 */
bool searchc(char c, int dir, int type);

bool crepsearch(int flag);

/*
 * showmatch - move the cursor to the matching paren or brace
 */
LPTR *showmatch(void);

/**
 * fwd_word(pos, type) - move forward one word
 *
 * Returns the resulting position, or NULL if EOF was reached.
 */
LPTR *fwd_word(LPTR *p, int type);

/**
 * bck_word(pos, type) - move backward one word
 *
 * Returns the resulting position, or NULL if EOF was reached.
 */
LPTR *bck_word(LPTR *p, int type);

/**
 * end_word(pos, type, in_change) - move to the end of the word
 *
 * There is an apparent bug in the 'e' motion of the real vi. At least
 * on the System V Release 3 version for the 80386. Unlike 'b' and 'w',
 * the 'e' motion crosses blank lines. When the real vi crosses a blank
 * line in an 'e' motion, the cursor is placed on the FIRST character
 * of the next non-blank line. The 'E' command, however, works correctly.
 * Since this appears to be a bug, I have not duplicated it here.
 *
 * There's a strange special case here that the 'in_change' parameter
 * helps us deal with. Vi effectively turns 'cw' into 'ce'. If we're on
 * a word with only one character, we need to stick at the current
 * position so we don't change two words.
 *
 * Returns the resulting position, or NULL if EOF was reached.
 */
LPTR *end_word(LPTR *p, int type, bool in_change);

#endif
