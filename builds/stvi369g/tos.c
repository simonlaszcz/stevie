/* $Header: /nw/tony/src/stevie/src/RCS/tos.c,v 1.5 89/07/13 22:45:31 tony Exp $
 *
 * System-dependent routines for the Atari ST.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef __PUREC__
#include <tos.h>
#define FA_INQUIRE	(0)
#define FA_SET		(1)
#else
#include <osbind.h>
#endif

#include "stevie.h"
#include "keymap.h"
#include "param.h"
#include "machine.h"
#include "cmdline.h"

#define	PSIZE	(128)
#define CMD_MAX	(124)

static void vbeep(void);

/*
 * inchar() - get a character from the keyboard
 *
 * Certain special keys are mapped to values above 0x80. These
 * mappings are defined in keymap.h. If the key has a non-zero
 * ascii value, it is simply returned. Otherwise it may be a
 * special key we want to map.
 *
 * We can call stuffin here. Return -1 if we do.
 
 * The ST has a bug involving keyboard input that seems to occur
 * when typing quickly, especially typing capital letters. Sometimes
 * a value of 0x02540000 is read. This doesn't correspond to anything
 * on the keyboard, according to my documentation. My solution is to
 * loop when any unknown key is seen. Normally, the bell is rung to
 * indicate the error. If the "bug" value is seen, we ignore it completely.
 */
int
inchar()
{
	register long	c;

	for (;;) {
		c = Bconin(2);
	
		if ((c & 0xff) != 0)
			return ((int)c & 0xff);
	
		switch ((int) (c >> 16) & 0xff) {
	
		case 0x62: return KM_HELP;
		case 0x61: return KM_UNDO;
		case 0x52: return KM_INSERT;
		case 0x47: return KM_HOME;
		case 0x48: return KM_UARROW;
		case 0x50: return KM_DARROW;
		case 0x4b: return KM_LARROW;
		case 0x4d: return KM_RARROW;
		case 0x29: return KM_CCIRCM;	/* control-circumflex */
		
		/*
		 * Occurs due to a bug in TOS.
		 */
		case 0x54:
			break;
		/*
		 * Add the function keys here later if we put in support
		 * for macros.
		 */
	
		default:
			beep();
			break;
	
		}
	}
}

void
outchar(char c)
{
	if (c < ' ')
		Bconout(2, c);
	else
		Bconout(5, c);
}

void
outstr(register char *s)
{
	while (*s)
		Bconout(2, *s++);
}

/*
 * flushbuf() - a no-op for TOS
 */
void
flushbuf()
{
	return;
}

#define	BGND	0
#define	TEXT	3

/*
 * vbeep() - visual bell
 */
static void
vbeep(void)
{
	int	text, bgnd;		/* text and background colors */
	long	l;

	Vsync();
	text = Setcolor(TEXT, -1);
	bgnd = Setcolor(BGND, -1);

	(void)Setcolor(TEXT, bgnd);		/* swap colors */
	(void)Setcolor(BGND, text);

	for (l=0; l < 5000 ;l++)	/* short pause */
		;

	Vsync();
	(void)Setcolor(TEXT, text);		/* restore colors */
	(void)Setcolor(BGND, bgnd);
}

void
beep()
{
	if (P(P_VB))
		vbeep();
	else
		outchar('\007');
}

void
windinit()
{
	if (Getrez() == 0)
		Columns = 40;		/* low resolution */
	else
		Columns = 80;		/* medium or high */

	P(P_LI) = Rows = 25;

	(void)Cursconf(1, 0);
}

void
windexit(int r)
{
	exit(r);
}

static	char	gobuf[5] = { '\033', 'Y', '\0', '\0', '\0' };

void
windgoto(int r, int c)
{
	gobuf[2] = r + 040;
	gobuf[3] = c + 040;
	outstr(gobuf);
}

/*
 * System calls or library routines missing in TOS.
 */
void
sleep(int n)
{
	int	k;

	k = Tgettime();
	while ( Tgettime() <= k+n )
		;
}

void
pause()
{
	long	n;

	for (n = 0; n < 8000 ;n++)
		;
}

FILE *
fopenb(char *fname, char *mode)
{
	char	modestr[10];

	sprintf(modestr, "%sb", mode);

	return fopen(fname, modestr);
}

/*
 * mktemp() - quick hack since there isn't one here
 */
