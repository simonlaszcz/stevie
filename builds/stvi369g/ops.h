/* $Header: /nw/tony/src/stevie/src/RCS/ops.h,v 1.2 89/07/19 08:08:21 tony Exp $
 *
 * Macros and declarations for the operator code in ops.c
 */

#ifndef OPS_H
#define OPS_H

#include "bool.h"
#include "stevie.h"

/*
 * Operators
 */
#define	NOP	0		/* no pending operation */
#define	DELETE	1
#define	YANK	2
#define	CHANGE	3
#define	LSHIFT	4
#define	RSHIFT	5
#define	FILTER	6
#define	TILDE	7

extern	int	operator;		/* current pending operator */

/*
 * When a cursor motion command is made, it is marked as being a character
 * or line oriented motion. Then, if an operator is in effect, the operation
 * becomes character or line oriented accordingly.
 *
 * Character motions are marked as being inclusive or not. Most char.
 * motions are inclusive, but some (e.g. 'w') are not.
 */

/*
 * Cursor motion types
 */
#define	MBAD	(-1)		/* 'bad' motion type marks unusable yank buf */
#define	MCHAR	0
#define	MLINE	1

extern	int	mtype;			/* type of the current cursor motion */
extern	bool	mincl;			/* true if char motion is inclusive */
extern	LPTR	startop;		/* cursor pos. at start of operator */

/*
 * Macro to get current character from a LPTR * value.
 */
#define	CHAR( lpp )	lpp->linep->s[lpp->index]

/*
 * doshift - handle a shift operation
 */
void doshift(int op, char c1, char c2, int num);

/*
 * dodelete - handle a delete operation
 */
void dodelete(char c1, char c2, int num);

/*
 * dofilter - filter lines through a command given by the user
 *
 * We use temp files and the system() routine here. This would normally
 * be done using pipes on a UNIX machine, but this is more portable to
 * the machines we usually run on. The system() routine needs to be able
 * to deal with redirection somehow, and should handle things like looking
 * at the PATH env. variable, and adding reasonable extensions to the
 * command name given by the user. All reasonable versions of system()
 * do this.
 */
void dofilter(char c1, char c2, int num);

#ifdef	TILDEOP
void dotilde(char c1, char c2, int num);
#endif

/*
 * dochange - handle a change operation
 */
void dochange(char c1, char c2, int num);

bool doyank(void);

/*
 * doput(dir)
 *
 * Put the yank buffer at the current location, using the direction given
 * by 'dir'.
 */
void doput(int dir);

/* @param join_cmd handling a real "join" command? */
bool dojoin(bool join_cmd);

/* @param startln if set, insert point really at start of line */
void startinsert(char *initstr, int startln);

#endif
