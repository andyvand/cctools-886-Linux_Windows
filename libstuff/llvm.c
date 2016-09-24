#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#ifdef __GNUC__
#include <unistd.h>
#endif

#if defined(_WIN32) || defined(WIN32)
#include <stdlib.h>
#include <Windows.h>
#include <fileapi.h>
#elif defined(__linux__)
#define __STDC_LIMIT_MACROS 1
#define __STDC_CONSTANT_MACROS 1

#include <sys/file.h>
#include <dlfcn.h>

#ifndef MAXPATHLEN
#define MAXPATHLEN 4096
#endif /* MAXPATHLEN */

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif /* PATH_MAX */
#else
#include <libc.h>
#include <sys/file.h>
#include <dlfcn.h>
#endif

#if defined(__linux__)
#include <llvm-c-3.8/llvm-c/Disassembler.h>
#else
#include <llvm-c/Disassembler.h>
#endif

#include "stuff/llvm.h"
#include "stuff/allocate.h"

#include <mach-o/dyld.h>

#ifdef __GNUC__
#include <unistd.h>
#endif

#ifndef __stdcall
#define __stdcall
#endif /* __stdcall */

/*
 * The disassembler API is currently exported from libLTO.dylib.  Eventually we
 * plan to include it (along with the current libLTO APIs) in a generic
 * libLLVM.dylib.
 */
#if defined(_WIN32) || defined(WIN32)
#define LIB_LLVM "LTO.dll"
#elif defined(__linux__)
#define LIB_LLVM "libLTO.so"
#else
#define LIB_LLVM "libLTO.dylib"
#endif

static int tried_to_load_llvm = 0;
#if defined(_WIN32) || defined(WIN32)
static HMODULE llvm_handle = NULL;
#define dlsym(a,b) GetProcAddress(a,b)
#else
static void *llvm_handle = NULL;
#endif
static void (*initialize)(void) = NULL;
static LLVMDisasmContextRef (*create)(const char *, void *, int,
	LLVMOpInfoCallback, LLVMSymbolLookupCallback) = NULL;
static LLVMDisasmContextRef (*createCPU)(const char *, const char *,void *, int,
	LLVMOpInfoCallback, LLVMSymbolLookupCallback) = NULL;
static void (*dispose)(LLVMDisasmContextRef) = NULL;
static size_t (*disasm)(LLVMDisasmContextRef, uint8_t *, uint64_t, uint64_t,
	char *, size_t) = NULL;
static int (*options)(LLVMDisasmContextRef, uint64_t) = NULL;
static const char * (*version)(void) = NULL;

/*
 * load_llvm() will dynamically load libLTO.dylib if tried_to_load_llvm is 0,
 * and set llvm_handle to the value returned by dlopen() and set the function
 * pointers.
 */
