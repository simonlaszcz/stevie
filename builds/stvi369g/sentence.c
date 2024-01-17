/*	Find the NEXT/PREVIOUS:
 *	- SENTENCE	findsent (dir)
 *	- PARAGRAPH	findpara (dir)
 *	- FUNCTION	findfunc (dir)
 *
 *	I've split these off from SEARCH.C, because they're alike and
 *	SEARCH.C is a big file already.  findfunc() was already there.
 *	I added findsent() and findpara().  -  Dave Tutelman
 */

#include "stevie.h"
#include "ops.h"
#include "linefunc.h"
#include "sentence.h"
#include "mark.h"

/* We'll be doing some classification of input characters, into: */
#define	BLANK	0	/* Whitespace */
#define	DOT	1	/* Period, exclamation, q-mark */
#define	EOL	2	/* End-of-line */
#define	OTHER	3	/* Any other non-blank stuff */

extern	int	operator;	/* From normal.c, is there an operator
				 * pending?
				 */

int
inclass (char c)
{
	switch (c) {
	  case ' ':
	  case '\t':
		return BLANK;
	  case '.':
	  case '!':
	  case '?':
		return DOT;
	  case '\n':
	  case '\r':
	  case '\0':
		return EOL;
	  default:
		if (c<' ' || c>'~')	return EOL;
		else			return OTHER;
	}
}

/* We'll also need to (1) tell if a line is just blanks, and
 *                    (2) skip to the next OTHER character.
 * Here are a couple of functions to do it.
 */

bool
blankline(LPTR *line)
{
	char	*p;
	int	class;

	if (! line)	return true;
	for (p = line->linep->s; (class=inclass(*p))!=EOL; p++)
		if (class!=BLANK)	return false;
	return true;
}

LPTR *
skiptotext(LPTR *lp, int dir)
{
	LPTR *lpp;

	lpp = lp;
	while (inclass( CHAR( lpp )) != OTHER) {
		lpp = (dir==FORWARD) ? nextchar (lpp) : prevchar (lpp);
		if (!lpp) return (lp);		/* hit the end */
	}
	return (lpp);
}

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
bool
findsent(int dir)
{
	LPTR	*curr, *last;	/* LPTR for current & last characters */
	int	ccurr, clast;	/* class of curr and last characters */
	size_t	oldindex;	/* need to keep in case search fails */

	curr  = Curschar;
	oldindex = curr->index;
	/* Get INTO most recent sentence sentence. */
	if (dir==BACKWARD)
		curr = prevchar (curr);
	curr = skiptotext (curr, BACKWARD);
	ccurr = OTHER;


	do {
		/* Take a step */
		last = curr; clast = ccurr;

		curr = (dir == FORWARD) ? nextchar(curr) : prevchar(curr);
		ccurr = inclass (CHAR( curr ));

		/* Test halting condition */
		if (dir==FORWARD &&
		    (ccurr==BLANK || ccurr==EOL) && clast==DOT) {
			setpcmark();
			last = skiptotext (last, FORWARD);
			*Curschar = *last;
			return true;
		}
		else if (dir==BACKWARD &&
		     ccurr==DOT && (clast==BLANK || clast==EOL)) {
			setpcmark();
			last = skiptotext (last, FORWARD);
			*Curschar = *last;
			return true;
		}
	} while (curr != NULL);

	Curschar->index = oldindex;	/* restore if search failed */
	return false;
}


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
bool
findpara(int dir)
{
	LPTR	*curr, *last;	/* current & last lines */
	bool	bcurr, blast;	/* "blankness" value for lines */

	curr  = Curschar;
	bcurr = (dir==FORWARD) ? false : true;	/* keeps us from passing the
						 * text initially. */

	do {
		/* Take a step */
		last = curr; blast = bcurr;
		curr = (dir == FORWARD) ? nextline(curr) : prevline(curr);
		bcurr = blankline (curr);

		/* Test halting condition */
		if (dir==FORWARD && bcurr && !blast) {
			setpcmark();
			curr = skiptotext (curr, FORWARD);
			*Curschar = *curr;
			return true;
		}
		else if (dir==BACKWARD && bcurr && !blast) {
			setpcmark();
			last = skiptotext (last, FORWARD);
			*Curschar = *last;
			return true;
		}
	} while (curr != NULL);

	return false;
}


/*
 * findfunc(dir) - Find the next function in direction 'dir'
 *
 * Return true if a function was found.
 *
 * Algorithm depends on a style of C coding in which the ONLY '{'
 * in the first column occurs at the beginning of a function definition.
 * This is a good and common style, but not syntactically required by C.
 */
bool
findfunc(int dir)
{
	LPTR	*curr;

	curr = Curschar;

	do {
		curr = (dir == FORWARD) ? nextline(curr) : prevline(curr);

		if (curr != NULL && curr->linep->s[0] == '{') {
			setpcmark();
			*Curschar = *curr;
			return true;
		}
	} while (curr != NULL);

	return false;
}

