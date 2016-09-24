/*
 * Copyright (c) 1999 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */
#ifndef RLD
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

#include <mach/mach.h>
#include <mach/mach_error.h>

#include <stuff/errors.h>

#if defined(_WIN32) || defined(WIN32)
#include <Windows.h>
#include <io.h>
#include <fcntl.h>

#undef  __private_extern__ 
#define __private_extern__ /*X*/
#endif

#if defined(_WIN32) || defined(WIN32)
uint32_t errors = 0;
#else
__private_extern__ uint32_t errors = 0;	/* number of calls to error() */
#endif

/*
 * Just print the message in the standard format without setting an error.
 */
__private_extern__ void warning(const char *format, ...)
{
    va_list ap;

	va_start(ap, format);
        fprintf(stderr, "warning: %s: ", progname);

#if defined(_MSC_VER)
        _vfprintf_l(stderr, format, NULL, ap);
#else
    	vfprintf(stderr, format, ap);
#endif

        fprintf(stderr, "\n");
	va_end(ap);
}

/*
 * Print the error message and return to the caller after setting the error
 * indication.
 */
__private_extern__
void
error(
const char *format,
...)
{
    va_list ap;

	va_start(ap, format);
    fprintf(stderr, "error: %s: ", progname);

#if defined(_MSC_VER)
    _vfprintf_l(stderr, format, NULL, ap);
#else
	vfprintf(stderr, format, ap);
#endif

    fprintf(stderr, "\n");
	va_end(ap);
	errors++;
}

/*
 * Print the error message, the architecture if not NULL and return to the
 * caller after setting the error indication.
 */
__private_extern__
void
error_with_arch(
const char *arch_name,
const char *format,
...)
{
    va_list ap;

	va_start(ap, format);
        fprintf(stderr, "error: %s: ", progname);
	if(arch_name != NULL)
	    fprintf(stderr, "for architecture: %s ", arch_name);
#if defined(_MSC_VER)
    _vfprintf_l(stderr, format, NULL, ap);
#else
    vfprintf(stderr, format, ap);
#endif
        fprintf(stderr, "\n");
	va_end(ap);
	errors++;
}

/*
 * Print the error message along with the system error message and return to
 * the caller after setting the error indication.
 */
__private_extern__
void
system_error(
const char *format,
...)
{
    va_list ap;
#if defined(_MSC_VER) && __STDC_WANT_SECURE_LIB__
    char errnum[MAX_PATH];
#endif

	va_start(ap, format);
    fprintf(stderr, "error: %s: ", progname);
#if defined(_MSC_VER)
    _vfprintf_l(stderr, format, NULL, ap);
#else
    vfprintf(stderr, format, ap);
#endif

#if defined(_MSC_VER) && __STDC_WANT_SECURE_LIB__
    strerror_s(errnum, sizeof(errnum), errno);
    fprintf(stderr, " (%s)\n", errnum);
#else
    fprintf(stderr, " (%s)\n", strerror(errno));
#endif

	va_end(ap);
	errors++;
}

/*
 * Print the error message along with the mach error string.
 */
__private_extern__
void
my_mach_error(
kern_return_t r,
char *format,
...)
{
    va_list ap;

	va_start(ap, format);
    fprintf(stderr, "error: %s: ", progname);

#if defined(_MSC_VER)
    _vfprintf_l(stderr, format, NULL, ap);
#else
	vfprintf(stderr, format, ap);
#endif

	fprintf(stderr, " (%s)\n", mach_error_string(r));
	va_end(ap);
	errors++;
}
#endif /* !defined(RLD) */
