/*
 * Copyright (c) 2004, Apple Computer, Inc. All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer. 
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution. 
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission. 
 * 
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#if defined(_WIN32) || defined(WIN32)
#include "../include/mach/mach.h"

#ifndef fatal
void fatal(const char *X, ...);
#endif

#ifndef warning
void warning(const char *X, ...);
#endif

#ifndef get_host_byte_sex
#if defined(__LITTLE_ENDIAN__)
#define get_host_byte_sex() __LITTLE_ENDIAN__
#else
#define get_host_byte_sex() __BIG_ENDIAN__
#endif
#endif

#if defined(_WIN32) || defined(WIN32)
#undef  __attribute__
#define __attribute__(x,...)
#endif

#ifndef __private_extern__
#define __private_extern__ /*X*/
#endif
#else
#include <mach/mach.h>
#endif

/* user defined (imported) */
extern char *progname;

/* defined in errors.c */
/* number of detected calls to error() */
extern uint32_t errors __attribute__((visibility("hidden")));

extern void warning(
    const char *format, ...)
#ifdef __GNUC__
    __attribute__ ((format (printf, 1, 2)))
#endif
    __attribute__((visibility("hidden")));
extern void error(
    const char *format, ...)
#ifdef __GNUC__
    __attribute__ ((format (printf, 1, 2)))
#endif
    __attribute__((visibility("hidden")));
extern void error_with_arch(
    const char *arch_name,
    const char *format, ...)
#ifdef __GNUC__
    __attribute__ ((format (printf, 2, 3)))
#endif
    __attribute__((visibility("hidden")));
extern void system_error(
    const char *format, ...)
#ifdef __GNUC__
    __attribute__ ((format (printf, 1, 2)))
#endif
    __attribute__((visibility("hidden")));
#if !defined(_WIN32) && !defined(WIN32)
extern void fatal(
    const char *format, ...)
#ifdef __GNUC__
    __attribute__ ((format (printf, 1, 2)))
#endif
    __attribute__((visibility("hidden")));
#endif
extern void system_fatal(
    const char *format, ...)
#ifdef __GNUC__
    __attribute__ ((format (printf, 1, 2)))
#endif
    __attribute__((visibility("hidden")));
extern void my_mach_error(
    kern_return_t r,
    char *format, ...)
#ifdef __GNUC__
    __attribute__ ((format (printf, 2, 3)))
#endif
    __attribute__((visibility("hidden")));
extern void mach_fatal(
    kern_return_t r,
    char *format, ...)
#ifdef __GNUC__
    __attribute__ ((format (printf, 2, 3)))
#endif
    __attribute__((visibility("hidden")));
