/* $Header: /nw/tony/src/stevie/src/RCS/param.c,v 1.10 89/08/02 10:59:10 tony Exp $
 *
 * Code to handle user-settable parameters. This is all pretty much table-
 * driven. To add a new parameter, put it in the params array, and add a
 * macro for it in param.h. If it's a numeric parameter, add any necessary
 * bounds checks to doset(). String parameters aren't currently supported.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "param.h"
#include "cmdline.h"
#include "stevie.h"
#include "machine.h"
#include "screen.h"
#include "alloc.h"
#include "bool.h"

struct	param	params[] = {
	{ "tabstop",	"ts",		8,	P_NUM },
	{ "scroll",	"scroll",	12,	P_NUM },
	{ "report",	"report",	5,	P_NUM },
	{ "lines",	"lines",	25,	P_NUM },
	{ "vbell",	"vb",		true,	P_BOOL },
	{ "showmatch",	"sm",		false,	P_BOOL },
	{ "wrapscan",	"ws",		true,	P_BOOL },
	{ "errorbells",	"eb",		false,	P_BOOL },
	{ "showmode",	"mo",		false,	P_BOOL },
	{ "backup",	"bk",		false,	P_BOOL },
	{ "return",	"cr",		true,	P_BOOL },
	{ "list",	"list",		false,	P_BOOL },
	{ "ignorecase",	"ic",		false,	P_BOOL },
	{ "autoindent",	"ai",		false,	P_BOOL },
	{ "number",	"nu",		false,	P_BOOL },
	{ "modelines",	"ml",		false,	P_BOOL },
	{ "tildeop",	"to",		false,	P_BOOL },
	{ "terse",	"terse",	false,	P_BOOL },
	{ "tagstack",	"tg",		false,	P_BOOL },
	{ "color",	"co",		-1,	P_NUM  },
	{ "",		"",		0,	0, }		/* end marker */

};

static	void	showparms(bool all);

void
doset(char *arg)
{
	register int	i;
	register char	*s;
	bool	did_lines = false;
	bool	state = true;		/* new state of boolean parms. */

	if (arg == NULL) {
		showparms(false);
		return;
	}
	if (strncmp(arg, "all", 3) == 0) {
		showparms(true);
		return;
	}
	if (strncmp(arg, "no", 2) == 0) {
		state = false;
		arg += 2;
	}

	for (i=0; params[i].fullname[0] != NUL ;i++) {
		s = params[i].fullname;
		if (strncmp(arg, s, strlen(s)) == 0)	/* matched full name */
			break;
		s = params[i].shortname;
		if (strncmp(arg, s, strlen(s)) == 0)	/* matched short name */
			break;
	}

	if (params[i].fullname[0] != NUL) {	/* found a match */
		if (params[i].flags & P_NUM) {
			did_lines = (i == P_LI);
			if (arg[strlen(s)] != '=' || state == false)
				emsg("Invalid set of numeric parameter");
			else {
				params[i].value = atoi(arg+strlen(s)+1);
				params[i].flags |= P_CHANGED;
				if (i==P_CO)  setcolor (P(P_CO));
			}
		} else /* boolean */ {
			if (arg[strlen(s)] == '=')
				emsg("Invalid set of boolean parameter");
			else {
				params[i].value = state;
				params[i].flags |= P_CHANGED;
			}
		}
	} else
		emsg("Unrecognized 'set' option");

	/*
	 * Update the screen in case we changed something like "tabstop"
	 * or "list" that will change its appearance.
	 */
	updatescreen();

	if (did_lines) {
		Rows = P(P_LI);
		P(P_LI) = Rows = setrows( Rows );
					/* setrows() is system-dependent.
					 * This assures no impossible values
					 * will be set.
					 */
		if (screenalloc() == -1) return;	/* allocate new screen buffers */
		screenclear();
		updatescreen();
	}
	/*
	 * Check the bounds for numeric parameters here
	 */
	if (P(P_TS) <= 0 || P(P_TS) > 32) {
		emsg("Invalid tab size specified");
		P(P_TS) = 8;
		return;
	}

	if (P(P_SS) <= 0 || P(P_SS) > Rows) {
		emsg("Invalid scroll size specified");
		P(P_SS) = 12;
		return;
	}

#ifndef	TILDEOP
	if (P(P_TO)) {
		emsg("Tilde-operator not enabled");
		P(P_TO) = false;
		return;
	}
#endif
	/*
	 * Check for another argument, and call doset() recursively, if
	 * found. If any argument results in an error, no further
	 * parameters are processed.
	 */
	while (*arg != ' ' && *arg != '\t') {	/* skip to next white space */
		if (*arg == NUL)
			return;			/* end of parameter list */
		arg++;
	}
	while (*arg == ' ' || *arg == '\t')	/* skip to next non-white */
		arg++;

	if (*arg)
		doset(arg);	/* recurse on next parameter */
}

static	void
showparms(bool all)
{
	register struct	param	*p;
	char	buf[64];

	gotocmd(true, 0);
	outstr("Parameters:\r\n");

	for (p = &params[0]; p->fullname[0] != NUL ;p++) {
		if (!all && ((p->flags & P_CHANGED) == 0))
			continue;
		if (p->flags & P_BOOL)
			sprintf(buf, "\t%s%s\r\n",
				(p->value ? "" : "no"), p->fullname);
		else
			sprintf(buf, "\t%s=%d\r\n", p->fullname, p->value);

		outstr(buf);
	}
	wait_return();
}
