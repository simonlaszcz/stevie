#ifndef EDIT_H
#define EDIT_H

void edit(void);
void insertchar(int c);
int gethexchar(void);
void getout(void);
void cursupdate(void);
void scrolldown(int nlines);

/*
 * oneright
 * oneleft
 * onedown
 * oneup
 *
 * Move one char {right,left,down,up}.  Return 1 when
 * sucessful, 0 when we hit a boundary (of a line, or the file).
 */
int oneright(void);
int oneleft(void);
void beginline(void);
int oneup(int n);
int onedown(int n);

#endif
