/*
 * STevie - ST editor for VI enthusiasts.    ...Tim Thompson...twitch!tjt...
 */

#ifndef STEVIE_H
#define STEVIE_H

/* One (and only 1) of the following 3 defines should be uncommented. */
/* Most of the code is machine-independent.  Most of the machine- */
/* dependent stuff is in window.c */

/*#define ATARI		1*/	/* For the Atari 520 ST */
/*#define UNIXPC	1*/	/* The AT&T UNIX PC (console) */
/*#define TCAP		1*/	/* For termcap-based terminals */

#define FILELENG (64000L)
#define NORMAL 0
#define CMDLINE 1
#define INSERT 2
#define APPEND 3
#define FORWARD 4
#define BACKWARD 5
#define WORDSEP " \t\n()[]{},;:'\"-="

#define CHANGED Changed=1
#define UNCHANGED Changed=0

#ifndef NULL
#define NULL ((void *)0)
#endif

struct charinfo {
	char ch_size;
	char *ch_str;
};

extern struct charinfo chars[];

extern int State;
extern int Rows;
extern int Columns;
extern char *Realscreen;
extern char *Nextscreen;
extern char *Filename;
extern char *Filemem;
extern char *Filemax;
extern char *Fileend;
extern char *Topchar;
extern char *Botchar;
extern char *Curschar;
extern char *Insstart;
extern int Cursrow, Curscol, Cursvcol;
extern int Prenum;
extern int Debug;
extern int Changed;
extern int Binary;
extern char Redobuff[], Undobuff[], Insbuff[];
extern char *Uncurschar, *Insptr;
extern int Ninsert, Undelchars;

#endif
