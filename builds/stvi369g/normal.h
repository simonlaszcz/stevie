/**
 * Contains the main routine for processing characters in command mode.
 * Communicates closely with the code in ops.c to handle the operators.
 */

#ifndef NORMAL_H
#define NORMAL_H

/*
 * normal(c)
 *
 * Execute a command in command mode.
 *
 * This is basically a big switch with the cases arranged in rough categories
 * in the following order:
 *
 *	1. File positioning commands
 *	2. Control commands (e.g. ^G, Z, screen redraw, etc)
 *	3. Character motions
 *	4. Search commands (of various kinds)
 *	5. Edit commands (e.g. J, x, X)
 *	6. Insert commands (e.g. i, o, O, A)
 *	7. Operators
 *	8. Abbreviations (e.g. D, C)
 *	9. Marks
 */
void normal(register int c);

#endif
