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
/*	$OpenBSD: move.c,v 1.2 1996/06/26 05:31:21 deraadt Exp $	*/
/*	$NetBSD: move.c,v 1.5 1995/03/26 03:27:57 glass Exp $	*/

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
static char sccsid[] = "@(#)move.c	8.3 (Berkeley) 4/2/94";
static char rcsid[] = "$OpenBSD: move.c,v 1.2 1996/06/26 05:31:21 deraadt Exp $";
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
#include <dirent.h>
#include <err.h>
#endif

#include <sys/stat.h>

#include <ar.h>
#include <fcntl.h>
#include <stdio.h>

#ifdef __GNUC__
#include <unistd.h>
#endif

#include "archive.h"
#include "extern.h"
#include "pathnames.h"

/*
 * move --
 *	Change location of named members in archive - if 'b' or 'i' option
 *	selected then named members are placed before 'posname'.  If 'a'
 *	option selected members go after 'posname'.  If no options, members
 *	are moved to end of archive.
 */
int move(char **argv)
{
	CF cf;
	off_t size, tsize;
	int afd, curfd, mods, tfd1, tfd2, tfd3;
	char *file;

	afd = open_archive(O_RDWR);
	mods = options & (AR_A|AR_B);

	tfd1 = tmp();			/* Files before key file. */
	tfd2 = tmp();			/* Files selected by user. */
	tfd3 = tmp();			/* Files after key file. */

	/*
	 * Break archive into three parts -- selected entries and entries
	 * before and after the key entry.  If positioning before the key,
	 * place the key at the beginning of the after key entries and if
	 * positioning after the key, place the key at the end of the before
	 * key entries.  Put it all back together at the end.
	 */

	/* Read and write to an archive; pad on both. */
	SETCF(afd, archive, 0, tname, RPAD|WPAD);
	for (curfd = tfd1; get_arobj(afd);) {
        file = files(argv);
		if (*argv && file) {
			if (options & AR_V)
				(void)printf("m - %s\n", file);
			cf.wfd = tfd2;
#if defined(_WIN32) || defined(WIN32)
            put_arobj(&cf, (struct _stat *)NULL);
#else
            put_arobj(&cf, (struct stat *)NULL);
#endif
			continue;
		}
		if (mods && compare(posname)) {
			mods = 0;
			if (options & AR_B)
				curfd = tfd3;
			cf.wfd = curfd;

#if defined(_WIN32) || defined(WIN32)
            put_arobj(&cf, (struct _stat *)NULL);
#else
			put_arobj(&cf, (struct stat *)NULL);
#endif

			if (options & AR_A)
				curfd = tfd3;
		} else {
			cf.wfd = curfd;

#if defined(_WIN32) || defined(WIN32)
            put_arobj(&cf, (struct _stat *)NULL);
#else
			put_arobj(&cf, (struct stat *)NULL);
#endif
		}
	}

	if (mods) {
		warnx("%s: archive member not found", posarg);
		close_archive(afd);
		return (1);
	}
	(void)_lseek(afd, (off_t)SARMAG, SEEK_SET);

	SETCF(tfd1, tname, afd, archive, NOPAD);
	tsize = size = _lseek(tfd1, (off_t)0, SEEK_CUR);
	(void)_lseek(tfd1, (off_t)0, SEEK_SET);
	copy_ar(&cf, size);

	tsize += size = _lseek(tfd2, (off_t)0, SEEK_CUR);
	(void)_lseek(tfd2, (off_t)0, SEEK_SET);
	cf.rfd = tfd2;
	copy_ar(&cf, size);

	tsize += size = _lseek(tfd3, (off_t)0, SEEK_CUR);
	(void)_lseek(tfd3, (off_t)0, SEEK_SET);
	cf.rfd = tfd3;
	copy_ar(&cf, size);

#if !defined(_WIN32) && !defined(WIN32)
	(void)ftruncate(afd, tsize + SARMAG);
#endif

	close_archive(afd);

	if (*argv) {
		orphans(argv);

		return (1);
	}
	return (0);
}	
