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
/* Exported API for standalone library */
#if !(defined(KLD) && defined(__STATIC__))
#include <mach/mach.h>
#else /* defined(KLD) && defined(__STATIC__) */
#include <mach/kern_return.h>
#endif /* !(defined(KLD) && defined(__STATIC__)) */
#include <mach-o/loader.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>

#if defined(_WIN32) || defined(WIN32)
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif

#ifndef bcopy
#ifdef __OPENSTEP__
extern char *bcopy(char *src, char *dst, int n);
#endif
#else
#define bcopy(src,dst,n) memcpy(dst,src,n)
#endif

extern void *bsearch(const void *key, const void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *));
#ifndef bzero
#ifdef __OPENSTEP__
extern int bzero(char *b, int length);
#endif
#else
#define bzero(b,l) memset(b,0,l)
#endif
extern void *memset(void *s, int c, size_t n);

/*
 * These are defined internally by GCC
 *
 * extern char *memcpy(void *dst, const void *src, int len);
 * extern size_t strlen(const char *s);
 */

extern int errno;
extern struct segment_command *
  getsegbynamefromheader(struct mach_header *mhp, char *segname);
extern int ptol(char *str);

/* setjmp/longjmp:
 * #include <setjmp.h>
 *
 * extern int setjmp(jmp_buf env);
 * extern void longjmp( jmp_buf env, int val);
 */

extern int slvprintf(char *buffer, int len, const char *fmt, va_list arg);
extern int sprintf(char *s, const char *format, ...);

extern char *strcat(char *s1, const char *s2);
extern int strcmp(const char *s1, const char *s2);
extern char *strcpy(char *s1, const char *s2);
char *strerror(int errnum);
extern int strncmp(const char *s1, const char *s2, size_t n);
extern char *strncpy(char *s1, const char *s2, size_t n);
extern long strtol(
    const char *nptr,
    char **endptr,
    register int base
);
extern unsigned long strtoul(
    const char *nptr,
    char **endptr,
    register int base
);

#if !(defined(KLD) && defined(__STATIC__))
/* Mach */
#ifdef __MACH30__
extern mach_port_t task_self_;
#else
extern port_t task_self_;
#endif
extern kern_return_t vm_allocate(
    mach_port_t target_task,
    vm_address_t *address,
    vm_size_t size,
    boolean_t anywhere
);
#if defined(_WIN32) || defined(WIN32)
inline kern_return_t vm_deallocate(
    mach_port_t target_task,
    vm_address_t address,
    vm_size_t size
)
{
    UNREFERENCED_PARAMETER(target_task);
    UNREFERENCED_PARAMETER(address);
    UNREFERENCED_PARAMETER(size);

    printf("FIXME: Implement vm_deallocate\n");
}
inline kern_return_t host_info(
    mach_port_t host,
    int flavor,
    host_info_t host_info,
    unsigned int *host_info_count
)
{
    UNREFERENCED_PARAMETER(host);
    UNREFERENCED_PARAMETER(flavor);
    UNREFERENCED_PARAMETER(host_info);
    UNREFERENCED_PARAMETER(host_info_count);

    printf("FIXME: Implement host_info\n");

    return 0;
}
#else
extern kern_return_t vm_deallocate(
    mach_port_t target_task,
    vm_address_t address,
    vm_size_t size
);
extern kern_return_t host_info(
    mach_port_t host,
    int flavor,
    host_info_t host_info,
    unsigned int *host_info_count
);
#endif

extern vm_size_t vm_page_size;
extern mach_port_t host_self(void);
extern int getpagesize(void);

#if defined(_WIN32) || defined(WIN32)
#undef StringError
#ifndef STRINGERROR_DEFINED
#define STRINGERROR_DEFINED 1
static inline char *StringError(int X)
{
    char ERRS[MAX_PATH];
    char *p = (char *)ERRS;

#if defined(_MSC_VER) && __STDC_WANT_SECURE_LIB__
    strerror_s(p, sizeof(ERRS), X);
#else
    p = strerror(X);
#endif

    return p;
}
#endif
#ifndef mach_error_string
#define mach_error_string(X) StringError(X)
#endif
#else
extern char *mach_error_string(int errnum);
#endif

/* Malloc/Zalloc */
extern int malloc_init(char *start, int size, int nodes);
extern char * zalloc(int size);
extern int zfree(char *start);
extern int zinit(char *start, int size, int nodes);
#define ZALLOC_NODES	64	/* default number of nodes */
#endif /* !(defined(KLD) && defined(__STATIC__)) */
extern void *malloc(size_t size);
extern void free(void *start);
extern void *realloc(void *ptr, size_t size);
