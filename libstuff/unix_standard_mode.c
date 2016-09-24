/*
 * Copyright (c) 2004 Apple Computer, Inc. All rights reserved.
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
#include <sys/types.h>
#include <stdlib.h>
#if defined(_MSC_VER)
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <Windows.h>

#ifndef bool
#define bool unsigned char
#endif
#else
#include <strings.h>
#endif

#include "stuff/bool.h"
#include "stuff/unix_standard_mode.h"

/*
 * get_unix_standard_mode() returns TRUE if we are running in UNIX standard
 * command mode (the default).
 */
__private_extern__
unsigned char
get_unix_standard_mode(
void)
{
#if defined(_WIN32) || defined(WIN32)
    static bool checked_environment_variable = 0;
    static bool unix_standard_mode = 1;
#else
    static unsigned char checked_environment_variable = FALSE;
    static unsigned char unix_standard_mode = TRUE;
#endif
    char *p;
#if defined(_MSC_VER) && __STDC_WANT_SECURE_LIB__
    size_t plen;
#endif

#if defined(_WIN32) || defined(WIN32)
    if (checked_environment_variable == 0) {
        checked_environment_variable = 1;
#else
    if (checked_environment_variable == FALSE) {
        checked_environment_variable = TRUE;
#endif
	    /*
	     * Pick up the UNIX standard command mode environment variable.
	     */
#if defined(_MSC_VER) && __STDC_WANT_SECURE_LIB__
        _dupenv_s(&p, &plen, "COMMAND_MODE");
#else
	    p = getenv("COMMAND_MODE");
#endif
	    if(p != NULL){
#if defined(_WIN32) || defined(WIN32)
            if (_stricmp("legacy", p) == 0)
                unix_standard_mode = 0;
#else
            if (strcasecmp("legacy", p) == 0)
                unix_standard_mode = FALSE;
#endif
	    }
	}
	return(unix_standard_mode);
}
#endif /* !defined(RLD) */
