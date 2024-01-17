/*
 * $Header: /nw/tony/src/stevie/src/RCS/keymap.h,v 1.2 89/03/11 22:42:30 tony Exp $
 *
 * Keycode definitions for special keys
 *
 * On systems that have any of these keys, the routine 'inchar' in the
 * machine-dependent code should return one of the codes here.
 */

#ifndef KEYMAP_H
#define KEYMAP_H

#define	KM_HELP		(0x80)
#define	KM_UNDO		(0x81)
#define	KM_INSERT	(0x82)
#define	KM_HOME		(0x83)
#define	KM_UARROW	(0x84)
#define	KM_DARROW	(0x85)
#define	KM_LARROW	(0x86)
#define	KM_RARROW	(0x87)
#define	KM_CCIRCM	(0x88) /* control-circumflex */

#define	KM_F1		(0x91) /* function keys */
#define	KM_F2		(0x92)
#define	KM_F3		(0x93)
#define	KM_F4		(0x94)
#define	KM_F5		(0x95)
#define	KM_F6		(0x96)
#define	KM_F7		(0x97)
#define	KM_F8		(0x98)
#define	KM_F9		(0x99)
#define	KM_F10		(0x9a)

#define	KM_SF1		(0xa1) /* shifted function keys */
#define	KM_SF2		(0xa2)
#define	KM_SF3		(0xa3)
#define	KM_SF4		(0xa4)
#define	KM_SF5		(0xa5)
#define	KM_SF6		(0xa6)
#define	KM_SF7		(0xa7)
#define	KM_SF8		(0xa8)
#define	KM_SF9		(0xa9)
#define	KM_SF10		(0xaa)

#endif
