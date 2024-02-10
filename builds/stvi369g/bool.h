#ifndef BOOL_H
#define BOOL_H

#ifdef __PUREC__
#define bool 	int
#define true	(1)
#define false	(0)
#else
#include <stdbool.h>
#endif

#endif