/* $Header: /nw/tony/src/stevie/src/RCS/screen.c,v 1.8 89/08/02 09:26:33 tony Exp $
 *
 * Routines to manipulate the screen representations.
 */

#include <string.h>
#include "param.h"
#include "term.h"
#include "ascii.h"
#include "stevie.h"
#include "screen.h"
#include "linefunc.h"
#include "misccmds.h"
#include "ptrfunc.h"
#include "main.h"
#include "machine.h"
#include "alloc.h"
#include "edit.h"

/*
 * This gets set if we ignored an update request while input was pending.
 * We check this when the input is drained to see if the screen should be
 * updated.
 */
bool	need_redraw = false;

/*
 * The following variable is set (in filetonext) to the number of physical
 * lines taken by the line the cursor is on. We use this to avoid extra
 * calls to plines(). The optimized routines lfiletonext() and lnexttoscreen()
 * make sure that the size of the cursor line hasn't changed. If so, lines
 * below the cursor will move up or down and we need to call the routines
 * filetonext() and nexttoscreen() to examine the entire screen.
 */
static int Cline_size;	/* size (in rows) of the cursor line */
static int Cline_row;	/* starting row of the cursor line */
static char *mkline(register int n);
static void filetonext(void);
static void nexttoscreen(void);
static bool lfiletonext(void);
static void lnexttoscreen(void);

/*
 * filetonext()
 *
 * Based on the current value of Topchar, transfer a screenfull of
 * stuff from Filemem to Nextscreen, and update Botchar.
 */

static void
filetonext()
{
	register int	row, col;
	register char	*screenp = Nextscreen;
	LPTR	memp;
	LPTR	save;			/* save pos. in case line won't fit */
	register char	*endscreen;
	register char	*nextrow;
	char	extra[16];
	int	nextra = 0;
	register int	c;
	int	n;
	bool	done;		/* if true, we hit the end of the file */
	bool	didline;	/* if true, we finished the last line */
	int	srow;		/* starting row of the current line */
	int	lno = 0;		/* number of the line we're doing */
	int	coff;		/* column offset */

	coff = P(P_NU) ? 8 : 0;

	save = memp = *Topchar;

	if (P(P_NU))
		lno = cntllines(Filemem, Topchar);

	/*
	 * The number of rows shown is Rows-1.
	 * The last line is the status/command line.
	 */
	endscreen = &screenp[(Rows-1)*Columns];

	done = didline = false;
	srow = row = col = 0;
	/*
	 * We go one past the end of the screen so we can find out if the
	 * last line fit on the screen or not.
	 */
	while ( screenp <= endscreen && !done) {


		if (P(P_NU) && col == 0 && memp.index == 0) {
			strcpy(extra, mkline(lno++));
			nextra = 8;
		}

		/* Get the next character to put on the screen. */

		/* The 'extra' array contains the extra stuff that is */
		/* inserted to represent special characters (tabs, and */
		/* other non-printable stuff.  The order in the 'extra' */
		/* array is reversed. */

		if ( nextra > 0 )
			c = extra[--nextra];
		else {
			c = (unsigned)(0xff & gchar(&memp));
			if (inc(&memp) == -1)
				done = 1;
			/* when getting a character from the file, we */
			/* may have to turn it into something else on */
			/* the way to putting it into 'Nextscreen'. */
			if ( c == TAB && !P(P_LS) ) {
				strcpy(extra,"        ");
				/* tab amount depends on current column */
				nextra = ((P(P_TS)-1) - (col - coff)%P(P_TS));
				c = ' ';
			}
			else if ( c == NUL && P(P_LS) ) {
				extra[0] = NUL;
				nextra = 1;
				c = '$';
			} else if ( (n = chars[c].ch_size) > 1 ) {
				char *p;
				nextra = 0;
				p = chars[c].ch_str;
				/* copy 'ch-str'ing into 'extra' in reverse */
				while ( n > 1 )
					extra[nextra++] = p[--n];
				c = p[0];
			}
		}

		if (screenp == endscreen) {
			/*
			 * We're one past the end of the screen. If the
			 * current character is null, then we really did
			 * finish, so set didline = true. In either case,
			 * break out because we're done.
			 */
			dec(&memp);
			if (memp.index != 0 && c == NUL) {
				didline = true;
				inc(&memp);
			}
			break;
		}

		if ( c == NUL ) {
			srow = ++row;
			/*
			 * Save this position in case the next line won't
			 * fit on the screen completely.
			 */
			save = memp;
			/* get pointer to start of next row */
			nextrow = &Nextscreen[row*Columns];
			/* blank out the rest of this row */
			while ( screenp != nextrow )
				*screenp++ = ' ';
			col = 0;
			continue;
		}
		if ( col >= Columns ) {
			row++;
			col = 0;
		}
		/* store the character in Nextscreen */
		*screenp++ = c;
		col++;
	}
	/*
	 * If we didn't hit the end of the file, and we didn't finish
	 * the last line we were working on, then the line didn't fit.
	 */
	if (!done && !didline) {
		/*
		 * Clear the rest of the screen and mark the unused lines.
		 */
		screenp = &Nextscreen[srow * Columns];
		while (screenp < endscreen)
			*screenp++ = ' ';
		for (; srow < (Rows-1) ;srow++)
			Nextscreen[srow * Columns] = '@';
		*Botchar = save;
		return;
	}
	/* make sure the rest of the screen is blank */
	while ( screenp < endscreen )
		*screenp++ = ' ';
	/* put '~'s on rows that aren't part of the file. */
	if ( col != 0 )
		row++;
	while ( row < Rows ) {
		Nextscreen[row*Columns] = '~';
		row++;
	}
	if (done)	/* we hit the end of the file */
		*Botchar = *Fileend;
	else
		*Botchar = memp;	/* FIX - prev? */
}

