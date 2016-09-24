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
#if defined(_MSC_VER)
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <windows.h>
#include <stdio.h>
#include <io.h>
#elif defined(__linux__)
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#else
#include <libc.h> /* first to get rid of pre-comp warning */
#endif

#include <mach/mach.h> /* first to get rid of pre-comp warning */

#include "stdio.h"

#include <signal.h>

#if !defined(_WIN32) && !defined(WIN32)
#include <sys/wait.h>
#include <sys/file.h>
#else
#include <Windows.h>
#include <fileapi.h>
#endif

#include <errno.h>

#include "stuff/errors.h"
#include "stuff/allocate.h"
#include "stuff/execute.h"

#include "mach-o/dyld.h"

/*
 * execute() does an execvp using the argv passed to it.  If the parameter
 * verbose is non-zero the command is printed to stderr.  A non-zero return
 * value indicates success zero indicates failure.
 */
__private_extern__
int
execute(
char **argv,
int verbose)
{
    char *name, **p;
    int forkpid, waitpid, termsig;
#if !defined(_WIN32) && !defined(WIN32)
#ifndef __OPENSTEP__
    int waitstatus;
#else
    union wait waitstatus;
#endif
#endif

    name = argv[0];

	if(verbose){
	    fprintf(stderr, "+ %s ", name);
	    p = &(argv[1]);
	    while(*p != (char *)0)
		    fprintf(stderr, "%s ", *p++);
	    fprintf(stderr, "\n");
	}

#if defined(_WIN32) || defined(WIN32)
	forkpid = GetProcessId(GetCurrentProcess());
#else
	forkpid = fork();
#endif
	if(forkpid == -1)
	    system_fatal("can't fork a new process to execute: %s", name);

	if(forkpid == 0){
#if defined(_WIN32) || defined(WIN32)
        char command[MAX_PATH];
#if defined(_MSC_VER) && __STDC_WANT_SECURE_LIB__
        sprintf_s(command, sizeof(command), "%s %s", name, (char *)argv);
#else
        snprintf(command, sizeof(command), "%s %s", name, (char *)argv);
#endif
        if(system(command) == -1)
#else
	    if(execvp(name, argv) == -1)
#endif
		system_fatal("can't find or exec: %s", name);
	    return(1); /* can't get here, removes a warning from the compiler */
	}
	else{
            waitpid = -1;
	    do{
#if !defined(_WIN32) && !defined(WIN32)
	        waitpid = wait(&waitstatus);
#else
            waitpid = 0;
#endif
	    } while (waitpid == -1 && errno == EINTR);
	    if(waitpid == -1)
		system_fatal("wait on forked process %d failed", forkpid);
#if !defined(_WIN32) && !defined(WIN32)
#if !defined(__OPENSTEP__)
        termsig = WTERMSIG(waitstatus);
#else
	    termsig = waitstatus.w_termsig;
#endif
#else
        termsig = 0;
#endif
	    if(termsig != 0 && termsig != SIGINT)
		fatal("fatal error in %s", name);
	    return(
#if !defined(_WIN32) && !defined(WIN32)
#ifndef __OPENSTEP__
		WEXITSTATUS(waitstatus) == 0 &&
#else
		waitstatus.w_retcode == 0 &&
#endif
#endif
		termsig == 0);
	}
}

/*
 * runlist is used by the routine execute_list() to execute a program and it 
 * contains the command line arguments.  Strings are added to it by
 * add_execute_list().  The routine reset_execute_list() resets it for new use.
 */
static struct {
    int size;
    int next;
    char **strings;
} runlist;

/*
 * This routine is passed a string to be added to the list of strings for 
 * command line arguments.
 */
__private_extern__
void
add_execute_list(
char *str)
{
	if(runlist.strings == (char **)0){
	    runlist.next = 0;
	    runlist.size = 128;
	    runlist.strings = allocate(runlist.size * sizeof(char **));
	}
	if(runlist.next + 1 >= runlist.size){
	    runlist.strings = reallocate(runlist.strings,
				(runlist.size * 2) * sizeof(char **));
	    runlist.size *= 2;
	}
	runlist.strings[runlist.next++] = str;
	runlist.strings[runlist.next] = (char *)0;
}

/*
 * This routine is passed a string to be added to the list of strings for 
 * command line arguments and is then prefixed with the path of the executable.
 */
__private_extern__
void
add_execute_list_with_prefix(
char *str)
{
	add_execute_list(cmd_with_prefix(str));
}

/*
 * This routine is passed a string of a command name and a string is returned
 * prefixed with the path of the executable and that command name.
 */
static char *cmdpref = NULL;

#ifndef MAXPATHLEN
#define MAXPATHLEN 4096
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#ifndef MIN
#define MIN(a,b) ((a) < (b)) ? (a) : (b)
#endif

__private_extern__
char *cmd_with_prefix(char *str)
{
#if defined(_WIN32) || defined(WIN32)
	char buf[MAX_PATH];
	uint32_t bufsize = MAX_PATH;
#else
#ifndef __linux__
	int i;
#endif

	char *prefix, buf[MAXPATHLEN], resolved_name[PATH_MAX], *p;
	uint32_t bufsize = MAXPATHLEN;
#endif

	/*
	 * Construct the prefix to the program running.
	 */
#if defined(_WIN32) || defined(WIN32)
	int bytes = GetModuleFileName(NULL, buf, bufsize);
	if (bytes == 0)
		return "";
	buf[bytes] = '\0';
#endif
	cmdpref = (char *)buf;
#if !defined(WIN32) && !defined(_WIN32) && !defined(__linux__)
	i = _NSGetExecutablePath(cmdpref, &bufsize);
	if(i == -1){
	    p = allocate(bufsize);
	    _NSGetExecutablePath(cmdpref, &bufsize);
	}
	prefix = realpath(p, resolved_name);
        cmdpref = rindex(prefix, '/');
	if(cmdpref != NULL)
        cmdpref[1] = '\0';

	return(makestr(prefix, str, NULL));
#elif defined(__linux__)
	char szTmp[32];
	snprintf(szTmp, sizeof(szTmp), "/proc/%d/exe", getpid());
	int bytes = MIN(readlink(szTmp, cmdpref, bufsize), bufsize - 1);
	if(bytes >= 0)
		cmdpref[bytes] = '\0';
	p = (char *)cmdpref;
	prefix = realpath(p, resolved_name);
        cmdpref = rindex(prefix, '/');
	if(cmdpref != NULL)
        cmdpref[1] = '\0';

        return(makestr(prefix, str, NULL));
#else
	while((cmdpref[bytes] != '\\') && (cmdpref[bytes] != '/'))
	{
		cmdpref[bytes] = '\0';
		--bytes;
	}

	return (makestr(cmdpref, str, NULL));
#endif
}

/*
 * This routine reset the list of strings of command line arguments so that
 * an new command line argument list can be built.
 */
__private_extern__
void
reset_execute_list(void)
{
	runlist.next = 0;
}

/*
 * This routine calls execute() to run the command built up in the runlist
 * strings.
 */
__private_extern__
int
execute_list(
int verbose)
{
	return(execute(runlist.strings, verbose));
}
#endif /* !defined(RLD) */

