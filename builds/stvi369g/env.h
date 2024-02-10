#ifndef ENV_H
#define ENV_H

/*
 * The defines in this file establish the environment we're compiling
 * in. Set these appropriately before compiling the editor.
 *
 * SET IN MAKEFILE
 */

/*
 * One (and only 1) of the following defines should be uncommented.
 * Most of the code is pretty machine-independent. Machine dependent
 * code goes in a file like tos.c or unix.c. The only other place
 * where machine dependent code goes is term.h for escape sequences.
 */

/* #define	ATARI			 For the Atari ST */
/* #define	UNIX			 System V or BSD */
/* #define	OS2			 Microsoft OS/2 1.1 */
/* #define	DOS			 MSDOS 3.3 (on AT) */

/*
 * The yank buffer is still static, but its size can be specified
 * here to override the default of 4K.
 */
#define	YBSIZE	8192		/* yank buffer size */

/*
 * The following defines control the inclusion of "optional" features. As
 * the code size of the editor grows, it will probably be useful to be able
 * to tailor the editor to get the features you most want in environments
 * with code size limits.
 *
 * TILDEOP
 *	Normally the '~' command works on a single character. This define
 *	turns on code that allows it to work like an operator. This is
 *	then enabled at runtime with the "tildeop" parameter.
 *
 * HELP
 *	If defined, a series of help screens may be viewed with the ":help"
 *	command. This eats a fair amount of data space.
 *
 * TERMCAP
 *	If defined, STEVIE uses TERMCAP to determine the escape sequences
 *	to control the screen; if so, TERMCAP support had better be there.
 *	If not defined, you generally get hard-coded escape sequences for
 *	some "reasonable" terminal. In Minix, this means the console. For
 *	UNIX, this means an ANSI standard terminal. For MSDOS, this means
 *	a good ANSI driver (like NANSI.SYS, not ANSI.SYS).
 *	See the file "term.h" for details about specific environments.
 *
 * TAGSTACK
 *	If defined, this includes code that stacks calls to ':ta'.  The
 *	additional command ':untag' pops the stack back to the point at
 *	which the call to ':ta' was made.  In this mode, if the tag stack
 *	is not empty, Ctrl-^ will be interpreted as ':untag' rather than
 *	':e #'.
 *
 */
/*
#define	TILDEOP
#define	HELP
#define	TERMCAP
#define	TAGSTACK
*/

#endif
