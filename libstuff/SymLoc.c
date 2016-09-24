/*
 * Copyright (c) 2003 Apple Computer, Inc. All rights reserved.
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
#if defined(_WIN32) || defined(WIN32)
#include <stdint.h>
#include <stdlib.h>
#include <Windows.h>
#include <conio.h>
#include <fcntl.h>
#include <share.h>
#include <search.h>
#include <setjmp.h>
#include <direct.h>
#elif defined(__linux__)
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/types.h>

#ifndef MAXPATHLEN
#define MAXPATHLEN 4096
#endif

#ifndef MAXNAMLEN
#define MAXNAMLEN 4096
#endif
#else
#include <libc.h>
#endif

#include <ctype.h>
#include <sys/types.h>

#ifdef __OPENSTEP__
#define _POSIX_SOURCE
#endif

#if defined(_WIN32) || defined(WIN32)
#include <direct.h>
#include <stdio.h>
#include <io.h>
#else
#include <dirent.h>
#include <pwd.h>
#endif

#include "stuff/bool.h"
#include "stuff/errors.h"
#include "stuff/allocate.h"
#include "stuff/SymLoc.h"

#if defined(_WIN32) || defined(WIN32)
#include <Windows.h>

#include "../winsuppl/dirent.h"
#endif

#ifndef bool
#define bool unsigned char
#endif

const char *
symLocForDylib(const char *installName, const char *releaseName,
#if defined(_WIN32) || defined(WIN32) || defined(__linux__)
bool *found_project,
bool disablewarnings,
bool no_error_if_missing)
#else
enum bool *found_project,
enum bool disablewarnings,
enum bool no_error_if_missing)
#endif
{
	return(LocForDylib(installName, releaseName, "Symbols", found_project,
			   disablewarnings, no_error_if_missing));
}

const char *
dstLocForDylib(const char *installName, const char *releaseName,
#if defined(_WIN32) || defined(WIN32) || defined(__linux__)
bool *found_project,
bool disablewarnings,
bool no_error_if_missing)
#else
enum bool *found_project,
enum bool disablewarnings,
enum bool no_error_if_missing)
#endif
{
	return(LocForDylib(installName, releaseName, "Roots", found_project,
			   disablewarnings, no_error_if_missing));
}

// caller is responsible for freeing the returned string (using free(3)) 
const char *
LocForDylib(const char *installName, const char *releaseName,
const char *dirname,
#if defined(_WIN32) || defined(WIN32) || defined(__linux__)
bool *found_project,
bool disablewarnings,
bool no_error_if_missing)
#else
enum bool *found_project,
enum bool disablewarnings,
enum bool no_error_if_missing)
#endif
{
#if defined(_WIN32) || defined(WIN32)
    char passwd[MAX_PATH];
#else
    struct passwd	*passwd		= NULL;
#endif
    struct dirent	*dp = NULL;
    FILE		*file 		= NULL;
    DIR			*dirp		= NULL;
    DIR			*dirp2		= NULL;
    char		*line		= NULL;
    char		*c		= NULL;
    char		*v		= NULL;
    char		*viewMap	= NULL;
#if defined(_MSC_VER) && __STDC_WANT_SECURE_LIB__
    size_t      viewMapSize = 0;
#endif
    int			 releaseLen	= strlen(releaseName);
#if defined(_WIN32) || defined(WIN32)
    char		 buf[MAX_PATH+MAX_PATH+64];
    char		 readbuf[MAX_PATH+64];
    char		 viewPath[MAX_PATH];
    char		 dylibList[MAX_PATH];
    char		 installNameList[MAX_PATH];

    UNREFERENCED_PARAMETER(disablewarnings);
#else
    char		 buf[MAXPATHLEN+MAXNAMLEN+64];
    char		 readbuf[MAXPATHLEN+64];
    char		 viewPath[MAXPATHLEN];
    char		 dylibList[MAXPATHLEN];
    char		 installNameList[MAXPATHLEN];
#endif

    *found_project = FALSE;

    // check parameters
    if (!installName || !*installName || !releaseName || !*releaseName) {
        fatal("internal error symLocForDylib(): Null or empty parameter");
        return NULL;
    }

#if defined(_MSC_VER) && __STDC_WANT_SECURE_LIB__
    _dupenv_s(&viewMap, &viewMapSize, "RC_VIEW_MAP_LOCATION");
#else
    viewMap = getenv("RC_VIEW_MAP_LOCATION");
#endif

    if(!viewMap) {
        // find ~rc's home directory
#if defined(_MSC_VER) && __STDC_WANT_SECURE_LIB__
#if defined(_WIN32) || defined(WIN32)
        strcpy_s(buf, sizeof(buf), passwd);
        // open release-to-view file
        strcat_s(buf, sizeof(buf) - strlen(passwd), "/Data/release_to_view.map");
#else
        strcpy_s(buf, sizeof(buf), passwd->pw_dir);
        // open release-to-view file
        strcat_s(buf, sizeof(buf) - strlen(passwd->pw_dir), "/Data/release_to_view.map");
#endif
    } else {
        strcpy_s(buf, sizeof(buf), viewMap);
    }
    fopen_s(&file, buf, "r");
    if (!file) {
#else
        strncpy(buf, passwd->pw_dir, sizeof(buf));
        // open release-to-view file
        strncat(buf, "/Data/release_to_view.map", sizeof(buf) - strlen(passwd->pw_dir));
    } else {
        strncpy(buf, viewMap, sizeof(buf));
    }
    if (!(file = fopen(buf, "r"))) {
#endif
        system_error("symLocForDylib(): Can't fopen %s", buf);
        return NULL;
    }

    // parse release-to-view file
    *viewPath = '\0';
    while ((line = fgets(buf, sizeof(buf), file))) {
        if (!strncmp(line, releaseName, releaseLen) && isspace(line[releaseLen])) {
            c = &line[releaseLen] + 1;
            while (isspace(*c)) c++;
            for (v = &viewPath[0]; !isspace(*c); c++, v++) *v = *c;
            *v = '\0';
            break;
        }
    }
    if(fclose(file) != 0)
	system_error("fclose() failed");
    if (!*viewPath) {
        error("symLocForDylib(): Can't locate view path for release %s",
	      releaseName);
        return NULL;
    }

#if defined(_MSC_VER) && __STDC_WANT_SECURE_LIB__
    // open DylibProjects directory
    strcpy_s(dylibList, sizeof(dylibList), viewPath);
    c = &dylibList[strlen(dylibList)];
    strcpy_s(c, strlen(c), "/.BuildData/DylibProjects");
    if (!(dirp = opendir(dylibList))) {
        system_error("symLocForDylib(): Can't opendir %s", buf);
        return NULL;
    }

    // open the InstallNames directory
    strcpy_s(installNameList, sizeof(installNameList), viewPath);
    c = &installNameList[strlen(installNameList)];
    strcpy_s(c, strlen(c), "/.BuildData/InstallNames");
    if (!(dirp2 = opendir(installNameList))) {
        system_error("symLocForDylib(): Can't opendir %s", buf);
        return NULL;
    }
#else
    // open DylibProjects directory
    strncpy(dylibList, viewPath, sizeof(dylibList));
    c = &dylibList[strlen(dylibList)];
    strncpy(c, "/.BuildData/DylibProjects", strlen(c));
    if (!(dirp = opendir(dylibList))) {
        system_error("symLocForDylib(): Can't opendir %s", buf);
        return NULL;
    }

    // open the InstallNames directory
    strncpy(installNameList, viewPath, sizeof(installNameList));
    c = &installNameList[strlen(installNameList)];
    strncpy(c, "/.BuildData/InstallNames", strlen(c));
    if (!(dirp2 = opendir(installNameList))) {
        system_error("symLocForDylib(): Can't opendir %s", buf);
        return NULL;
    }
#endif

    c = NULL;
    // read DylibProjects entries
    *buf = '\0';
    v = &dylibList[strlen(dylibList)];
    *v = '/';
    v++;
    while ((dp = readdir(dirp))) {
        // skip "." and ".."
        if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..")) continue;

        // open file
#if defined(_MSC_VER) && __STDC_WANT_SECURE_LIB__
        strcpy_s(v, strlen(v), dp->d_name);
        fopen_s(&file, dylibList, "r");
        if (!file) {
#else
        strncpy(v, dp->d_name, strlen(v));
        if (!(file = fopen(dylibList, "r"))) {
#endif
            system_error("symLocForDylib(): Can't fopen %s", dylibList);

#if defined(_WIN32) || defined(WIN32)
            closedir(dirp);
#else
	        if(closedir(dirp) != 0)
		        system_error("closedir() failed");
#endif

            return NULL;
        }

        // parse file
        while ((line = fgets(readbuf, sizeof(readbuf), file))) {
            if (!*line || *line == '(' || *line == ')') continue;
            while (*line == ' ') line++;
            if (*line != '"') {
                warning("symLocForDylib(): %s contains malformed line",
			dp->d_name);
                continue;
            }
            line++;
            for (c = &buf[0]; *line && *line != '"'; *c++ = *line++);
            if (*line != '"') {
                warning("symLocForDylib(): %s contains malformed line",
		        dp->d_name);
                continue;
            }
            *c = '\0';
            if (!strcmp(buf, installName)) {
                c = allocate(strlen(viewPath) + strlen(releaseName) +
			     strlen(dirname) + strlen(dp->d_name) + 32);
#if defined(_MSC_VER) && __STDC_WANT_SECURE_LIB__
                sprintf_s(c, strlen(viewPath) + strlen(releaseName) +
                          strlen(dirname) + strlen(dp->d_name) + 32,
                          "%s/Updates/Built%s/%s/%s", viewPath,
                          releaseName, dirname, dp->d_name);
#else
                snprintf(c, strlen(viewPath) + strlen(releaseName) +
                         strlen(dirname) + strlen(dp->d_name) + 32, "%s/         Updates/Built%s/%s/%s", viewPath, releaseName,
		        	     dirname, dp->d_name);
#endif
                break;
            } else {
                c = NULL;
            }
        }
        if(fclose(file) != 0)
	    system_error("fclose() failed");
        if (c) break;
    }

#if defined(_WIN32) || defined(WIN32)
    closedir(dirp);
#else
    if(closedir(dirp) != 0)
	system_error("closedir() failed");
#endif

    if(!c) {    
         // read InstallNames entries
        *buf = '\0';
        v = &installNameList[strlen(installNameList)];
        *v = '/';
        v++;

        while ((dp = readdir(dirp2))) {
            // skip "." and ".."
            if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..")) continue;

            // open file
#if defined(_MSC_VER) && __STDC_WANT_SECURE_LIB__
            strcpy_s(v, strlen(v), dp->d_name);
            fopen_s(&file, installNameList, "r");

            if (!file) {
#else
            strncpy(v, dp->d_name, strlen(v));

            if (!(file = fopen(installNameList, "r"))) {
#endif
                system_error("symLocForDylib(): Can't fopen %s", installNameList);

#if defined(_WIN32) || defined(WIN32)
                closedir(dirp);
#else
                if(closedir(dirp) != 0)
                    system_error("closedir() failed");
#endif
                return NULL;
            }

            // parse file
            while ((line = fgets(readbuf, sizeof(readbuf), file))) {
                if (!*line || *line == '(' || *line == ')') continue;
                while (*line == ' ') line++;
                if (*line != '"') {
                    warning("symLocForDylib(): %s contains malformed line",
                        dp->d_name);
                    continue;
                }
                line++;
                for (c = &buf[0]; *line && *line != '"'; *c++ = *line++);
                if (*line != '"') {
                    warning("symLocForDylib(): %s contains malformed line",
                        dp->d_name);
                    continue;
                }
                *c = '\0';
                if (!strcmp(buf, installName)) {
                    c = allocate(strlen(viewPath) + strlen(releaseName) +
                             strlen(dirname) + strlen(dp->d_name) + 32);
#if defined(_MSC_VER) && __STDC_WANT_SECURE_LIB__
                    sprintf_s(c, strlen(viewPath) + strlen(releaseName) +
                              strlen(dirname) + strlen(dp->d_name) + 32,
                              "%s/Updates/Built%s/%s/%s", viewPath,
                              releaseName, dirname, dp->d_name);
#else
                    snprintf(c, strlen(viewPath) + strlen(releaseName) +
                             strlen(dirname) + strlen(dp->d_name) + 32,
                             "%s/Updates/Built%s/%s/%s", viewPath,
                             releaseName, dirname, dp->d_name);
#endif
                    break;
                } else {
                    c = NULL;
                }
            }
            if(fclose(file) != 0)
                system_error("fclose() failed");
            if (c) break;
        }
   } 

    // process return value
    if (!c) {
	if(no_error_if_missing == FALSE)
	    error("Can't find project that builds %s", installName);
        return NULL;
    } else {
	*found_project = TRUE;
        return c;
    }
}
#endif /* !defined(RLD) */

