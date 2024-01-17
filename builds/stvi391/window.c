/*
 * STevie - ST editor for VI enthusiasts.    ...Tim Thompson...twitch!tjt...
 */

#include <stdio.h>
#include <stdlib.h>

#ifdef ATARI
#ifdef __PUREC__
#include <tos.h>
#else
#include <osbind.h>
#endif
#define EscSeq(x) Cconout('\033');Cconout(x);
#endif
#ifdef UNIXPC
#include <sys/window.h>
#endif
#ifdef TCAP
#include <curses.h>
#endif

#include "stevie.h"
#include "window.h"

void
windinit(void)
{
#ifdef ATARI
	Columns=80;
	Rows=25;
	Cursconf(1, 0);
#endif
#ifdef UNIXPC
        struct uwdata uw;
        
	winit();
	if ( ioctl(0,WIOCGETD,&uw) == -1
	  && ioctl(1,WIOCGETD,&uw) == -1
	  && ioctl(2,WIOCGETD,&uw) == -1 ) {
		fprintf(stderr,"*** ERROR *** Not a window!\n");
		windexit(1);
	}
	Columns = uw.uw_width / uw.uw_hs;
	Rows = uw.uw_height / uw.uw_vs;
	cbreak();
	nonl();
	noecho();
#endif
#ifdef TCAP
	char *getenv();
	char *p = getenv("TERM");

	initscr();
	Columns = 80;
	if ( strncmp(p,"vt52",4)==0 )
		Rows = 25;
	else
		Rows = 24;
	cbreak();
	nonl();
	noecho();
#endif
}

void
windgoto(int r, int c)
{
#ifdef UNIXPC
	printf("\033[%d;%dH",r+1,c+1);
#endif
#ifdef ATARI
	EscSeq('Y');
	Cconout(r+040);
	Cconout(c+040);
#endif
#ifdef TCAP
	move(r,c);
#endif
}

void
windexit(int r)
{
#ifdef UNIXPC
	nocbreak();
	nl();
	echo();
	wexit();
#endif
#ifdef TCAP
	nocbreak();
	nl();
	echo();
	endwin();
#endif
	exit(r);
}

void
windclear(void)
{
#ifdef UNIXPC
	printf("\033[H\033[J");
#endif
#ifdef ATARI
	Cconws("\033H\033J");
#endif
#ifdef TCAP
	clear();
	refresh();
#endif
}

int
windgetc(void)
{
#ifdef ATARI
	return((int)Cnecin());
#else
	return(getchar());
#endif
}

void
windstr(char *s)
{
#ifdef ATARI
	Cconws(s);
#endif
#ifdef UNIXPC
	printf("%s",s);
#endif
#ifdef TCAP
	addstr(s);
	refresh();
#endif
}

void
windputc(int c)
{
#ifdef ATARI
	Cconout(c);
#endif
#ifdef UNIXPC
	putchar(c);
#endif
#ifdef TCAP
	addch(c);
#endif
}

void
windrefresh(void)
{
#ifdef TCAP
	refresh();
#endif
}

void
beep(void)
{
#ifdef ATARI
	Cconout('\007');
#else
	putchar('\007');
#endif
}
