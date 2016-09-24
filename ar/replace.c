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
/*	$OpenBSD: replace.c,v 1.2 1996/06/26 05:31:23 deraadt Exp $	*/
/*	$NetBSD: replace.c,v 1.6 1995/03/26 03:28:01 glass Exp $	*/

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
static char sccsid[] = "@(#)replace.c	8.3 (Berkeley) 4/2/94";
static char rcsid[] = "$OpenBSD: replace.c,v 1.2 1996/06/26 05:31:23 deraadt Exp $";
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
#include <string.h>

#ifdef __GNUC__
#include <unistd.h>
#endif

#include "archive.h"
#include "extern.h"

/*
 * replace --
 *	Replace or add named members to archive.  Entries already in the
 *	archive are swapped in place.  Others are added before or after 
 *	the key entry, based on the a, b and i options.  If the u option
 *	is specified, modification dates select for replacement.
 */
int replace(char **argv)
{
	char *file;
	int afd, curfd, errflg, exists, mods, sfd, tfd1, tfd2;
#if defined(_WIN32) || defined(WIN32)
    struct _stat sb;
#else
	struct stat sb;
#endif
	CF cf;
	off_t size, tsize;

	tsize = 0;
	errflg = 0;
	/*
	 * If doesn't exist, simply append to the archive.  There's
	 * a race here, but it's pretty short, and not worth fixing.
	 */
	exists = !_stat(archive, &sb);
	afd = open_archive(O_CREAT|O_RDWR);

	if (!exists) {
		tfd1 = -1;
		tfd2 = tmp();
		goto append;
	} 

	tfd1 = tmp();			/* Files before key file. */
	tfd2 = tmp();			/* Files after key file. */

	/*
	 * Break archive into two parts -- entries before and after the key
	 * entry.  If positioning before the key, place the key at the
	 * beginning of the after key entries and if positioning after the
	 * key, place the key at the end of the before key entries.  Put it
	 * all back together at the end.
	 */
	mods = (options & (AR_A|AR_B));
	for (curfd = tfd1; get_arobj(afd);) {
        file = files(argv);
		if (*argv && file) {
#if defined(_MSC_VER) && __STDC_WANT_SECURE_LIB__
            _sopen_s(&sfd, file, O_RDONLY, SH_DENYNO, 0);
            if (sfd < 0) {
#else
			if ((sfd = _open(file, O_RDONLY)) < 0) {
#endif
				errflg = 1;
				warn("%s", file);
				goto useold;
			}
			(void)_fstat(sfd, &sb);
			if (options & AR_U && sb.st_mtime <= chdr.date) {
				_close(sfd);
				goto useold;
			}

			if (options & AR_V)
			     (void)printf("r - %s\n", file);

			/* Read from disk, write to an archive; pad on write */
			SETCF(sfd, file, curfd, tname, WPAD);
			put_arobj(&cf, &sb);
			(void)_close(sfd);
			skip_arobj(afd);
			continue;
		}

		if (mods && compare(posname)) {
			mods = 0;
			if (options & AR_B)
				curfd = tfd2;
			/* Read and write to an archive; pad on both. */
			SETCF(afd, archive, curfd, tname, RPAD|WPAD);
#if defined(_WIN32) || defined(WIN32)
            put_arobj(&cf, (struct _stat *)NULL);
#else
			put_arobj(&cf, (struct stat *)NULL);
#endif
			if (options & AR_A)
				curfd = tfd2;
		} else {
			/* Read and write to an archive; pad on both. */
useold:			SETCF(afd, archive, curfd, tname, RPAD|WPAD);
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

	/* Append any left-over arguments to the end of the after file. */
append:	while ((file = *argv++) != NULL) {
		if (options & AR_V)
			(void)printf("a - %s\n", file);
#if defined(_MSC_VER) && __STDC_WANT_SECURE_LIB__
        _sopen_s(&sfd, file, O_RDONLY, SH_DENYNO, 0);
        if (sfd < 0) {
#else
		if ((sfd = _open(file, O_RDONLY)) < 0) {
#endif
			errflg = 1;
			warn("%s", file);
			continue;
		}
		(void)_fstat(sfd, &sb);
		/* Read from disk, write to an archive; pad on write. */
		SETCF(sfd, file,
		    options & (AR_A|AR_B) ? tfd1 : tfd2, tname, WPAD);
		put_arobj(&cf, &sb);
		(void)_close(sfd);
	}
	
	(void)_lseek(afd, (off_t)SARMAG, SEEK_SET);

	SETCF(tfd1, tname, afd, archive, NOPAD);
	if (tfd1 != -1) {
		tsize = size = _lseek(tfd1, (off_t)0, SEEK_CUR);
		(void)_lseek(tfd1, (off_t)0, SEEK_SET);
		copy_ar(&cf, size);
	} else
		tsize = 0;

	tsize += size = _lseek(tfd2, (off_t)0, SEEK_CUR);
	(void)_lseek(tfd2, (off_t)0, SEEK_SET);
	cf.rfd = tfd2;
	copy_ar(&cf, size);

#if !defined(_WIN32) && !defined(WIN32)
    (void)ftruncate(afd, tsize + SARMAG);
#endif

	close_archive(afd);
	return (errflg);
}	
