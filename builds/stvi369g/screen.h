/**
 * Routines to manipulate the screen representations.
 */

#ifndef SCREEN_H
#define SCREEN_H

/*
 * updateline() - update the line the cursor is on
 *
 * Updateline() is called after changes that only affect the line that
 * the cursor is on. This improves performance tremendously for normal
 * insert mode operation. The only thing we have to watch for is when
 * the cursor line grows or shrinks around a row boundary. This means
 * we have to repaint other parts of the screen appropriately. If
 * lfiletonext() returns false, the size of the cursor line (in rows)
 * has changed and we have to call updatescreen() to do a complete job.
 */
void updateline(void);

void updatescreen(void);

/*
 * prt_line() - print the given line
 */
void prt_line(char *s);

void screenclear(void);
void cursupdate(void);

/*
 * The rest of the routines in this file perform screen manipulations.
 * The given operation is performed physically on the screen. The
 * corresponding change is also made to the internal screen image.
 * In this way, the editor anticipates the effect of editing changes
 * on the appearance of the screen. That way, when we call screenupdate
 * a complete redraw isn't usually necessary. Another advantage is that
 * we can keep adding code to anticipate screen changes, and in the
 * meantime, everything still works.
 */

/*
 * s_ins(row, nlines) - insert 'nlines' lines at 'row'
 */
void s_ins(int row, int nlines);

/*
 * s_del(row, nlines) - delete 'nlines' lines at 'row'
 */
void s_del(int row, int nlines);

#endif
