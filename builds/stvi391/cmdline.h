#ifndef CMDLINE_H
#define CMDLINE_H

void readcmdline(int firstc);
void badcmd(void);
void gotocmd(int clr, int fresh, int firstc);
void message(char *s);
int writeit(char *fname);
void filemess(char *s);

#endif
