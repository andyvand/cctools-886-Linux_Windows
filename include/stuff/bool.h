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
/* bool.h */
#ifndef ENUM_DYLD_BOOL
#define ENUM_DYLD_BOOL

#define _STDBOOL_H  /* for gcc header */
#define __STDBOOL_H /* for clang header */

#include <stdint.h>

#if defined(__i386__) || defined(__arm__) || defined(__aarch64__) || defined(__arm64__) || defined(__x86_64__) || defined(_AMD64_) || defined(_M_AMD64) || defined(_X86_) || defined(_M_X86) || defined(_IX86_) || defined(_M_IX86) || defined(_ARM_) || defined(_M_ARM) || defined(_ARM64_) || defined(_M_ARM64)
#define __LITTLE_ENDIAN__
#elif defined(__ppc__) || defined(__ppc64__)
#define __BIG_ENDIAN__
#else /* __NO_ENDIAN__ */
#error "Set the endianness for the CPU in this header"
#endif /* __ENDIAN__ */

#include <stdio.h>
#include <fcntl.h>

#ifndef _open
#define _open open
#endif /* _open */

#ifndef _sopen
#if defined(__linux__)
#define _sopen(a,b,c,d) open(a,b,d)
#else
#define _sopen sopen
#endif
#endif /* _sopen */

#ifndef _close
#define _close close
#endif /* _close */

#ifndef _read
#define _read read
#endif /* _read */

#ifndef _write
#define _write write
#endif /* _write */

#ifndef _unlink
#define _unlink unlink
#endif /* _unlink */

#ifndef _fileno
#define _fileno fileno
#endif /* _fileno */

#ifndef _umask
#define _umask umask
#endif /* _umask */

#ifndef _lseek
#define _lseek lseek
#endif /* _lseek */

#ifndef _fstat
#define _fstat fstat
#endif /* _fstat */

#ifndef _stat
#define _stat stat
#endif /* _stat */

#ifndef __cdecl
#define __cdecl
#endif /* __cdecl */

#ifndef __stdcall
#define __stdcall
#endif /* __stdcall */

#ifndef UINT_MAX
#define UINT_MAX 0xFFFFFFFF
#endif /* UINT_MAX */

#ifndef BOOL
#define BOOL unsigned char
#endif /* BOOL */

#ifndef FALSE
#define FALSE 0
#endif /* FALSE */

#ifndef TRUE
#define TRUE 1
#endif /* TRUE */

#ifdef __linux__
#include <linux/swab.h>

#ifndef SWAP_LONG
#define SWAP_LONG __swab32
#endif
#endif

#endif /* ENUM_DYLD_BOOL */

