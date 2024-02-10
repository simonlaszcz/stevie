#ifndef MISCCMDS_H
#define MISCCMDS_H

#include <stdlib.h>

/*
 * opencmd
 *
 * Add a blank line below the current line.
 */
void opencmd(void);

int issepchar(char c);
int cntlines(char *pbegin, char *pend);
void fileinfo(void);
void gotoline(int n);

/*
 * yankline
 *
 * Save a copy of the current line(s) for later 'p'lacing.
 */
void yankline(int n);

/*
 * putline
 *
 * If there is a currently saved line(s), 'p'ut it.
 * If k==1, 'P'ut the line (i.e. above instead of below.
 */
void putline(int k);

void inschar(int c);
void insstr(char *s);
void appchar(int c);
int canincrease(size_t n);
void delchar(void);
void delword(int deltrailing);
void delline(int nlines);

#endif
