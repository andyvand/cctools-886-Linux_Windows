#ifndef _COFF_AOUTHDR_H
#define _COFF_AOUTHDR_H
/*
 * These data structures are discribed in the pecoff_v8.doc in section
 * "3.4. Optional Header (Image Only)"
 */
#include <stdint.h>

/*
 * Every image file has an optional header that provides information to the
 * loader. This header is optional in the sense that some files (specifically,
 * object files) do not have it. For image files, this header is required.
 *
 * Since definitions for this header were based from the GNU binutils files
 * coff/external.h, coff/internal.h and other files the copyright info is below.
 */

/* Copyright 2001 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. */

struct aouthdr
{
	uint16_t magic;		/* type of file				*/
	uint16_t vstamp;	/* version stamp			*/
	uint32_t tsize;		/* text size in bytes, padded to FW bdry*/
	uint32_t dsize;		/* initialized data "  "		*/
	uint32_t bsize;		/* uninitialized data "   "		*/
	uint32_t entry;		/* entry pt.				*/
	uint32_t text_start;	/* base of text used for this file 	*/
	uint32_t data_start;	/* base of data used for this file 	*/

	uint32_t ImageBase;
	uint32_t SectionAlignment;
	uint32_t FileAlignment;
	uint16_t MajorOperatingSystemVersion;
	uint16_t MinorOperatingSystemVersion;
	uint16_t MajorImageVersion;
	uint16_t MinorImageVersion;
	uint16_t MajorSubsystemVersion;
	uint16_t MinorSubsystemVersion;
	uint32_t Win32VersionValue;	/* Reserved, must be zero. */
	uint32_t SizeOfImage;
	uint32_t SizeOfHeaders;
	uint32_t CheckSum;
	uint16_t Subsystem;
	uint16_t DllCharacteristics;
	uint32_t SizeOfStackReserve;
	uint32_t SizeOfStackCommit;
	uint32_t SizeOfHeapReserve;
	uint32_t SizeOfHeapCommit;
	uint32_t LoaderFlags;
	uint32_t NumberOfRvaAndSizes;
	uint32_t DataDirectory[16][2]; /* 16 entries, 2 elements/entry, */
};

struct aouthdr_64
{
	uint16_t magic;		/* type of file				*/
	uint16_t vstamp;	/* version stamp			*/
	uint32_t tsize;		/* text size in bytes, padded to FW bdry*/
	uint32_t dsize;		/* initialized data "  "		*/
	uint32_t bsize;		/* uninitialized data "   "		*/
	uint32_t entry;		/* entry pt.				*/
	uint32_t text_start;	/* base of text used for this file 	*/
	/* note no base of data field in the 64-bit optional header */

	uint64_t ImageBase;
	uint32_t SectionAlignment;
	uint32_t FileAlignment;
	uint16_t MajorOperatingSystemVersion;
	uint16_t MinorOperatingSystemVersion;
	uint16_t MajorImageVersion;
	uint16_t MinorImageVersion;
	uint16_t MajorSubsystemVersion;
	uint16_t MinorSubsystemVersion;
	uint32_t Win32VersionValue;	/* Reserved, must be zero. */
	uint32_t SizeOfImage;
	uint32_t SizeOfHeaders;
	uint32_t CheckSum;
	uint16_t Subsystem;
	uint16_t DllCharacteristics;
	uint64_t SizeOfStackReserve;
	uint64_t SizeOfStackCommit;
	uint64_t SizeOfHeapReserve;
	uint64_t SizeOfHeapCommit;
	uint32_t LoaderFlags;
	uint32_t NumberOfRvaAndSizes;
	uint32_t DataDirectory[16][2]; /* 16 entries, 2 elements/entry, */
};

struct aouthdr_rom {
	uint16_t magic;
	uint16_t vstamp;
	uint32_t tsize;
	uint32_t dsize;
	uint32_t bsize;
	uint32_t entry;
	uint32_t text_start;
	uint32_t data_start;
	uint32_t bss_start;
	uint32_t gpr_mask;
	uint32_t cpr_mask[4];
	uint32_t gp_value;
};

/* for the magic field */
#define PE32MAGIC       0x10b   /* 32-bit image */
#define PE32PMAGIC	0x20b	/* 32-bit image inside 64-bit address space */
#define ROMMAGIC	0x107   /* ROM image */

/* for the FileAlignment field */
#define FILEALIGNMENT	0x200	/* The alignment factor (in bytes) that is used
				   to align the raw data of sections in the
				   image file. The value should be a power of 2
				   between 512 and 64 K, inclusive. The default
				   is 512. If the SectionAlignment is less than
				   the architecture's page size, then
				   FileAlignment must match SectionAlignment. */

/* for the SectionAlignment field */
#define SECTIONALIGNMENT 0x1000 /* The alignment (in bytes) of sections when
				   they are loaded into memory. It must be
				   greater than or equal to FileAlignment.
				   The default is the page size for the
				   architecture. */

/* for the vstamp field */
#define LINKER_VERSION 256 /* That is, 2.56 */
/* This piece of magic sets the "linker version" field to LINKER_VERSION.  */
#define VSTAMP (LINKER_VERSION / 100 + (LINKER_VERSION % 100) * 256)

/* for the Subsystem field */
#define IMAGE_SUBSYSTEM_UNKNOWN 0
#define IMAGE_SUBSYSTEM_NATIVE 1
#define IMAGE_SUBSYSTEM_WINDOWS_GUI 2
#define IMAGE_SUBSYSTEM_WINDOWS_CUI 3
#define IMAGE_SUBSYSTEM_OS2_CUI 5
#define IMAGE_SUBSYSTEM_POSIX_CUI 7
#define IMAGE_SUBSYSTEM_NATIVE_WINDOWS 8
#define IMAGE_SUBSYSTEM_WINDOWS_CE_GUI 9
#define IMAGE_SUBSYSTEM_EFI_APPLICATION		10
#define IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER	11
#define IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER	12
#define IMAGE_SUBSYSTEM_EFI_ROM 13
#define IMAGE_SUBSYSTEM_XBOX 14
#define IMAGE_SUBSYSTEM_WINDOWS_BOOT_APPLICATION 16

#endif /* _COFF_AOUTHDR_H */

