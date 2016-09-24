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
#include <stuff/bool.h>
#include <llvm-c/Disassembler.h>
#include <string.h>

#ifndef rindex
#define rindex strrchr
#endif

/* Name of this program for error messages (argv[0]) */
extern char *progname;

/*
 * The flags to indicate the actions to perform.
 */
#if defined(_WIN32) || defined(WIN32)
extern BOOL fflag; /* print the fat headers */
extern BOOL aflag; /* print the archive header */
extern BOOL hflag; /* print the exec or mach header */
extern BOOL lflag; /* print the load commands */
extern BOOL Lflag; /* print the shared library names */
extern BOOL Dflag; /* print the shared library id name */
extern BOOL tflag; /* print the text */
extern BOOL dflag; /* print the data */
extern BOOL oflag; /* print the objctive-C info */
extern BOOL Oflag; /* print the objctive-C selector strings only */
extern BOOL rflag; /* print the relocation entries */
extern BOOL Sflag; /* print the contents of the __.SYMDEF file */
extern BOOL vflag; /* print verbosely (symbolicly) when possible */
extern BOOL Vflag; /* print dissassembled operands verbosely */
extern BOOL cflag; /* print the argument and environ strings of a core file */
extern BOOL iflag; /* print the shared library initialization table */
extern BOOL Wflag; /* print the mod time of an archive as an decimal number */
extern BOOL Xflag; /* don't print leading address in disassembly */
extern BOOL Zflag; /* don't use simplified ppc mnemonics in disassembly */
extern BOOL Bflag; /* force Thumb disassembly (ARM objects only) */
extern BOOL Qflag; /* use the HACKED llvm-mc disassembler */
extern BOOL qflag; /* use 'C' Public llvm-mc disassembler */
extern BOOL gflag; /* group the disassembly */
extern BOOL jflag; /* print opcode bytes */
#ifdef OTOOL_NG_SUPPORT
extern BOOL zflag; /* change PIE flag */
extern uint64_t start_offset; /* offset to start reading mach-o header from */
#endif
extern char *pflag; 	/* procedure name to start disassembling from */
extern char *segname,
*sectname;	    /* name of the section to print the contents of */
extern char *mcpu; 	/* the arg of the -mcpu=arg flag */
                    /* Print function offsets when disassembling when TRUE. */

extern BOOL function_offsets;
extern BOOL no_show_raw_insn;
#else
extern unsigned char fflag; /* print the fat headers */
extern unsigned char aflag; /* print the archive header */
extern unsigned char hflag; /* print the exec or mach header */
extern unsigned char lflag; /* print the load commands */
extern unsigned char Lflag; /* print the shared library names */
extern unsigned char Dflag; /* print the shared library id name */
extern unsigned char tflag; /* print the text */
extern unsigned char dflag; /* print the data */
extern unsigned char oflag; /* print the objctive-C info */
extern unsigned char Oflag; /* print the objctive-C selector strings only */
extern unsigned char rflag; /* print the relocation entries */
extern unsigned char Sflag; /* print the contents of the __.SYMDEF file */
extern unsigned char vflag; /* print verbosely (symbolicly) when possible */
extern unsigned char Vflag; /* print dissassembled operands verbosely */
extern unsigned char cflag; /* print the argument and environ strings of a core file */
extern unsigned char iflag; /* print the shared library initialization table */
extern unsigned char Wflag; /* print the mod time of an archive as an decimal number */
extern unsigned char Xflag; /* don't print leading address in disassembly */
extern unsigned char Zflag; /* don't use simplified ppc mnemonics in disassembly */
extern unsigned char Bflag; /* force Thumb disassembly (ARM objects only) */
extern unsigned char Qflag; /* use the HACKED llvm-mc disassembler */
extern unsigned char qflag; /* use 'C' Public llvm-mc disassembler */
extern unsigned char gflag; /* group the disassembly */
extern unsigned char jflag; /* print opcode bytes */
extern char *pflag; 	/* procedure name to start disassembling from */
extern char *segname,
     *sectname;	    /* name of the section to print the contents of */
extern char *mcpu; 	/* the arg of the -mcpu=arg flag */
/* Print function offsets when disassembling when TRUE. */

extern unsigned char function_offsets;
extern unsigned char no_show_raw_insn;
#endif

uint32_t m68k_usrstack(void);
uint32_t m88k_usrstack(void);
uint32_t i386_usrstack(void);
uint32_t hppa_usrstack(void);
uint32_t sparc_usrstack(void);

#ifndef STRUCT_INST
#define STRUCT_INST

struct inst {
    uint64_t address;
    char *label;
#if defined(_WIN32) || defined(WIN32)
    BOOL needs_tmp_label;
#else
    unsigned char needs_tmp_label;
#endif
    char *tmp_label;
#if defined(_WIN32) || defined(WIN32)
    BOOL print;
    BOOL has_raw_target_address;
#else
    unsigned char print;
    unsigned char has_raw_target_address;
#endif
    uint64_t raw_target_address;
};

#endif /* !defined(STRUCT_INST) */
