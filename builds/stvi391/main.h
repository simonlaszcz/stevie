#ifndef MAIN_H
#define MAIN_H

/*
 * filetonext()
 *
 * Based on the current value of Topchar, transfer a screenfull of
 * stuff from Filemem to Nextscreen, and update Botchar.
 */
void filetonext(void);

/*
 * nexttoscreen
 *
 * Transfer the contents of Nextscreen to the screen, using Realscreen
 * to avoid unnecessary output.
 */
void nexttoscreen(void);
void updatescreen(void);
void screenclear(void);
void filealloc(void);
void screenalloc(void);
int readfile(char *fname, char *fromp, int nochangename);
void stuffin(char *s);
void addtobuff(char *s, char c1, char c2, char c3, char c4, char c5, char c6);
int vgetc(void);
int vpeekc(void);

/*
 * anyinput
 *
 * Return non-zero if input is pending.
 */
int anyinput(void);

#endif
