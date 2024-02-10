#ifndef LINEFUNC_H
#define LINEFUNC_H

#include <stdlib.h>

/*
 * nextline(curr)
 *
 * Return a pointer to the beginning of the next line after the
 * 'curr' char.  Return NULL if there is no next line.
 */
char *nextline(char *curr);

/*
 * prevline(curr)
 *
 * Return a pointer to the beginning of the previous line before the
 * 'curr' char.  Return NULL if there is no previous line.
 */
char *prevline(char *curr);

/*
 * coladvance(p,col)
 *
 * Try to advance to the specified column, starting at p.
 */
char *coladvance(char *p, int col);

char *alloc(size_t size);
char *strsave(char *string);
char *ssearch(int dir, char *str);
void dosearch(int dir, char *str);
void repsearch(void);
char *fwdsearch(char *str);
char *bcksearch(char *str);

#endif
