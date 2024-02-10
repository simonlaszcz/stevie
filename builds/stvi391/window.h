#ifndef WINDOW_H 
#define WINDOW_H 

void windinit(void);
void windgoto(int r, int c);
void windexit(int r);
void windclear(void);
int windgetc(void);
void windstr(char *s);
void windputc(int c);
void windrefresh(void);
void beep(void);

#endif