/*
 * nexttoscreen
 *
 * Transfer the contents of Nextscreen to the screen, using Realscreen
 * to avoid unnecessary output.
 */
static void
nexttoscreen()
{
	register char	*np = Nextscreen;
	register char	*rp = Realscreen;
	register char	*endscreen;
	register int	row = 0, col = 0;
	int	gorow = -1, gocol = -1;

	if (anyinput()) {
		need_redraw = true;
		return;
	}

	endscreen = &np[(Rows-1)*Columns];

	CUROFF;		/* disable cursor */

	for ( ; np < endscreen ; np++,rp++ ) {
		/* If desired screen (contents of Nextscreen) does not */
		/* match what's really there, put it there. */
		if ( *np != *rp ) {
			/* if we are positioned at the right place, */
			/* we don't have to use windgoto(). */
			if (gocol != col || gorow != row) {
				/*
				 * If we're just off by one, don't send
				 * an entire esc. seq. (this happens a lot!)
				 */
				if (gorow == row && gocol+1 == col) {
					outchar(*(np-1));
					gocol++;
				} else
					windgoto(gorow=row,gocol=col);
			}
			outchar(*rp = *np);
			gocol++;
		}
		if ( ++col >= Columns ) {
			col = 0;
			row++;
		}
	}
	CURON;		/* enable cursor again */
}

/*
 * lfiletonext() - like filetonext() but only for cursor line
 *
 * Returns true if the size of the cursor line (in rows) hasn't changed.
 * This determines whether or not we need to call filetonext() to examine
 * the entire screen for changes.
 */
