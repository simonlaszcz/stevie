/* $Header: /nw/tony/src/stevie/src/RCS/edit.c,v 1.11 89/08/02 19:57:12 tony Exp $
 *
 * The main edit loop as well as some other simple cursor movement routines.
 */

#include <ctype.h>
#include "main.h"
#include "edit.h"
#include "misccmds.h"
#include "cmdline.h"
#include "ascii.h"
#include "linefunc.h"
#include "alloc.h"
#include "stevie.h"
#include "screen.h"
#include "normal.h"
#include "machine.h"
#include "ptrfunc.h"

/*
 * This flag is used to make auto-indent work right on lines where only
 * a <RETURN> or <ESC> is typed. It is set when an auto-indent is done,
 * and reset when any other editting is done on the line. If an <ESC>
 * or <RETURN> is received, and did_ai is true, the line is truncated.
 */
bool	did_ai = false;

void
edit()
{
	extern	bool	need_redraw;
	int	c;
	register char	*p, *q;

	Prenum = 0;

	/* position the display and the cursor at the top of the file. */
	*Topchar = *Filemem;
	*Curschar = *Filemem;
	Cursrow = Curscol = 0;

	do_mlines();		/* check for mode lines before starting */

	updatescreen();

	for ( ;; ) {

	/* Figure out where the cursor is based on Curschar. */
	cursupdate();

	if (need_redraw && !anyinput()) {
		updatescreen();
		need_redraw = false;
	}

	if (!anyinput())
		windgoto(Cursrow,Curscol);


	c = vgetc();

	if (State == NORMAL) {

		/* We're in the normal (non-insert) mode. */

		/* Pick up any leading digits and compute 'Prenum' */
		if ( (Prenum>0 && isdigit(c)) || (isdigit(c) && c!='0') ){
			Prenum = Prenum*10 + (c-'0');
			continue;
		}
		/* execute the command */
		normal(c);
		Prenum = 0;

	} else {

		/*
		 * Insert or Replace mode.
		 */
		switch (c) {

		case ESC:	/* an escape ends input mode */

			/*
			 * If we just did an auto-indent, truncate the
			 * line, and put the cursor back.
			 */
			if (did_ai) {
				Curschar->linep->s[0] = NUL;
				Curschar->index = 0;
				did_ai = false;
			}

			set_want_col = true;

			/* Don't end up on a '\n' if you can help it. */
			if (gchar(Curschar) == NUL && Curschar->index != 0)
				dec(Curschar);

			/*
			 * The cursor should end up on the last inserted
			 * character. This is an attempt to match the real
			 * 'vi', but it may not be quite right yet.
			 */
			if (Curschar->index != 0 && !endofline(Curschar))
				dec(Curschar);

			State = NORMAL;
			smsg("");

			/* construct the Redo buffer */
			p=Redobuff;
			q=Insbuff;
			while ( q < Insptr )
				*p++ = *q++;
			*p++ = ESC;
			*p = NUL;
			updatescreen();
			break;

		case CTRL('D'):
			/*
			 * Control-D is treated as a backspace in insert
			 * mode to make auto-indent easier. This isn't
			 * completely compatible with vi, but it's a lot
			 * easier than doing it exactly right, and the
			 * difference isn't very noticeable.
			 */
		case BS:
			/* can't backup past starting point */
			if (Curschar->linep == Insstart->linep &&
			    Curschar->index <= Insstart->index) {
				beep();
				break;
			}

			/* can't backup to a previous line */
			if (Curschar->linep != Insstart->linep &&
			    Curschar->index <= 0) {
				beep();
				break;
			}

			did_ai = false;
			dec(Curschar);
			if (State == INSERT)
				delchar(true);
			/*
			 * It's a little strange to put backspaces into
			 * the redo buffer, but it makes auto-indent a
			 * lot easier to deal with.
			 */
			*Insptr++ = BS;
			Ninsert++;
			cursupdate();
			updateline();
			break;

		case CR:
		case NL:
			if (State == REPLACE)		/* DMT added, 12/89 */
				delchar(false);
			*Insptr++ = NL;
			Ninsert++;
			opencmd(FORWARD, true);		/* open a new line */
			break;

		default:
			did_ai = false;
			insertchar(c);
			break;
		}
	}
	}
}

