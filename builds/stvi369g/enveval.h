/*
 *	Evaluate a string, expanding environment variables
 *	where encountered.
 *	We'll use the UNIX convention for representing environment
 *	variables: $xxx, where xxx is the shortest string that
 *	matches some environment variable.
 */

#ifndef ENVEVAL_H
#define ENVEVAL_H

/**
 *  s=	Pointer to buffer, currently containing string.  It will be
 *	expanded in-place in the buffer.
 *  len=Maximum allowable length of the buffer.  (In this version, we
 *	use a static buffer of 256 bytes internally.)
 *
 * RETURNS:
 *	 0	on success.
 *	-1	on failure.  In this case, s may contain a partially
 *			converted string, but it won't contain a partial
 *			string.  It will be the FULL string, with as
 *			many substitutions as we could find.
 */
int EnvEval(char *s, int len);

#endif
