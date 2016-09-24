/*
 * Copyright © 2009 Apple Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1.  Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer. 
 * 2.  Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution. 
 * 3.  Neither the name of Apple Inc. ("Apple") nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission. 
 * 
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * @APPLE_LICENSE_HEADER_END@
 */
#ifndef __DYNAMIC__
#define _BUILDING_NOTIFY_ 1

#include <notify.h>

#if !defined(_WIN32) && !defined(WIN32)
#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(x)
#endif
#else
#include <stdint.h>
#include <Windows.h>
#endif

uint32_t notify_post(const char *name)
{
    UNREFERENCED_PARAMETER(name);

	return 0;
}

uint32_t notify_register_check(const char *name, int *out_token)
{
    UNREFERENCED_PARAMETER(name);

	*out_token = 0;
	return 0;
}

uint32_t notify_register_signal(const char *name, int sig, int *out_token)
{
    UNREFERENCED_PARAMETER(sig);
    UNREFERENCED_PARAMETER(name);

	*out_token = 0;
	return 0;
}

#ifndef MACH_PORT_NULL
#define MACH_PORT_NULL 0
#endif

#ifndef mach_port_t
#define mach_port_t int
#endif

uint32_t notify_register_mach_port(const char *name, mach_port_t *notify_port, int flags, int *out_token)
{
    UNREFERENCED_PARAMETER(flags);
    UNREFERENCED_PARAMETER(name);

	*notify_port = MACH_PORT_NULL;
	out_token = 0;
	return 0;
}

uint32_t notify_register_file_descriptor(const char *name, int *notify_fd, int flags, int *out_token)
{
    UNREFERENCED_PARAMETER(flags);
    UNREFERENCED_PARAMETER(name);

	*notify_fd = 0;
	out_token = 0;
	return 0;
}

uint32_t notify_check(int token, int *check)
{
    UNREFERENCED_PARAMETER(token);

	*check = 1;
	return 0;
}

uint32_t notify_cancel(int token)
{
    UNREFERENCED_PARAMETER(token);
    
    return 0;
}

int __notify_78945668_info__ = 0;

uint32_t notify_monitor_file(int t, char *s, int f)
{
    UNREFERENCED_PARAMETER(f);
    UNREFERENCED_PARAMETER(s);
    UNREFERENCED_PARAMETER(t);

	return 0;
}
#endif /* !defined(__DYNAMIC__) */
