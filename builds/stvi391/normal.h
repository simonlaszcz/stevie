#ifndef NORMAL_H
#define NORMAL_H

void normal(int c);

/*
 * tabinout(inout,num)
 *
 * If inout==0, add a tab to the begining of the next num lines.
 * If inout==1, delete a tab from the begining of the next num lines.
 */
void tabinout(int inout, int num);
void startinsert(char *initstr);
void resetundo(void);

#endif