static bool
lfiletonext()
{
	register int	row = 0, col = 0;
	register char	*screenp = NULL;
	LPTR	memp;
	register char	*nextrow = NULL;
	char	extra[16];
	int	nextra = 0;
	register int	c = 0;
	int	n = 0;
	bool	eof = false;
	int	lno = 0;		/* number of the line we're doing */
	int	coff = 0;		/* column offset */

	coff = P(P_NU) ? 8 : 0;

	/*
	 * This should be done more efficiently.
	 */
	if (P(P_NU))
		lno = cntllines(Filemem, Curschar);

	screenp = Nextscreen + (Cline_row * Columns);

	memp = *Curschar;
	memp.index = 0;

	eof = false;
	col = 0;
	row = Cline_row;

	while (!eof) {

		if (P(P_NU) && col == 0 && memp.index == 0) {
			strcpy(extra, mkline(lno));
			nextra = 8;
		}

		/* Get the next character to put on the screen. */

		/* The 'extra' array contains the extra stuff that is */
		/* inserted to represent special characters (tabs, and */
		/* other non-printable stuff.  The order in the 'extra' */
		/* array is reversed. */

		if ( nextra > 0 )
			c = extra[--nextra];
		else {
			c = (unsigned)(0xff & gchar(&memp));
			if (inc(&memp) == -1)
				eof = true;
			/* when getting a character from the file, we */
			/* may have to turn it into something else on */
			/* the way to putting it into 'Nextscreen'. */
			if ( c == TAB && !P(P_LS) ) {
				strcpy(extra,"        ");
				/* tab amount depends on current column */
				nextra = ((P(P_TS)-1) - (col - coff)%P(P_TS));
				c = ' ';
			} else if ( c == NUL && P(P_LS) ) {
				extra[0] = NUL;
				nextra = 1;
				c = '$';
			} else if ( c != NUL && (n=chars[c].ch_size) > 1 ) {
				char *p;
				nextra = 0;
				p = chars[c].ch_str;
				/* copy 'ch-str'ing into 'extra' in reverse */
				while ( n > 1 )
					extra[nextra++] = p[--n];
				c = p[0];
			}
		}

		if ( c == NUL ) {
			row++;
			/* get pointer to start of next row */
			nextrow = &Nextscreen[row*Columns];
			/* blank out the rest of this row */
			while ( screenp != nextrow )
				*screenp++ = ' ';
			col = 0;
			break;
		}

		if ( col >= Columns ) {
			row++;
			col = 0;
		}
		/* store the character in Nextscreen */
		*screenp++ = c;
		col++;
	}
	return ((row - Cline_row) == Cline_size);
}

/*
 * lnexttoscreen
 *
 * Like nexttoscreen() but only for the cursor line.
 */
static void
lnexttoscreen()
{
	register char	*np = Nextscreen + (Cline_row * Columns);
	register char	*rp = Realscreen + (Cline_row * Columns);
	register char	*endline;
	register int	row, col;
	int	gorow = -1, gocol = -1;

	if (anyinput()) {
		need_redraw = true;
		return;
	}

	endline = np + (Cline_size * Columns);

	row = Cline_row;
	col = 0;

	CUROFF;		/* disable cursor */

	for ( ; np < endline ; np++,rp++ ) {
		/* If desired screen (contents of Nextscreen) does not */
		/* match what's really there, put it there. */
		if ( *np != *rp ) {
			/* if we are positioned at the right place, */
			/* we don't have to use windgoto(). */
			if (gocol != col || gorow != row) {
				/*
				 * If we're just off by one, don't send
				 * an entire esc. seq. (this happens a lot!)
				 */
				if (gorow == row && gocol+1 == col) {
					outchar(*(np-1));
					gocol++;
				} else
					windgoto(gorow=row,gocol=col);
			}
			outchar(*rp = *np);
			gocol++;
		}
		if ( ++col >= Columns ) {
			col = 0;
			row++;
		}
	}
	CURON;		/* enable cursor again */
}

