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
/*	$OpenBSD: extract.c,v 1.2 1996/06/26 05:31:20 deraadt Exp $	*/
/*	$NetBSD: extract.c,v 1.5 1995/03/26 03:27:53 glass Exp $	*/

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
static char sccsid[] = "@(#)extract.c	8.3 (Berkeley) 4/2/94";
static char rcsid[] = "$OpenBSD: extract.c,v 1.2 1996/06/26 05:31:20 deraadt Exp $";
#endif
#endif /* not lint */

#include <stuff/bool.h>

#if defined(_WIN32) || defined(WIN32)
#include <stdint.h>
#include <sys/types.h>
#include <Windows.h>
#include <io.h>

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
#include <sys/time.h>
#include <dirent.h>
#include <err.h>
#endif

#include <sys/stat.h>

#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#ifdef __GNUC__
#include <unistd.h>
#endif

#include "archive.h"
#include "extern.h"

/*
 * extract --
 *	Extract files from the named archive - if member names given only
 *	extract those members otherwise extract all members.  If 'o' option
 *	selected modify date of newly created file to be same as archive
 *	members date otherwise date is time of extraction.  Does not modify
 *	archive.
 */
int extract(char **argv)
{
    char *file = NULL;
    int afd, all, eval, tfd;
#if defined(_WIN32) || defined(WIN32)
    struct utimbuf tv;
#else
    struct timeval tv[2];
#endif
#if defined(_WIN32) || defined(WIN32)
    struct _stat sb;
#else
    struct stat sb;
#endif
    CF cf;

    eval = 0;
#if defined(_WIN32) || defined(WIN32)
    tv.actime = 0;
    tv.modtime = 0;
#else
    tv[0].tv_usec = tv[1].tv_usec = 0;
#endif

    afd = open_archive(O_RDONLY);

    /* Read from an archive, write to disk; pad on read. */
    SETCF(afd, archive, 0, 0, RPAD);
    for (all = !*argv; get_arobj(afd);) {
        if (all)
            file = chdr.name;
        else {
            file = files(argv);
            if (!file) {
                skip_arobj(afd);
                continue;
            }
        }

#if defined(_WIN32) || defined(WIN32)
        if (options & AR_U && !_stat(file, &sb) &&
#else
        if (options & AR_U && !stat(file, &sb) &&
#endif
            (sb.st_mtime > chdr.date))
            continue;

#if defined(_MSC_VER) && __STDC_WANT_SECURE_LIB__
        _sopen_s(&tfd, file, O_WRONLY | O_CREAT | O_TRUNC, S_IWUSR, chdr.mode);
        if (tfd < 0) {
#else
        if ((tfd = _sopen(file, O_WRONLY | O_CREAT | O_TRUNC, S_IWUSR, chdr.mode)) < 0) {
#endif
            warn("%s", file);
            skip_arobj(afd);
            eval = 1;
            continue;
        }

        if (options & AR_V)
            (void)printf("x - %s\n", file);

        cf.wfd = tfd;
        cf.wname = file;

        copy_ar(&cf, chdr.size);

#if !defined(_WIN32) && !defined(WIN32)
        if (fchmod(tfd, (short)chdr.mode)) {
            warn("chmod: %s", file);

            eval = 1;
        }
#endif

        if (options & AR_O) {
#if defined(_WIN32) || defined(WIN32)
            tv.actime = chdr.date;
            tv.modtime = chdr.date;

            if (utime(file, &tv)) {
#else
            tv[0].tv_sec = tv[1].tv_sec = chdr.date;

            if (utimes(file, tv)) {
#endif
                warn("utimes: %s", file);

                eval = 1;
            }
        }

        (void)_close(tfd);

        if (!all && !*argv)
            break;
    }

    close_archive(afd);

    if (*argv) {
        orphans(argv);

        return (1);
    }

    return (0);
}

