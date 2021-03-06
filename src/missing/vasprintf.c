/* Like vsprintf but provides a pointer to malloc'd storage, which must
	 be freed by the caller.
	 Copyright (C) 1994 Free Software Foundation, Inc.

This file is part of the libiberty library.
Libiberty is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

Libiberty is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with libiberty; see the file COPYING.LIB.	If
not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.	*/

#include <config.h>

#ifndef HAVE_VASPRINTF

#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int int_vasprintf(char **result, char *format, va_list args) {
	char *p = format;
	/*
	** Add one to make sure that it is never zero, which might cause malloc
	** to return NULL.
	*/
	int total_width = strlen(format) + 1;
	va_list ap;

#ifdef WIN32
	memcpy(&ap, &args, sizeof(va_list));
#else
	/* Does this actually work? */
	memcpy(&ap, args, sizeof(va_list));
#endif

	while (*p != '\0') {
		if (*p++ == '%') {
			while (strchr("-+ #0", *p))
				++p;
				if (*p == '*') {
					++p;
					total_width += abs(va_arg(ap, int));
				} else
					total_width += strtoul(p, &p, 10);

				if (*p == '.') {
					++p;
					if (*p == '*') {
						++p;
						total_width += abs(va_arg(ap, int));
					} else
						total_width += strtoul(p, &p, 10);
				}

				while (strchr("hlL", *p))
					++p;
				/*
				** Should be big enough for any format
				** specifier except %s and floats.
				*/
				total_width += 30;

				switch (*p) {
					case 'd':
					case 'i':
					case 'o':
					case 'u':
					case 'x':
					case 'X':
					case 'c':
						(void) va_arg(ap, int);
						break;

					case 'f':
					case 'e':
					case 'E':
					case 'g':
					case 'G':
						(void) va_arg(ap, double);
						/*
						** Since an ieee double can have an exponent of 307,
						** we'll make the buffer wide enough to cover
						** the gross case.
						*/
						total_width += 307;
						break;

					case 's':
						total_width += strlen(va_arg(ap, char *));
						break;

					case 'p':
					case 'n':
						(void) va_arg(ap, char *);
						break;
				}
			}
		}

		*result = malloc(total_width);
		if (*result != NULL)
			return (vsprintf(*result, format, args));
		else
			return (0);
}

int vasprintf(char **strp, const char *fmt, va_list ap) {
	return (int_vasprintf(strp, (char *) fmt, ap));
}

#endif