static char *
mkline(register int n)
{
	static	char	lbuf[9];
	register int	i = 2;

	strcpy(lbuf, "        ");

	lbuf[i++] = (n % 10) + '0';
	n /= 10;
	if (n != 0) {
		lbuf[i++] = (n % 10) + '0';
		n /= 10;
	}
	if (n != 0) {
		lbuf[i++] = (n % 10) + '0';
		n /= 10;
	}
	if (n != 0) {
		lbuf[i++] = (n % 10) + '0';
		n /= 10;
	}
	if (n != 0) {
		lbuf[i++] = (n % 10) + '0';
		n /= 10;
	}
	return lbuf;
}

/*
 * updateline() - update the line the cursor is on
 *
 * Updateline() is called after changes that only affect the line that
 * the cursor is on. This improves performance tremendously for normal
 * insert mode operation. The only thing we have to watch for is when
 * the cursor line grows or shrinks around a row boundary. This means
 * we have to repaint other parts of the screen appropriately. If
 * lfiletonext() returns false, the size of the cursor line (in rows)
 * has changed and we have to call updatescreen() to do a complete job.
 */
void
updateline()
{
	if (!lfiletonext())
		updatescreen();	/* bag it, do the whole screen */
	else
		lnexttoscreen();
}

void
updatescreen()
{
	extern	bool	interactive;

	if (interactive) {
		filetonext();
		nexttoscreen();
	}
}

/*
 * prt_line() - print the given line
 */
void
prt_line(char *s)
{
	register int	si = 0;
	register int	c;
	register int	col = 0;

	char	extra[16];
	int	nextra = 0;
	int	n;

	for (;;) {

		if ( nextra > 0 )
			c = extra[--nextra];
		else {
			c = s[si++];
			if ( c == TAB && !P(P_LS) ) {
				strcpy(extra, "        ");
				/* tab amount depends on current column */
				nextra = (P(P_TS) - 1) - col%P(P_TS);
				c = ' ';
			} else if ( c == NUL && P(P_LS) ) {
				extra[0] = NUL;
				nextra = 1;
				c = '$';
			} else if ( c != NUL && (n=chars[c].ch_size) > 1 ) {
				char	*p;

				nextra = 0;
				p = chars[c].ch_str;
				/* copy 'ch-str'ing into 'extra' in reverse */
				while ( n > 1 )
					extra[nextra++] = p[--n];
				c = p[0];
			}
		}

		if ( c == NUL )
			break;

		outchar(c);
		col++;
	}
}

void
screenclear()
{
	register char	*rp, *np;
	register char	*end;

	CLS;		/* clear the display */

	rp  = Realscreen;
	end = Realscreen + Rows * Columns;
	np  = Nextscreen;

	/* blank out the stored screens */
	while (rp != end)
		*rp++ = *np++ = ' ';
}