void
insertchar(int c)
{
	inschar(c);
	*Insptr++ = c;
	Ninsert++;
	/*
	 * The following kludge avoids overflowing the statically
	 * allocated insert buffer. Just dump the user back into
	 * command mode, and print a message.
	 */
	if (Insptr+10 >= &Insbuff[1024]) {
		stuffin(mkstr(ESC));
		emsg("No buffer space - returning to command mode");
		sleep(2);
	}
	updateline();
}

void
getout()
{
	windgoto(Rows-1,0);
	putchar('\r');
	putchar('\n');
	windexit(0);
}

void
scrolldown(int nlines)
{
	register LPTR	*p;
	register int	done = 0;	/* total # of physical lines done */

	/* Scroll up 'nlines' lines. */
	while (nlines--) {
		if ((p = prevline(Topchar)) == NULL)
			break;
		done += plines(p);
		*Topchar = *p;
		/*
		 * If the cursor is on the bottom line, we need to
		 * make sure it gets moved up the appropriate number
		 * of lines so it stays on the screen.
		 */
		if (Curschar->linep == Botchar->linep->prev) {
			int	i = 0;
			while (i < done) {
				i += plines(Curschar);
				*Curschar = *prevline(Curschar);
			}
		}
	}
	s_ins(0, done);
}

void
scrollup(int nlines)
{
	register LPTR	*p;
	register int	done = 0;	/* total # of physical lines done */
	register int	pl;		/* # of plines for the current line */

	/* Scroll down 'nlines' lines. */
	while (nlines--) {
		pl = plines(Topchar);
		if ((p = nextline(Topchar)) == NULL)
			break;
		done += pl;
		if (Curschar->linep == Topchar->linep)
			*Curschar = *p;
		*Topchar = *p;

	}
	s_del(0, done);
}

/*
 * oneright
 * oneleft
 * onedown
 * oneup
 *
 * Move one char {right,left,down,up}.  Return true when
 * sucessful, false when we hit a boundary (of a line, or the file).
 */

bool
oneright()
{
	set_want_col = true;

	switch (inc(Curschar)) {
	case 0:
		return true;
	case 1:
		dec(Curschar);		/* crossed a line, so back up */
		/* fall through */
	default:
		return false;
	}
}

bool
oneleft()
{
	set_want_col = true;

	switch (dec(Curschar)) {
	case 0:
		return true;
	case 1:
		inc(Curschar);		/* crossed a line, so back up */
		/* fall through */
	default:
		return false;
	}
}

void
beginline(bool flag)
{
	while ( oneleft() )
		;
	if (flag) {
		while (isspace(gchar(Curschar)) && oneright())
			;
	}
	set_want_col = true;
}

bool
oneup(int n)
{
	LPTR	p, *np;
	register int	k;

	p = *Curschar;
	for ( k=0; k<n; k++ ) {
		/* Look for the previous line */
		if ( (np=prevline(&p)) == NULL ) {
			/* If we've at least backed up a little .. */
			if ( k > 0 )
				break;	/* to update the cursor, etc. */
			else
				return false;
		}
		p = *np;
	}
	*Curschar = p;
	/* This makes sure Topchar gets updated so the complete line */
	/* is one the screen. */
	cursupdate();
	/* try to advance to the column we want to be at */
	*Curschar = *coladvance(&p, Curswant);
	return true;
}

bool
onedown(int n)
{
	LPTR	p, *np;
	register int	k;

	p = *Curschar;
	for ( k=0; k<n; k++ ) {
		/* Look for the next line */
		if ( (np=nextline(&p)) == NULL ) {
			if ( k > 0 )
				break;
			else
				return false;
		}
		p = *np;
	}
	/* try to advance to the column we want to be at */
	*Curschar = *coladvance(&p, Curswant);
	return true;
}
