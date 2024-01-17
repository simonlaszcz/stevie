/* $Header: /nw/tony/src/stevie/src/RCS/alloc.c,v 1.5 89/08/06 09:49:22 tony Exp $
 * Various allocation routines and routines returning information about
 * allocated objects.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "alloc.h"
#include "cmdline.h"
#include "undo.h"
#include "mark.h"

char *alloc(size_t size)
{
	char *p = (char *)malloc(size);

	if (p == NULL) {
		/* if there is no more room... */
		emsg("alloc() is unable to find memory!");
	}

	return p;
}

char *strsave(char *string)
{
	return strcpy(alloc(strlen(string) + 1), string);
}

int screenalloc()
{
	/*
	 * If we're changing the size of the screen, free the old arrays
	 */
	if (Realscreen != NULL)
		free(Realscreen);
	if (Nextscreen != NULL)
		free(Nextscreen);

	Realscreen = malloc(Rows * Columns);
	Nextscreen = malloc(Rows * Columns);

	if (Realscreen == NULL || Nextscreen == NULL)
		return -1;

	return 0;
}

/*
 * Allocate and initialize a new line structure with room for
 * 'nchars'+1 characters. We add one to nchars here to allow for
 * null termination because all the callers would just do it otherwise.
 */
EDLINE *newline(size_t nchars)
{
	register EDLINE *l;

	if ((l = (EDLINE *) alloc(sizeof(EDLINE))) == NULL)
		return NULL;

	l->s = alloc(nchars + 1);	/* the line is empty */
	if (l->s == NULL)
		return NULL;
	l->s[0] = NUL;
	l->size = nchars + 1;
	l->prev = NULL;		/* should be initialized by caller */
	l->next = NULL;

	return l;
}

/*
 * filealloc() - construct an initial empty file buffer
 */
void filealloc()
{
	if ((Filemem->linep = newline(0)) == NULL) {
		fprintf(stderr, "Unable to allocate file memory!\n");
		exit(1);
	}
	if ((Filetop->linep = newline(0)) == NULL) {
		fprintf(stderr, "Unable to allocate file memory!\n");
		exit(1);
	}
	if ((Fileend->linep = newline(0)) == NULL) {
		fprintf(stderr, "Unable to allocate file memory!\n");
		exit(1);
	}
	Filemem->index = 0;
	Filetop->index = 0;
	Fileend->index = 0;

	Filetop->linep->next = Filemem->linep;	/* connect Filetop to Filemem */
	Filemem->linep->prev = Filetop->linep;

	Filemem->linep->next = Fileend->linep;	/* connect Filemem to Fileend */
	Fileend->linep->prev = Filemem->linep;

	*Curschar = *Filemem;
	*Topchar = *Filemem;

	Filemem->linep->num = 0;
	Fileend->linep->num = 0xffff;

	clrall();		/* clear all marks */
	u_clear();		/* clear the undo buffer */
}

/*
 * freeall() - free the current buffer
 *
 * Free all lines in the current buffer.
 */
void freeall()
{
	register EDLINE *lp, *xlp;

	for (lp = Filetop->linep; lp != NULL; lp = xlp) {
		if (lp->s != NULL)
			free(lp->s);
		xlp = lp->next;
		free(lp);
	}

	Curschar->linep = NULL;	/* clear pointers */
	Filetop->linep = NULL;
	Filemem->linep = NULL;
	Fileend->linep = NULL;

	u_clear();
}

/*
 * bufempty() - return TRUE if the buffer is empty
 */
bool bufempty()
{
	return (buf1line() && Filemem->linep->s[0] == NUL);
}

/*
 * buf1line() - return TRUE if there is only one line
 */
bool buf1line()
{
	return (Filemem->linep->next == Fileend->linep);
}

/*
 * lineempty() - return TRUE if the current line is empty
 */
bool lineempty()
{
	return (Curschar->linep->s[0] == NUL);
}

/*
 * endofline() - return TRUE if the given position is at end of line
 *
 * This routine will probably never be called with a position resting
 * on the NUL byte, but handle it correctly in case it happens.
 */
bool endofline(register LPTR * p)
{
	return (p->linep->s[p->index] == NUL
		|| p->linep->s[p->index + 1] == NUL);
}

/*
 * canincrease(n) - returns TRUE if the current line can be increased 'n' bytes
 *
 * This routine returns immediately if the requested space is available.
 * If not, it attempts to allocate the space and adjust the data structures
 * accordingly. If everything fails it returns FALSE.
 */
bool canincrease(register size_t n)
{
	register size_t nsize;
	register char *s;	/* pointer to new space */

	nsize = strlen(Curschar->linep->s) + 1 + n;	/* size required */

	if (nsize <= (size_t)Curschar->linep->size)
		return true;

	/*
	 * Need to allocate more space for the string. Allow some extra
	 * space on the assumption that we may need it soon. This avoids
	 * excessive numbers of calls to malloc while entering new text.
	 */
	if ((s = alloc(nsize + SLOP)) == NULL) {
		emsg("Can't add anything, file is too big!");
		State = NORMAL;
		return false;
	}

	Curschar->linep->size = nsize + SLOP;
	strcpy(s, Curschar->linep->s);
	free(Curschar->linep->s);
	Curschar->linep->s = s;

	return true;
}

char *mkstr(char c)
{
	static char s[2];

	s[0] = c;
	s[1] = NUL;

	return s;
}
