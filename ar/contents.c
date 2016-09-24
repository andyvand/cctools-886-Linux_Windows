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
/*	$OpenBSD: contents.c,v 1.2 1996/06/26 05:31:19 deraadt Exp $	*/
/*	$NetBSD: contents.c,v 1.5 1995/03/26 03:27:49 glass Exp $	*/

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
static char sccsid[] = "@(#)contents.c	8.3 (Berkeley) 4/2/94";
static char rcsid[] = "$OpenBSD: contents.c,v 1.2 1996/06/26 05:31:19 deraadt Exp $";
#endif
#endif /* not lint */

#if !defined(_WIN32) && !defined(WIN32)
#include <sys/param.h>
#include <sys/time.h>
#else
#include <stdint.h>
#include <Windows.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <sys/timeb.h>
#include <sys/utime.h>
#endif

#include <sys/stat.h>

#include <ar.h>

#if defined(_WIN32) || defined(WIN32)
#include <direct.h>

#include "../winsuppl/dirent.h"
#else
#include <dirent.h>
#endif

#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#if defined(_WIN32) || defined(WIN32)
#include <time.h>
#include <sys/timeb.h>
#include <sys/utime.h>
#elif defined(__linux__)
#include <time.h>
#include <sys/timeb.h>
#include <utime.h>
#else
#include <tzfile.h>
#endif

#ifdef __GNUC__
#include <unistd.h>
#endif


#include "archive.h"
#include "extern.h"

/*
 * contents --
 *	Handles t[v] option - opens the archive and then reads headers,
 *	skipping member contents.
 */
int contents(char **argv)
{
	int afd, all;
#if defined(_MSC_VER) && __STDC_WANT_SECURE_LIB__
    struct tm tp;
#else
    struct tm *tp;
#endif
	char *file, buf[25];
	
	afd = open_archive(O_RDONLY);

    for (all = !*argv; get_arobj(afd);) {
        if (all)
            file = chdr.name;
        else {
            file = files(argv);

            if (!file)
                goto next;
        }
            if (options & AR_V) {
#ifndef __linux__
#if defined(_WIN32) || defined(WIN32)
                        _set_fmode(chdr.mode);
#else
			(void)strmode(chdr.mode, buf);
#endif
#endif /* __linux__ */

#if defined(_MSC_VER)
			(void)printf("%s %6d/%-6d %8lld ", buf + 1,
                         chdr.uid, chdr.gid, (__int64)chdr.size);
#else
			(void)printf("%s %6d/%-6d %8lld ", buf + 1,
                         chdr.uid, chdr.gid, (long long)chdr.size);
#endif

#if defined(_MSC_VER) && __STDC_WANT_SECURE_LIB__
            localtime_s(&tp, &chdr.date);
#else
			tp = localtime(&chdr.date);
#endif

#if defined(_MSC_VER) && __STDC_WANT_SECURE_LIB__
			(void)strftime(buf, sizeof(buf), "%b %e %H:%M %Y", &tp);
#else
            (void)strftime(buf, sizeof(buf), "%b %e %H:%M %Y", tp);
#endif

			(void)printf("%s %s\n", buf, file);
		} else
			(void)printf("%s\n", file);
		if (!all && !*argv)
			break;
next:		skip_arobj(afd);
	}

	close_archive(afd);

	if (*argv) {
		orphans(argv);

		return (1);
	}
	return (0);
}

