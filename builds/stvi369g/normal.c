/* $Header: /nw/tony/src/stevie/src/RCS/normal.c,v 1.25 89/08/06 09:50:25 tony Exp $
 *
 * Contains the main routine for processing characters in command mode.
 * Communicates closely with the code in ops.c to handle the operators.
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "stevie.h"
#include "ops.h"
#include "param.h"
#include "term.h"
#include "keymap.h"
#include "ascii.h"
#include "linefunc.h"
#include "misccmds.h"
#include "cmdline.h"
#include "mark.h"
#include "search.h"
#include "sentence.h"
#include "alloc.h"
#include "normal.h"
#include "undo.h"
#include "edit.h"
#include "screen.h"
#include "main.h"
#include "machine.h"
#include "help.h"
#include "ptrfunc.h"

/*
 * Generally speaking, every command in normal() should either clear any
 * pending operator (with CLEAROP), or set the motion type variable.
 */

#define	CLEAROP	(operator=NOP)	/* clear any pending operator */

int	operator = NOP;		/* current pending operator */
int	mtype;			/* type of the current cursor motion */
bool	mincl;			/* true if char motion is inclusive */
LPTR	startop;		/* cursor pos. at start of operator */

/*
 * Operators can have counts either before the operator, or between the
 * operator and the following cursor motion as in:
 *
 *	d3w or 3dw
 *
 * If a count is given before the operator, it is saved in opnum. If
 * normal() is called with a pending operator, the count in opnum (if
 * present) overrides any count that came later.
 */
static	int	opnum = 0;

#define	DEFAULT1(x)	(((x) == 0) ? 1 : (x))

/*
 * normal(c)
 *
 * Execute a command in command mode.
 *
 * This is basically a big switch with the cases arranged in rough categories
 * in the following order:
 *
 *	1. File positioning commands
 *	2. Control commands (e.g. ^G, Z, screen redraw, etc)
 *	3. Character motions
 *	4. Search commands (of various kinds)
 *	5. Edit commands (e.g. J, x, X)
 *	6. Insert commands (e.g. i, o, O, A)
 *	7. Operators
 *	8. Abbreviations (e.g. D, C)
 *	9. Marks
 */
