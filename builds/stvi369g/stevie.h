/*
 * $Header: /nw/tony/src/stevie/src/RCS/stevie.h,v 1.19 89/07/12 21:33:32 tony Exp $
 *
 * Main header file included by all source files.
 */

#ifndef STEVIE_H
#define STEVIE_H

#include <stddef.h>
#include "bool.h"
#include "env.h"	/* defines to establish the compile-time environment */

#ifndef NUL
#define NUL			'\0'
#endif

#define NULCH		'\0'

#define UNUSED(x)	(void)(x)

#define NORMAL		(0)
#define CMDLINE 	(1)
#define INSERT 		(2)
#define REPLACE 	(3)
#define FORWARD 	(4)
#define BACKWARD 	(5)

/*
 * SLOP is the amount of extra space we get for text on a line during
 * editing operations that need more space. This keeps us from calling
 * malloc every time we get a character during insert mode. No extra
 * space is allocated when the file is initially read.
 */
#define	SLOP		(10)

/*
 * LINEINC is the gap we leave between the artificial line numbers. This
 * helps to avoid renumbering all the lines every time a new line is
 * inserted.
 */
#define	LINEINC		(10)

#define CHANGED		Changed=true
#define UNCHANGED	Changed=false

struct line {
	struct	line	*prev, *next;	/* previous and next lines */
	char	*s;			/* text for this line */
	size_t	size;			/* actual size of space at 's' */
	unsigned long	num;		/* line "number" */
};

#define	LINEOF(x)	((x)->linep->num)

struct	lptr {
	struct	line	*linep;		/* line we're referencing */
	size_t	index;			/* position within that line */
};

typedef	struct line	EDLINE;
typedef	struct lptr	LPTR;

struct charinfo {
	char ch_size;
	char *ch_str;
};

extern struct charinfo chars[];

extern	int	State;
extern	int	Rows;
extern	int	Columns;
extern	char	*Realscreen;
extern	char	*Nextscreen;
extern	char	*Filename;
extern	LPTR	*Filemem;
extern	LPTR	*Filetop;
extern	LPTR	*Fileend;
extern	LPTR	*Topchar;
extern	LPTR	*Botchar;
extern	LPTR	*Curschar;
extern	LPTR	*Insstart;
extern	int	Cursrow, Curscol, Cursvcol, Curswant;
extern	bool	set_want_col;
extern	int	Prenum;
extern	bool	Changed;
extern	char	Redobuff[], Insbuff[];
extern	char	*Insptr;
extern	int	Ninsert;
extern	bool	got_int;

#endif