char *
mktemp(char *name)
{
	int	num;		/* pasted into the string to make it unique */
	char	cbuf[7];
	char	*s;		/* where the X's start in name */
	long fd = 0;

	if ((s = strchr(name, 'X')) == NULL)	/* needs to be an X */
		return (char *) NULL;

	if (strlen(s) != 6)			/* should be 6 X's */
		return (char *) NULL;

	for (num = 0; num < 1000 ;num++) {
		sprintf(cbuf, "%06d", num);
		strcpy(s, cbuf);
		if ((fd = Fopen(name, 0)) < 0)
			return name;
		Fclose((int)fd);
	}
	return (char *) NULL;
}

void
doshell(char *cmd)
{
	if (cmd == NULL) {
/*
		TODO: what to do here?
		shell();
*/
		wait_return();
		return;
	}
	syscall(cmd);
	wait_return();
}

/*
 * fixname(s) - fix up a dos name
 *
 * Takes a name like:
 *
 *	\x\y\z\base.ext
 *
 * and trims 'base' to 8 characters, and 'ext' to 3.
 */
char *
fixname(char *s)
{
	static	char	f[PSIZE];
	char	base[32];
	char	ext[32];
	char	*p;
	int	i;

	strcpy(f, s);

	for (i=0; i < PSIZE ;i++)
		if (f[i] == '/')
			f[i] = '\\';

	/*
	 * Split the name into directory, base, extension.
	 */
	if ((p = strrchr(f, '\\')) != NULL) {
		strcpy(base, p+1);
		p[1] = '\0';
	} else {
		strcpy(base, f);
		f[0] = '\0';
	}

	if ((p = strchr(base, '.')) != NULL) {
		strcpy(ext, p+1);
		*p = '\0';
	} else
		ext[0] = '\0';

	/*
	 * Trim the base name if necessary.
	 */
	if (strlen(base) > 8)
		base[8] = '\0';
	
	if (strlen(ext) > 3)
		ext[3] = '\0';

	/*
	 * Paste it all back together
	 */
	strcat(f, base);
	strcat(f, ".");
	strcat(f, ext);

	return f;
}

/*
 *	FILL IT IN, FOR YOUR SYSTEM, AND SHARE IT!
 *
 *	The next couple of functions do system-specific stuff.
 *	They currently do nothing; I'm not familiar enough with
 *	system-specific programming on this system.
 *	If you fill it in for your system, please post the results
 *	and share with the rest of us.
 */
void
setcolor(int c)
/*
 * Set the color to c, using the local system convention for numbering
 * colors or video attributes.
 *
 * If you implement this, remember to note the original color in
 * windinit(), before you do any setcolor() commands, and
 * do a setcolor() back to the original as part of windexit().
 */
{
	UNUSED(c);
	return;
}

/**
 * Set the number of lines to r, if possible.  Otherwise
 * "do the right thing".  Return the number of lines actually set.
 *
 * If you implement this, remember to note the original number of rows
 * in windinit(), before you do any setrows() commands, and
 * do a setrows() back to the original as part of windexit().
 */
int
setrows(int r)
{
	UNUSED(r);
	/* Since we do nothing, just return the current number of lines */
	return ( P(P_LI) );
}

/**
 * Returns the inverse video attribute or color of co.
 * The existing code below is VERY simple-minded.
 * Replace it with proper code for your system.
 */
int
reverse_color(int co)
{
	if (co)		return (0);
	else		return (1);
}

int
fstat(const char *path, STATBUF *buf)
{
	*buf = Fattrib(path, FA_INQUIRE, 0);

	/* 0: success */
	return *buf < 0;
}

int
fchmod(const char *path, STATBUF *buf)
{
	/* 0: success */
	return Fattrib(path, FA_SET, *buf) < 0;
}

long
syscall(const char *cmd)
{
	char path[PSIZE];
	/* pascal format: length in first byte (excluding null) */
	char args[CMD_MAX + 2];
	int pos = 0;
	int pos2 = 0;

	if (cmd == NULL)
		return 1;

	pos = 0;
	while (pos < PSIZE - 1 && cmd[pos] != NUL) {
		path[pos] = cmd[pos];
		++pos;
	}
	path[pos++] = NUL;

	args[0] = 0;
	pos2 = 1;
	while (pos2 <= CMD_MAX && cmd[pos] != NUL)
		args[pos2++] = cmd[pos++];
	args[pos2] = NUL;
	args[0] = pos2 - 1;

	return Pexec(0, path, args, NULL);
}