static void load_llvm(void)
{
   uint32_t bufsize;
#if defined(_WIN32) || defined(WIN32)
   char *p;
   char *llvm_path;
   char buf[MAX_PATH];
#else
   char *p;
   char *prefix;
   char *llvm_path;
   char buf[MAXPATHLEN];
   char resolved_name[PATH_MAX];

#ifndef __linux__
   int i = 0;
#endif
#endif

	if(tried_to_load_llvm == 0){
	    tried_to_load_llvm = 1;
	    /*
	     * Construct the prefix to this executable assuming it is in a bin
	     * directory relative to a lib directory of the matching lto library
	     * and first try to load that.  If not then fall back to trying
	     * "/Applications/Xcode.app/Contents/Developer/Toolchains/
	     * XcodeDefault.xctoolchain/usr/lib/" LIB_LLVM.
	     */
#if defined(_WIN32) || defined(WIN32)
        bufsize = MAX_PATH;
        p = buf;
        llvm_handle = GetModuleHandleA(LIB_LLVM);
#else
	    bufsize = MAXPATHLEN;
        p = buf;
#if defined(__linux__)
	p = getcwd(buf, bufsize);
#else
        i = _NSGetExecutablePath(p, &bufsize);
        if (i == -1) {
            p = allocate(bufsize);
            _NSGetExecutablePath(p, &bufsize);
        }
#endif
        prefix = realpath(p, resolved_name);
        p = rindex(prefix, '/');
        if (p != NULL)
            p[1] = '\0';
        llvm_path = makestr(prefix, "../lib/" LIB_LLVM, NULL);

        llvm_handle = dlopen(llvm_path, RTLD_NOW);
        if (llvm_handle == NULL) {
            free(llvm_path);
            llvm_path = NULL;
#if defined(__linux__)
            llvm_handle = dlopen("/usr/lib/llvm-3.8/lib/" LIB_LLVM, RTLD_NOW);
#else
            llvm_handle = dlopen("/Applications/Xcode.app/Contents/"
                                 "Developer/Toolchains/XcodeDefault."
                                 "xctoolchain/usr/lib/" LIB_LLVM,
                                 RTLD_NOW);
#endif
        }
        if (llvm_handle == NULL)
            return;
#endif

	    create = (LLVMDisasmContextRef (__stdcall *)(const char *, void *, int, LLVMOpInfoCallback, LLVMSymbolLookupCallback))dlsym(llvm_handle, "LLVMCreateDisasm");
	    dispose = (void (__stdcall *)(LLVMDisasmContextRef))dlsym(llvm_handle, "LLVMDisasmDispose");
	    disasm = (size_t (__stdcall *)(LLVMDisasmContextRef, uint8_t *, uint64_t, uint64_t, char *, size_t))dlsym(llvm_handle, "LLVMDisasmInstruction");

	    /* Note we allow these to not be defined */
	    options = (int (__stdcall *)(LLVMDisasmContextRef, uint64_t))dlsym(llvm_handle, "LLVMSetDisasmOptions");
	    createCPU = (LLVMDisasmContextRef (__stdcall *)(const char *, const char *, void *, int, LLVMOpInfoCallback, LLVMSymbolLookupCallback))dlsym(llvm_handle, "LLVMCreateDisasmCPU");
	    version = (const char * (__stdcall *)(void))dlsym(llvm_handle, "lto_get_version");

	    if(create == NULL ||
	       dispose == NULL ||
	       disasm == NULL){

#if defined(_WIN32) || defined(WIN32)
                llvm_handle = NULL;
#else
		dlclose(llvm_handle);
#endif
		if(llvm_path != NULL)
		    free(llvm_path);
		llvm_handle = NULL;
		create = NULL;
		createCPU = NULL;
		dispose = NULL;
		disasm = NULL;
		options = NULL;
		version = NULL;
		return;
	    }
	}
	if(llvm_handle == NULL)
	    return;
}

/*
 * Wrapper to dynamically load LIB_LLVM and call LLVMCreateDisasm().
 */
__private_extern__
LLVMDisasmContextRef
llvm_create_disasm(
const char *TripleName,
const char *CPU,
void *DisInfo,
int TagType,
LLVMOpInfoCallback GetOpInfo,
LLVMSymbolLookupCallback SymbolLookUp)
{
   LLVMDisasmContextRef DC;

	if(tried_to_load_llvm == 0){
	    load_llvm();
	}
	if(llvm_handle == NULL)
	    return(NULL);

	/*
	 * Note this was added after the interface was defined, so it may
	 * be undefined.  But if not we must call it first.
	 */
	initialize = (void (__stdcall *)(void))dlsym(llvm_handle, "lto_initialize_disassembler");
	if(initialize != NULL)
	    initialize();

	if(*CPU != '\0' && createCPU != NULL)
	    DC = createCPU(TripleName, CPU, DisInfo, TagType, GetOpInfo,
			   SymbolLookUp);
	else
	    DC = create(TripleName, DisInfo, TagType, GetOpInfo, SymbolLookUp);
	return(DC);
}

/*
 * Wrapper to call LLVMDisasmDispose().
 */
__private_extern__
void
llvm_disasm_dispose(
LLVMDisasmContextRef DC)
{
	if(dispose != NULL)
	    dispose(DC);
}

/*
 * Wrapper to call LLVMDisasmInstruction().
 */
__private_extern__
size_t
llvm_disasm_instruction(
LLVMDisasmContextRef DC,
uint8_t *Bytes,
uint64_t BytesSize,
uint64_t Pc,
char *OutString,
size_t OutStringSize)
{

	if(disasm == NULL)
	    return(0);
	return(disasm(DC, Bytes, BytesSize, Pc, OutString, OutStringSize));
}

/*
 * Wrapper to call LLVMSetDisasmOptions().
 */
__private_extern__
int
llvm_disasm_set_options(
LLVMDisasmContextRef DC,
uint64_t Options)
{

	if(options == NULL)
	    return(0);
	return(options(DC, Options));
}

/*
 * Wrapper to call lto_get_version().
 */
__private_extern__
const char *
llvm_disasm_version_string(void)
{
	if(tried_to_load_llvm == 0){
	    load_llvm();
	}
	if(llvm_handle == NULL)
	    return(NULL);
	if(version == NULL)
	    return(NULL);
	return(version());
}

