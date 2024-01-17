#include <stdlib.h>

#define NULLCH	'\0'

size_t strcspn(const char *str1, const char *str2);
static int contains(char c, const char *s);

size_t
strcspn(const char *str1, const char *str2)
{
	size_t n = 0;

	while (*str1 != NULLCH && !contains(*str1, str2)) {
		++n;
		++str1;
	}

	return n;
}

static int
contains(char c, const char *s)
{
	while (*s != NULLCH)
		if (*s == c)
			return 1;
		else
			++s;

	return 0;
}
