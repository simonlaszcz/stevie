/*
 * STevie - ST editor for VI enthusiasts.    ...Tim Thompson...twitch!tjt...
 */

#include <stdio.h>
#include "stevie.h"
#include "hexchars.h"

/*
 * This stuff is used to provide readable interpretations of unprintable
 * characters.  The 'xxx' in each is replaced with either hex, octal, or
 * decimal numbers, depending on what command-line argument is given.
 */

struct charinfo chars[] = {
	/* 000 */	5, "[xxx]",
	/* 001 */	5, "[xxx]",
	/* 002 */	5, "[xxx]",
	/* 003 */	5, "[xxx]",
	/* 004 */	5, "[xxx]",
	/* 005 */	5, "[xxx]",
	/* 006 */	5, "[xxx]",
	/* 007 */	5, "[xxx]",
	/* 010 */	5, "[xxx]",
	/* 011 */	5, "[xxx]",
	/* 012 */	1, NULL,
	/* 013 */	5, "[xxx]",
	/* 014 */	5, "[xxx]",
	/* 015 */	5, "[xxx]",
	/* 016 */	5, "[xxx]",
	/* 017 */	5, "[xxx]",
	/* 020 */	5, "[xxx]",
	/* 021 */	5, "[xxx]",
	/* 022 */	5, "[xxx]",
	/* 023 */	5, "[xxx]",
	/* 024 */	5, "[xxx]",
	/* 025 */	5, "[xxx]",
	/* 026 */	5, "[xxx]",
	/* 027 */	5, "[xxx]",
	/* 030 */	5, "[xxx]",
	/* 031 */	5, "[xxx]",
	/* 032 */	5, "[xxx]",
	/* 033 */	5, "[xxx]",
	/* 034 */	5, "[xxx]",
	/* 035 */	5, "[xxx]",
	/* 036 */	5, "[xxx]",
	/* 037 */	5, "[xxx]",
	/* 040 */	1, NULL,
	/* 041 */	1, NULL,
	/* 042 */	1, NULL,
	/* 043 */	1, NULL,
	/* 044 */	1, NULL,
	/* 045 */	1, NULL,
	/* 046 */	1, NULL,
	/* 047 */	1, NULL,
	/* 050 */	1, NULL,
	/* 051 */	1, NULL,
	/* 052 */	1, NULL,
	/* 053 */	1, NULL,
	/* 054 */	1, NULL,
	/* 055 */	1, NULL,
	/* 056 */	1, NULL,
	/* 057 */	1, NULL,
	/* 060 */	1, NULL,
	/* 061 */	1, NULL,
	/* 062 */	1, NULL,
	/* 063 */	1, NULL,
	/* 064 */	1, NULL,
	/* 065 */	1, NULL,
	/* 066 */	1, NULL,
	/* 067 */	1, NULL,
	/* 070 */	1, NULL,
	/* 071 */	1, NULL,
	/* 072 */	1, NULL,
	/* 073 */	1, NULL,
	/* 074 */	1, NULL,
	/* 075 */	1, NULL,
	/* 076 */	1, NULL,
	/* 077 */	1, NULL,
	/* 100 */	1, NULL,
	/* 101 */	1, NULL,
	/* 102 */	1, NULL,
	/* 103 */	1, NULL,
	/* 104 */	1, NULL,
	/* 105 */	1, NULL,
	/* 106 */	1, NULL,
	/* 107 */	1, NULL,
	/* 110 */	1, NULL,
	/* 111 */	1, NULL,
	/* 112 */	1, NULL,
	/* 113 */	1, NULL,
	/* 114 */	1, NULL,
	/* 115 */	1, NULL,
	/* 116 */	1, NULL,
	/* 117 */	1, NULL,
	/* 120 */	1, NULL,
	/* 121 */	1, NULL,
	/* 122 */	1, NULL,
	/* 123 */	1, NULL,
	/* 124 */	1, NULL,
	/* 125 */	1, NULL,
	/* 126 */	1, NULL,
	/* 127 */	1, NULL,
	/* 130 */	1, NULL,
	/* 131 */	1, NULL,
	/* 132 */	1, NULL,
	/* 133 */	1, NULL,
	/* 134 */	1, NULL,
	/* 135 */	1, NULL,
	/* 136 */	1, NULL,
	/* 137 */	1, NULL,
	/* 140 */	1, NULL,
	/* 141 */	1, NULL,
	/* 142 */	1, NULL,
	/* 143 */	1, NULL,
	/* 144 */	1, NULL,
	/* 145 */	1, NULL,
	/* 146 */	1, NULL,
	/* 147 */	1, NULL,
	/* 150 */	1, NULL,
	/* 151 */	1, NULL,
	/* 152 */	1, NULL,
	/* 153 */	1, NULL,
	/* 154 */	1, NULL,
	/* 155 */	1, NULL,
	/* 156 */	1, NULL,
	/* 157 */	1, NULL,
	/* 160 */	1, NULL,
	/* 161 */	1, NULL,
	/* 162 */	1, NULL,
	/* 163 */	1, NULL,
	/* 164 */	1, NULL,
	/* 165 */	1, NULL,
	/* 166 */	1, NULL,
	/* 167 */	1, NULL,
	/* 170 */	1, NULL,
	/* 171 */	1, NULL,
	/* 172 */	1, NULL,
	/* 173 */	1, NULL,
	/* 174 */	1, NULL,
	/* 175 */	1, NULL,
	/* 176 */	1, NULL,
	/* 177 */	5, "[xxx]",
	/* 200 */	5, "[xxx]",
	/* 201 */	5, "[xxx]",
	/* 202 */	5, "[xxx]",
	/* 203 */	5, "[xxx]",
	/* 204 */	5, "[xxx]",
	/* 205 */	5, "[xxx]",
	/* 206 */	5, "[xxx]",
	/* 207 */	5, "[xxx]",
	/* 210 */	5, "[xxx]",
	/* 211 */	5, "[xxx]",
	/* 212 */	5, "[xxx]",
	/* 213 */	5, "[xxx]",
	/* 214 */	5, "[xxx]",
	/* 215 */	5, "[xxx]",
	/* 216 */	5, "[xxx]",
	/* 217 */	5, "[xxx]",
	/* 220 */	5, "[xxx]",
	/* 221 */	5, "[xxx]",
	/* 222 */	5, "[xxx]",
	/* 223 */	5, "[xxx]",
	/* 224 */	5, "[xxx]",
	/* 225 */	5, "[xxx]",
	/* 226 */	5, "[xxx]",
	/* 227 */	5, "[xxx]",
	/* 230 */	5, "[xxx]",
	/* 231 */	5, "[xxx]",
	/* 232 */	5, "[xxx]",
	/* 233 */	5, "[xxx]",
	/* 234 */	5, "[xxx]",
	/* 235 */	5, "[xxx]",
	/* 236 */	5, "[xxx]",
	/* 237 */	5, "[xxx]",
	/* 240 */	5, "[xxx]",
	/* 241 */	5, "[xxx]",
	/* 242 */	5, "[xxx]",
	/* 243 */	5, "[xxx]",
	/* 244 */	5, "[xxx]",
	/* 245 */	5, "[xxx]",
	/* 246 */	5, "[xxx]",
	/* 247 */	5, "[xxx]",
	/* 250 */	5, "[xxx]",
	/* 251 */	5, "[xxx]",
	/* 252 */	5, "[xxx]",
	/* 253 */	5, "[xxx]",
	/* 254 */	5, "[xxx]",
	/* 255 */	5, "[xxx]",
	/* 256 */	5, "[xxx]",
	/* 257 */	5, "[xxx]",
	/* 260 */	5, "[xxx]",
	/* 261 */	5, "[xxx]",
	/* 262 */	5, "[xxx]",
	/* 263 */	5, "[xxx]",
	/* 264 */	5, "[xxx]",
	/* 265 */	5, "[xxx]",
	/* 266 */	5, "[xxx]",
	/* 267 */	5, "[xxx]",
	/* 270 */	5, "[xxx]",
	/* 271 */	5, "[xxx]",
	/* 272 */	5, "[xxx]",
	/* 273 */	5, "[xxx]",
	/* 274 */	5, "[xxx]",
	/* 275 */	5, "[xxx]",
	/* 276 */	5, "[xxx]",
	/* 277 */	5, "[xxx]",
	/* 300 */	5, "[xxx]",
	/* 301 */	5, "[xxx]",
	/* 302 */	5, "[xxx]",
	/* 303 */	5, "[xxx]",
	/* 304 */	5, "[xxx]",
	/* 305 */	5, "[xxx]",
	/* 306 */	5, "[xxx]",
	/* 307 */	5, "[xxx]",
	/* 310 */	5, "[xxx]",
	/* 311 */	5, "[xxx]",
	/* 312 */	5, "[xxx]",
	/* 313 */	5, "[xxx]",
	/* 314 */	5, "[xxx]",
	/* 315 */	5, "[xxx]",
	/* 316 */	5, "[xxx]",
	/* 317 */	5, "[xxx]",
	/* 320 */	5, "[xxx]",
	/* 321 */	5, "[xxx]",
	/* 322 */	5, "[xxx]",
	/* 323 */	5, "[xxx]",
	/* 324 */	5, "[xxx]",
	/* 325 */	5, "[xxx]",
	/* 326 */	5, "[xxx]",
	/* 327 */	5, "[xxx]",
	/* 330 */	5, "[xxx]",
	/* 331 */	5, "[xxx]",
	/* 332 */	5, "[xxx]",
	/* 333 */	5, "[xxx]",
	/* 334 */	5, "[xxx]",
	/* 335 */	5, "[xxx]",
	/* 336 */	5, "[xxx]",
	/* 337 */	5, "[xxx]",
	/* 340 */	5, "[xxx]",
	/* 341 */	5, "[xxx]",
	/* 342 */	5, "[xxx]",
	/* 343 */	5, "[xxx]",
	/* 344 */	5, "[xxx]",
	/* 345 */	5, "[xxx]",
	/* 346 */	5, "[xxx]",
	/* 347 */	5, "[xxx]",
	/* 350 */	5, "[xxx]",
	/* 351 */	5, "[xxx]",
	/* 352 */	5, "[xxx]",
	/* 353 */	5, "[xxx]",
	/* 354 */	5, "[xxx]",
	/* 355 */	5, "[xxx]",
	/* 356 */	5, "[xxx]",
	/* 357 */	5, "[xxx]",
	/* 360 */	5, "[xxx]",
	/* 361 */	5, "[xxx]",
	/* 362 */	5, "[xxx]",
	/* 363 */	5, "[xxx]",
	/* 364 */	5, "[xxx]",
	/* 365 */	5, "[xxx]",
	/* 366 */	5, "[xxx]",
	/* 367 */	5, "[xxx]",
	/* 370 */	5, "[xxx]",
	/* 371 */	5, "[xxx]",
	/* 372 */	5, "[xxx]",
	/* 373 */	5, "[xxx]",
	/* 374 */	5, "[xxx]",
	/* 375 */	5, "[xxx]",
	/* 376 */	5, "[xxx]",
	/* 377 */	5, "[xxx]",
};

/*
 * octchars()
 *
 * Convert the charinfo strings to octal.
 */
void
octchars(void)
{
	char *p;
	int n;

	for ( n=0; n<256; n++ ) {
		p = chars[n].ch_str;
		if ( p==NULL || *p != '[' )
			continue;
		sprintf(++p,"%03o]",n);
	}
}

/*
 * hexchars()
 *
 * Convert the charinfo strings to hex.
 */
void
hexchars(void)
{
	char *p;
	int n;

	for ( n=0; n<256; n++ ) {
		p = chars[n].ch_str;
		if ( p==NULL || *p != '[' )
			continue;
		sprintf(++p,"x%02X]",n);
	}
}

/*
 * decchars()
 *
 * Convert the charinfo strings to decimal.
 */
void
decchars(void)
{
	char *p;
	int n;

	for ( n=0; n<256; n++ ) {
		p = chars[n].ch_str;
		if ( p==NULL || *p != '[' )
			continue;
		sprintf(++p,"%3d]",n);
	}
}

int
hextoint(int c)
{
	if ( c>='0' && c<='9' )
		return(c-'0');
	if ( c>='a' && c<='f' )
		return(10+c-'a');
	if ( c>='A' && c<='F' )
		return(10+c-'A');
	return(-1);
}
