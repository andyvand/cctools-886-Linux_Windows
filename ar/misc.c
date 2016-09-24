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
/*	$OpenBSD: misc.c,v 1.2 1996/06/26 05:31:21 deraadt Exp $	*/
/*	$NetBSD: misc.c,v 1.6 1995/03/26 03:27:55 glass Exp $	*/

/*-
 * Copyright (c) 1990, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Hugh Smith at The University of Guelph.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef lint
#if 0
static char sccsid[] = "@(#)misc.c	8.3 (Berkeley) 4/2/94";
static char rcsid[] = "$OpenBSD: misc.c,v 1.2 1996/06/26 05:31:21 deraadt Exp $";
#endif
#endif /* not lint */

#if defined(_WIN32) || defined(WIN32)
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <Windows.h>
#include <io.h>
#include <fcntl.h>

#include <errno.h>
#include <winerror.h>
#include <time.h>
#include <sys/timeb.h>
#include <sys/utime.h>

#include "../winsuppl/dirent.h"

#ifndef warn
#define warn printf
#endif

#ifndef warnx
#define warnx printf
#endif
#else
#include <sys/param.h>
#include <dirent.h>
#include <err.h>
#endif

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __GNUC__
#include <unistd.h>
#endif

#include <stuff/bool.h>

#include "archive.h"
#include "extern.h"
#include "pathnames.h"

char *tname = "temporary file";		/* temporary file "name" */

#ifndef sigset_t
#define sigset_t int
#endif /* sigset_t */

int tmp(void)
{
	char *envtmp = NULL;
#if defined(_MSC_VER) && __STDC_WANT_SECURE_LIB__
   	size_t envtmpsz = 0;
#endif
#if defined(__APPLE__) && !defined(_WIN32) && !defined(WIN32)
    	sigset_t set;
	sigset_t oset;
#endif
	static int first = 0;
	int fd = 0;
#if defined(_WIN32) || defined(WIN32)
	char path[MAX_PATH];
#else
	char path[MAXPATHLEN];
#endif

	if (!first && !envtmp) {
#if defined(_WIN32) || defined(WIN32)
#define TMPDIR "TEMP"
#else
#define TMPDIR "TMPDIR"
#endif

#if defined(_MSC_VER) && __STDC_WANT_SECURE_LIB__
        	_dupenv_s(&envtmp, &envtmpsz, TMPDIR);
#else
		envtmp = getenv("TMPDIR");
#endif

		first = 1;
	}

	if (envtmp)
	{
#if defined(_MSC_VER) && __STDC_WANT_SECURE_LIB__
	        (void)sprintf_s(path, sizeof(path), "%s/%s", envtmp, _NAME_ARTMP);
#else
	        (void)snprintf(path, sizeof(path), "%s/%s", envtmp, _NAME_ARTMP);
#endif
    	} else {
#if defined(_MSC_VER) && __STDC_WANT_SECURE_LIB__
        	strcpy_s(path, sizeof(path), _PATH_ARTMP);
#else
		strncpy(path, _PATH_ARTMP, sizeof(path));
#endif
	}

#if defined(__APPLE__) && !defined(_WIN32) && !defined(WIN32)
	sigfillset(&set);
	(void)sigprocmask(SIG_BLOCK, &set, &oset);
#endif

#if defined(_MSC_VER) && __STDC_WANT_SECURE_LIB__
	_mktemp_s(path, sizeof(path));
	_sopen_s(&fd, path, O_RDWR, SH_DENYNO, 0);

 	if (fd == -1)
#else
	if ((fd = mkstemp(path)) == -1)
#endif
		(void)_unlink(path);

#if defined(__APPLE__) && !defined(_WIN32) && !defined(WIN32)
	(void)sigprocmask(SIG_SETMASK, &oset, NULL);
#endif

	return (fd);
}

/*
 * files --
 *	See if the current file matches any file in the argument list; if it
 * 	does, remove it from the argument list.
 */
char *files(char **argv)
{
	char **list, *p;

	for (list = argv; *list; ++list)
		if (compare(*list)) {
			p = *list;
			for (; (list[0] = list[1]) != NULL; ++list)
				continue;
			return (p);
		}
	return (NULL);
}

void orphans(char **argv)
{
	for (; *argv; ++argv)
	{
		warnx("%s: not found in archive", *argv);
	}
}

char *rname(char *path)
{
	char *ind = strrchr(path, '/');

	return ind ? ind + 1 : path;
}

int compare(char *dest)
{
	if (options & AR_TR)
	{
		return (!strncmp(chdr.name, rname(dest), OLDARMAXNAME));
	}

	return (!strcmp(chdr.name, rname(dest)));
}

void badfmt(void)
{
#if defined(_WIN32) || defined(WIN32)
    errno = ENFILE;

    fprintf(stderr, "%s", archive);
#elif defined(__linux__)
    errno = EBADFD;

    err(1, "%s", archive);
#else
    errno = EFTYPE;

    err(1, "%s", archive);
#endif
}

void error(char *name)
{
#if defined(_WIN32) || defined(WIN32)
    fprintf(stderr, "%s", name);
#else
    err(1, "%s", name);
#endif
}

