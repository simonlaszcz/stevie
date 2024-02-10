/**
 * Undo facility
 *
 * The routines in this file comprise a general undo facility for use
 * throughout the rest of the editor. The routine u_save() is called
 * before each edit operation to save the current contents of the lines
 * to be editted. Later, u_undo() can be called to return those lines
 * to their original state. The routine u_clear() should be called
 * whenever a new file is going to be editted to clear the undo buffer.
 */

#ifndef UNDO_H
#define UNDO_H

#include "stevie.h"

/*
 * u_save(l, u) - save the current contents of part of the file
 *
 * The lines between 'l' and 'u' are about to be changed. This routine
 * saves their current contents into the undo buffer. The range l to u
 * is not inclusive because when we do an open, for example, there aren't
 * any lines in between. If no lines are to be saved, then l->next == u.
 */
void u_save(EDLINE *l, EDLINE *u);

/*
 * u_saveline() - save the current line in the undo buffer
 */
void u_saveline(void);

/*
 * u_undo() - effect an 'undo' operation
 *
 * The last edit is undone by restoring the modified section of the file
 * to its original state. The lines we're going to trash are copied to
 * the undo buffer so that even an 'undo' can be undone. Rings the bell
 * if the undo buffer is empty.
 */
void u_undo(void);

/*
 * u_clear() - clear the undo buffer
 *
 * This routine is called to clear the undo buffer at times when the
 * pointers are about to become invalid, such as when a new file is
 * about to be editted.
 */
void u_clear(void);

/*
 * u_lundo() - undo the current line (the 'U' command)
 */
void u_lundo(void);

/*
 * u_lcheck() - clear the "line undo" buffer if we've moved to a new line
 */
void u_lcheck(void);

#endif