void
normal(register int c)
{
	register int	n;
	register char	*s;	/* temporary variable for misc. strings */
	bool	flag = false;
	int	type = 0;	/* used in some operations to modify type */
	int	dir = FORWARD;	/* search direction */
	int	nchar = NUL;
	bool	finish_op;

	/*
	 * If there is an operator pending, then the command we take
	 * this time will terminate it. Finish_op tells us to finish
	 * the operation before returning this time (unless the operation
	 * was cancelled.
	 */
	finish_op = (operator != NOP);

	/*
	 * If we're in the middle of an operator AND we had a count before
	 * the operator, then that count overrides the current value of
	 * Prenum. What this means effectively, is that commands like
	 * "3dw" get turned into "d3w" which makes things fall into place
	 * pretty neatly.
	 */
	if (finish_op) {
		if (opnum != 0)
			Prenum = opnum;
	} else
		opnum = 0;

	u_lcheck();	/* clear the "line undo" buffer if we've moved */

	switch (c & 0xff) {

	/*
	 * Screen positioning commands
	 */
	case CTRL('D'):
		CLEAROP;
		if (Prenum)
			P(P_SS) = (Prenum > Rows-1) ? Rows-1 : Prenum;
		scrollup(P(P_SS));
		onedown(P(P_SS));
		updatescreen();
		break;

	case CTRL('U'):
		CLEAROP;
		if (Prenum)
			P(P_SS) = (Prenum > Rows-1) ? Rows-1 : Prenum;
		scrolldown(P(P_SS));
		oneup(P(P_SS));
		updatescreen();
		break;

	/*
	 * This is kind of a hack. If we're moving by one page, the calls
	 * to stuffin() do exactly the right thing in terms of leaving
	 * some context, and so on. If a count was given, we don't have
	 * to worry about these issues.
	 */
	case CTRL('F'):
		CLEAROP;
		n = DEFAULT1(Prenum);
		if (n > 1) {
			if ( ! onedown(Rows * n) )
				beep();
			cursupdate();
		} else {
			/* screenclear(); */
			stuffin("Lz\nM");
		}
		break;

	case CTRL('B'):
		CLEAROP;
		n = DEFAULT1(Prenum);
		if (n > 1) {
			if ( ! oneup(Rows * n) )
				beep();
			cursupdate();
		} else {
			/* screenclear(); */
			stuffin("Hz-M");
		}
		break;

	case CTRL('E'):
		CLEAROP;
		scrollup(DEFAULT1(Prenum));
		updatescreen();
		break;

	case CTRL('Y'):
		CLEAROP;
		scrolldown(DEFAULT1(Prenum));
		updatescreen();
		break;

	case 'z':
		CLEAROP;
		switch (vgetc()) {
		case NL:		/* put Curschar at top of screen */
		case CR:
			*Topchar = *Curschar;
			Topchar->index = 0;
			updatescreen();
			break;

		case '.':		/* put Curschar in middle of screen */
			n = Rows/2;
			goto dozcmd;

		case '-':		/* put Curschar at bottom of screen */
			n = Rows-1;
			/* fall through */

		dozcmd:
			{
				register LPTR	*lp = Curschar;
				register int	l = 0;

				while ((l < n) && (lp != NULL)) {
					l += plines(lp);
					*Topchar = *lp;
					lp = prevline(lp);
				}
			}
			Topchar->index = 0;
			updatescreen();
			break;

		default:
			beep();
		}
		break;

	/*
	 * Control commands
	 */
	case ':':
		CLEAROP;
		if ((s = getcmdln(c)) != NULL)
			docmdln(s);
		break;

	case KM_HELP:
		CLEAROP;
		if (help()) {
			screenclear();
			updatescreen();
		}
		break;

	case CTRL('L'):
		CLEAROP;
		screenclear();
		updatescreen();
		break;


	case CTRL('O'):			/* ignored */
		/*
		 * A command that's ignored can be useful. We use it at
		 * times when we want to postpone redraws. By stuffing
		 * in a control-o, redraws get suspended until the editor
		 * gets back around to processing input.
		 */
		break;

	case CTRL('G'):
		CLEAROP;
		fileinfo();
		break;

	case KM_CCIRCM:			/* shorthand command */
		CLEAROP;
#ifdef TAGSTACK
		/* If tag stacking compiled in & enabled, this is an untag.
		 * Otherwise, or if tag stack empty, edit alternate file.
		 * "untage" is so interpreted by dountag().
		 */
		if (P(P_TG))
			stuffin(":untage\n");
		else
#endif
			stuffin(":e #\n");
		break;

	case 'Z':			/* write, if changed, and exit */
		if (vgetc() != 'Z') {
			beep();
			break;
		}
		doxit();
		break;

	/*
	 * Macro evaluates true if char 'c' is a valid identifier character
	 */
#	define	IDCHAR(c)	(isalpha(c) || isdigit(c) || (c) == '_')

	case CTRL(']'):			/* :ta to current identifier */
		CLEAROP;
		{
			char	ch;
			LPTR	save;

			save = *Curschar;
			/*
			 * First back up to start of identifier. This
			 * doesn't match the real vi but I like it a
			 * little better and it shouldn't bother anyone.
			 */
			ch = gchar(Curschar);
			while (IDCHAR(ch)) {
				if (!oneleft())
					break;
				ch = gchar(Curschar);
			}
			if (!IDCHAR(ch))
				oneright();

			stuffin(":ta ");
			/*
			 * Now grab the chars in the identifier
			 */
			ch = gchar(Curschar);
			while (IDCHAR(ch)) {
				stuffin(mkstr(ch));
				if (!oneright())
					break;
				ch = gchar(Curschar);
			}
			stuffin("\n");

			*Curschar = save;	/* restore, in case of error */
		}
		break;

	/*
	 * Character motion commands
	 */
	case 'G':
		mtype = MLINE;
		*Curschar = *gotoline(Prenum);
		beginline(true);
		break;

	case 'H':
		mtype = MLINE;
		*Curschar = *Topchar;
		for (n = Prenum; n && onedown(1) ;n--)
			;
		beginline(true);
		break;

	case 'M':
		mtype = MLINE;
		*Curschar = *Topchar;
		for (n = 0; n < Rows/2 && onedown(1) ;n++)
			;
		beginline(true);
		break;

	case 'L':
		mtype = MLINE;
		*Curschar = *prevline(Botchar);
		for (n = Prenum; n && oneup(1) ;n--)
			;
		beginline(true);
		break;

	case 'l':
	case KM_RARROW:
	case ' ':
		mtype = MCHAR;
		mincl = false;
		n = DEFAULT1(Prenum);
		while (n--) {
			if ( ! oneright() )
				beep();
		}
		set_want_col = true;
		break;

	case 'h':
	case KM_LARROW:
	case CTRL('H'):
		mtype = MCHAR;
		mincl = false;
		n = DEFAULT1(Prenum);
		while (n--) {
			if ( ! oneleft() )
				beep();
		}
		set_want_col = true;
		break;

	case '-':
		flag = true;
		/* fall through */

	case 'k':
	case KM_UARROW:
	case CTRL('P'):
		mtype = MLINE;
		if ( ! oneup(DEFAULT1(Prenum)) )
			beep();
		if (flag)
			beginline(true);
		break;

	case '+':
	case CR:
	case NL:
		flag = true;
		/* fall through */

	case 'j':
	case KM_DARROW:
	case CTRL('N'):
		mtype = MLINE;
		if ( ! onedown(DEFAULT1(Prenum)) )
			beep();
		if (flag)
			beginline(true);
		break;

	/*
	 * This is a strange motion command that helps make operators
	 * more logical. It is actually implemented, but not documented
	 * in the real 'vi'. This motion command actually refers to "the
	 * current line". Commands like "dd" and "yy" are really an alternate
	 * form of "d_" and "y_". It does accept a count, so "d3_" works to
	 * delete 3 lines.
	 */
	case '_':
	lineop:
		mtype = MLINE;
		onedown(DEFAULT1(Prenum)-1);
		break;

	case '|':
		mtype = MCHAR;
		mincl = true;
		beginline(false);
		if (Prenum > 0)
			*Curschar = *coladvance(Curschar, Prenum-1);
		Curswant = Prenum - 1;
		break;
		
	/*
	 * Word Motions
	 */

	case 'B':
		type = 1;
		/* fall through */

	case 'b':
		mtype = MCHAR;
		mincl = false;
		set_want_col = true;
		for (n = DEFAULT1(Prenum); n > 0 ;n--) {
			LPTR	*pos;

			if ((pos = bck_word(Curschar, type)) == NULL) {
				beep();
				CLEAROP;
				break;
			} else
				*Curschar = *pos;
		}
		break;

	case 'W':
		type = 1;
		/* fall through */

	case 'w':
		/*
		 * This is a little strange. To match what the real vi
		 * does, we effectively map 'cw' to 'ce', and 'cW' to 'cE'.
		 * This seems impolite at first, but it's really more
		 * what we mean when we say 'cw'.
		 */
		if (operator == CHANGE)
			goto doecmd;

		mtype = MCHAR;
		mincl = false;
		set_want_col = true;
		for (n = DEFAULT1(Prenum); n > 0 ;n--) {
			LPTR	*pos;

			if ((pos = fwd_word(Curschar, type)) == NULL) {
				beep();
				CLEAROP;
				break;
			} else
				*Curschar = *pos;
		}
		break;

	case 'E':
		type = 1;
		/* fall through */

	case 'e':
	doecmd:
		mtype = MCHAR;
		mincl = true;
		set_want_col = true;
		for (n = DEFAULT1(Prenum); n > 0 ;n--) {
			LPTR	*pos;

			/*
			 * The first motion gets special treatment if we're
			 * do a 'CHANGE'.
			 */
			if (n == DEFAULT1(Prenum))
				pos = end_word(Curschar,type,operator==CHANGE);
			else
				pos = end_word(Curschar, type, false);

			if (pos == NULL) {
				beep();
				CLEAROP;
				break;
			} else
				*Curschar = *pos;
		}
		break;

	case '$':
		mtype = MCHAR;
		mincl = true;
		while ( oneright() )
			;
		Curswant = 999;		/* so we stay at the end */
		break;

	case '^':
		mtype = MCHAR;
		mincl = false;
		beginline(true);
		break;

	case '0':
		mtype = MCHAR;
		mincl = true;
		beginline(false);
		break;

	/*
	 * Searches of various kinds
	 */
	case '?':
	case '/':
		s = getcmdln(c);	/* get the search string */

		/*
		 * If they backspaced out of the search command,
		 * just bag everything.
		 */
		if (s == NULL) {
			CLEAROP;
			break;
		}

		mtype = MCHAR;
		mincl = false;
		set_want_col = true;

		/*
		 * If no string given, pass NULL to repeat the prior search.
		 * If the search fails, abort any pending operator.
		 */
		if (!dosearch(
				(c == '/') ? FORWARD : BACKWARD,
				(*s == NUL) ? NULL : s
			     ))
			CLEAROP;
		break;

	case 'n':
		mtype = MCHAR;
		mincl = false;
		set_want_col = true;
		if (!repsearch(0))
			CLEAROP;
		break;

	case 'N':
		mtype = MCHAR;
		mincl = false;
		set_want_col = true;
		if (!repsearch(1))
			CLEAROP;
		break;

	/*
	 * Character searches
	 */
	case 'T':
		dir = BACKWARD;
		/* fall through */

	case 't':
		type = 1;
		goto docsearch;

	case 'F':
		dir = BACKWARD;
		/* fall through */

	case 'f':
	docsearch:
		mtype = MCHAR;
		mincl = true;
		set_want_col = true;
		if ((nchar = vgetc()) == ESC)	/* search char */
			break;

		for (n = DEFAULT1(Prenum); n > 0 ;n--) {
			if (!searchc(nchar, dir, type)) {
				CLEAROP;
				beep();
			}
		}
		break;

	case ',':
		flag = 1;
		/* fall through */

	case ';':
		mtype = MCHAR;
		mincl = true;
		set_want_col = true;
		for (n = DEFAULT1(Prenum); n > 0 ;n--) {
			if (!crepsearch(flag)) {
				CLEAROP;
				beep();
			}
		}
		break;

	case '(':			/* sentence searches */
		dir = BACKWARD;
		/* fall through */

	case ')':
		mtype = MCHAR;
		mincl = false;
		set_want_col = true;
		if (!findsent(dir)) {
			beep();
			CLEAROP;
		}
		break;

	case '{':			/* paragraph searches */
		dir = BACKWARD;
		/* fall through */

	case '}':
		mtype = MCHAR;
		mincl = false;
		set_want_col = true;
		if (!findpara(dir)) {
			beep();
			CLEAROP;
		}
		break;

	case '[':			/* function searches */
		dir = BACKWARD;
		/* fall through */

	case ']':
		mtype = MLINE;
		set_want_col = true;
		if (vgetc() != c) {
			beep();
			CLEAROP;
			break;
		}

		if (!findfunc(dir)) {
			beep();
			CLEAROP;
		}
		break;

	case '%':
		mtype = MCHAR;
		mincl = true;
		{
			LPTR	*pos;

			if ((pos = showmatch()) == NULL) {
				beep();
				CLEAROP;
			} else {
				setpcmark();
				*Curschar = *pos;
				set_want_col = true;
			}
		}
		break;
		
	/*
	 * Edits
	 */
	case '.':		/* repeat last change (usually) */
		/*
		 * If a delete is in effect, we let '.' help out the same
		 * way that '_' helps for some line operations. It's like
		 * an 'l', but subtracts one from the count and is inclusive.
		 */
		if (operator == DELETE || operator == CHANGE) {
			if (Prenum != 0) {
				n = DEFAULT1(Prenum) - 1;
				while (n--)
					if (! oneright())
						break;
			}
			mtype = MCHAR;
			mincl = true;
		} else {			/* a normal 'redo' */
			CLEAROP;
			stuffin(Redobuff);
		}
		break;

	case 'u':
	case KM_UNDO:
		CLEAROP;
		u_undo();
		break;

	case 'U':
		CLEAROP;
		u_lundo();
		break;

	case 'x':
		CLEAROP;
		if (lineempty())	/* can't do it on a blank line */
			beep();
		if (Prenum)
			stuffnum(Prenum);
		stuffin("d.");
		break;

	case 'X':
		CLEAROP;
		if (!oneleft())
			beep();
		else {
			strcpy(Redobuff, "X");
			u_saveline();
			delchar(true);
			updateline();
		}
		break;

	case 'r':
		CLEAROP;
		if (lineempty()) {	/* Nothing to replace */
			beep();
			break;
		}
		if ((nchar = vgetc()) == ESC)
			break;

		if (nchar==CR || nchar==NL) {
			stuffin("R\n\033");
			break;
		}

		if (nchar & 0x80) {
			beep();
			break;
		}
		u_saveline();

		/* Change current character. */
		pchar(Curschar, nchar);

		/* Save stuff necessary to redo it */
		sprintf(Redobuff, "r%c", nchar);

		CHANGED;
		updateline();
		break;

	case '~':		/* swap case */
		if (!P(P_TO)) {
			CLEAROP;
			if (lineempty()) {
				beep();
				break;
			}
			c = gchar(Curschar);

			if (isalpha(c)) {
				if (islower(c))
					c = toupper(c);
				else
					c = tolower(c);
			}
			u_saveline();

			pchar(Curschar, c);	/* Change current character. */
			oneright();

			strcpy(Redobuff, "~");

			CHANGED;
			updateline();
		}
#ifdef	TILDEOP
		else {
			if (operator == TILDE)		/* handle '~~' */
				goto lineop;
			if (Prenum != 0)
				opnum = Prenum;
			startop = *Curschar;
			operator = TILDE;
		}
#endif

		break;

	case 'J':
		CLEAROP;

		u_save(Curschar->linep->prev, Curschar->linep->next->next);

		if (!dojoin(true))
			beep();

		strcpy(Redobuff, "J");
		updatescreen();
		break;

	/*
	 * Inserts
	 */
	case 'A':
		set_want_col = true;
		while (oneright())
			;
		/* fall through */

	case 'a':
		CLEAROP;
		/* Works just like an 'i'nsert on the next character. */
		if (!lineempty())
			inc(Curschar);
		u_saveline();
		startinsert(mkstr(c), false);
		break;

	case 'I':
		beginline(true);
		/* fall through */

	case 'i':
	case KM_INSERT:
		CLEAROP;
		u_saveline();
		startinsert(mkstr(c), false);
		break;

	case 'o':
		CLEAROP;
		u_save(Curschar->linep, Curschar->linep->next);
		opencmd(FORWARD, true);
		startinsert("o", true);
		break;

	case 'O':
		CLEAROP;
		u_save(Curschar->linep->prev, Curschar->linep);
		opencmd(BACKWARD, true);
		startinsert("O", true);
		break;

	case 'R':
		CLEAROP;
		u_saveline();
		startinsert("R", false);
		break;

	/*
	 * Operators
	 */
	case 'd':
		if (operator == DELETE)		/* handle 'dd' */
			goto lineop;
		if (Prenum != 0)
			opnum = Prenum;
		startop = *Curschar;
		operator = DELETE;
		break;

	case 'c':
		if (operator == CHANGE)		/* handle 'cc' */
			goto lineop;
		if (Prenum != 0)
			opnum = Prenum;
		startop = *Curschar;
		operator = CHANGE;
		break;

	case 'y':
		if (operator == YANK)		/* handle 'yy' */
			goto lineop;
		if (Prenum != 0)
			opnum = Prenum;
		startop = *Curschar;
		operator = YANK;
		break;

	case '>':
		if (operator == RSHIFT)		/* handle >> */
			goto lineop;
		if (Prenum != 0)
			opnum = Prenum;
		startop = *Curschar;
		operator = RSHIFT;
		break;

	case '<':
		if (operator == LSHIFT)		/* handle << */
			goto lineop;
		if (Prenum != 0)
			opnum = Prenum;
		startop = *Curschar;	/* save current position */
		operator = LSHIFT;
		break;

	case '!':
		if (operator == FILTER)		/* handle '!!' */
			goto lineop;
		if (Prenum != 0)
			opnum = Prenum;
		startop = *Curschar;
		operator = FILTER;
		break;

	case 'p':
		doput(FORWARD);
		break;

	case 'P':
		doput(BACKWARD);
		break;

	/*
	 * Abbreviations
	 */
	case 'D':
		stuffin("d$");
		break;

	case 'Y':
		if (Prenum)
			stuffnum(Prenum);
		stuffin("yy");
		break;

	case 'C':
		stuffin("c$");
		break;

	case 's':				/* substitute characters */
		if (Prenum)
			stuffnum(Prenum);
		stuffin("c.");
		break;

	/*
	 * Marks
	 */
	case 'm':
		CLEAROP;
		if (!setmark(vgetc()))
			beep();
		break;

	case '\'':
		flag = true;
		/* fall through */

	case '`':
		{
			LPTR	mtmp, *mark;

			mark = getmark(vgetc());
			if (mark == NULL) {
				beep();
				CLEAROP;
			} else {
				mtmp = *mark;
				setpcmark();
				*Curschar = mtmp;
				if (flag)
					beginline(true);
			}
			mtype = flag ? MLINE : MCHAR;
			mincl = false;		/* ignored if not MCHAR */
			set_want_col = true;
		}
		break;

	default:
		CLEAROP;
		beep();
		break;
	}

	/*
	 * If an operation is pending, handle it...
	 */
	if (finish_op) {		/* we just finished an operator */
		if (operator == NOP)	/* ... but it was cancelled */
			return;

		switch (operator) {

		case LSHIFT:
		case RSHIFT:
			doshift(operator, c, nchar, Prenum);
			break;

		case DELETE:
			dodelete(c, nchar, Prenum);
			break;

		case YANK:
			(void) doyank();	/* no redo on yank... */
			break;

		case CHANGE:
			dochange(c, nchar, Prenum);
			break;

		case FILTER:
			dofilter(c, nchar, Prenum);
			break;

#ifdef	TILDEOP
		case TILDE:
			dotilde(c, nchar, Prenum);
			break;
#endif

		default:
			beep();
		}
		operator = NOP;
	}
}