void
cursupdate()
{
	register LPTR	*p = NULL;
	register int	icnt = 0, c = 0, nlines = 0;
	register int	i = 0;
	int	didinc = 0;

	if (bufempty()) {		/* special case - file is empty */
		*Topchar  = *Filemem;
		*Curschar = *Filemem;
	} else if ( LINEOF(Curschar) < LINEOF(Topchar) ) {
		nlines = cntllines(Curschar,Topchar);
		/* if the cursor is above the top of */
		/* the screen, put it at the top of the screen.. */
		*Topchar = *Curschar;
		Topchar->index = 0;
		/* ... and, if we weren't very close to begin with, */
		/* we scroll so that the line is close to the middle. */
		if ( nlines > Rows/3 ) {
			for (i=0, p = Topchar; i < Rows/3 ;i++, *Topchar = *p)
				if ((p = prevline(p)) == NULL)
					break;
		} else
			s_ins(0, nlines-1);
		updatescreen();
	}
	else if (LINEOF(Curschar) >= LINEOF(Botchar)) {
		nlines = cntllines(Botchar,Curschar);
		/* If the cursor is off the bottom of the screen, */
		/* put it at the top of the screen.. */
		/* ... and back up */
		if ( nlines > Rows/3 ) {
			p = Curschar;
			for (i=0; i < (2*Rows)/3 ;i++)
				if ((p = prevline(p)) == NULL)
					break;
			*Topchar = *p;
		} else {
			scrollup(nlines);
		}
		updatescreen();
	}

	Cursrow = Curscol = Cursvcol = 0;
	for ( p=Topchar; p->linep != Curschar->linep ;p = nextline(p) )
		Cursrow += plines(p);

	Cline_row = Cursrow;
	Cline_size = plines(p);

	if (P(P_NU))
		Curscol = 8;

	for (i=0; i <= Curschar->index ;i++) {
		c = Curschar->linep->s[i];
		/* A tab gets expanded, depending on the current column */
		if ( c == TAB && !P(P_LS) )
			icnt = P(P_TS) - (Cursvcol % P(P_TS));
		else
			icnt = chars[(unsigned)(c & 0xff)].ch_size;
		Curscol += icnt;
		Cursvcol += icnt;
		if ( Curscol >= Columns ) {
			Curscol -= Columns;
			Cursrow++;
			didinc = true;
		}
		else
			didinc = false;
	}
	if (didinc)
		Cursrow--;

	if (c == TAB && State == NORMAL && !P(P_LS)) {
		Curscol--;
		Cursvcol--;
	} else {
		Curscol -= icnt;
		Cursvcol -= icnt;
	}
	if (Curscol < 0)
		Curscol += Columns;

	if (set_want_col) {
		Curswant = Cursvcol;
		set_want_col = false;
	}
}

/*
 * The rest of the routines in this file perform screen manipulations.
 * The given operation is performed physically on the screen. The
 * corresponding change is also made to the internal screen image.
 * In this way, the editor anticipates the effect of editing changes
 * on the appearance of the screen. That way, when we call screenupdate
 * a complete redraw isn't usually necessary. Another advantage is that
 * we can keep adding code to anticipate screen changes, and in the
 * meantime, everything still works.
 */

/*
 * s_ins(row, nlines) - insert 'nlines' lines at 'row'
 */
void
s_ins(int row, int nlines)
{
	register char	*s, *d;		/* src & dest for block copy */
	register char	*e;		/* end point for copy */

	if ( ! CANIL )		/* can't do it */
		return;

	/*
	 * It "looks" better if we do all the inserts at once
	 */
	SAVCUR;			/* save position */
	windgoto(row, 0);

	CRTIL( row, nlines );

	windgoto(Rows-1, 0);	/* delete any garbage that may have */
	CLEOL;			/* been shifted to the bottom line */
	RESCUR;			/* restore the cursor position */

	/*
	 * Now do a block move to update the internal screen image
	 */
	d = Realscreen + (Columns * (Rows - 1)) - 1;
	s = d - (Columns * nlines);
	e = Realscreen + (Columns * row);

	while (s >= e)
		*d-- = *s--;

	/*
	 * Clear the inserted lines
	 */
	s = Realscreen + (row * Columns);
	e = s + (nlines * Columns);
	while (s < e)
		*s++ = ' ';
}

/*
 * s_del(row, nlines) - delete 'nlines' lines at 'row'
 */
void
s_del(int row, int nlines)
{
	register char	*s, *d, *e;

#ifndef BIOS
	if ( ! CANDL ) return;		/* can't do it */
#endif

	/* delete the lines */
	SAVCUR;				/* save position */

	windgoto (Rows-1, 0);		/* go to status line */
	CLEOL;				/* Clear it */
	windgoto (row, 0);		/* Go to 1st line-to-del */
	CRTDL( row, nlines );		/* Delete the lines */
	RESCUR;				/* Restore the cursor */

	/*
	 * do a block move to update the internal image
	 */
	d = Realscreen + (row * Columns);
	s = d + (nlines * Columns);
	e = Realscreen + ((Rows - 1) * Columns);

	while (s < e)
		*d++ = *s++;

	while (d < e)		/* clear the lines at the bottom */
		*d++ = ' ';
}
