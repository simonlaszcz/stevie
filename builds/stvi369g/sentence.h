/*	Find the NEXT/PREVIOUS:
 *	- SENTENCE	findsent (dir)
 *	- PARAGRAPH	findpara (dir)
 *	- FUNCTION	findfunc (dir)
 *
 *	I've split these off from SEARCH.C, because they're alike and
 *	SEARCH.C is a big file already.  findfunc() was already there.
 *	I added findsent() and findpara().  -  Dave Tutelman
 */

#ifndef SENTENCE_H
#define SENTENCE_H

#include "bool.h"
#include "stevie.h"

int inclass (char c);
bool blankline(LPTR *line);
LPTR *skiptotext(LPTR *lp, int dir);

/*
 * findsent (dir) - Find the next sentence in direction 'dir'
 *
 * Return true if a sentence was found.
 *
 * Algorithm: found end of a sentence if:
 *   FWD - current char is BLANK | EOL and last is DOT.
 *   BKWD- current char is DOT and last is BLANK | EOL.
 * In either case, we then have to skip to text at beginning of next sentence.
 *
 */
bool findsent(int dir);

/*
 * findpara(dir) - Find the next paragraph in direction 'dir'
 *
 * Return true if a paragraph was found.
 *
 * Algorithm: found beginning of paragraph if:
 *   FWD - current line is non-blank and last is blank.
 *   BKWD- current line is blank and last is non-blank.
 * Then we skip to the first non-blank, non-dot text.
 *
 */
bool findpara(int dir);

/*
 * findfunc(dir) - Find the next function in direction 'dir'
 *
 * Return true if a function was found.
 *
 * Algorithm depends on a style of C coding in which the ONLY '{'
 * in the first column occurs at the beginning of a function definition.
 * This is a good and common style, but not syntactically required by C.
 */
bool findfunc(int dir);

#endif
