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

#define BUILDING_GET_SECT_INFO 1

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "mach-o/loader.h"
#include "stuff/bytesex.h"
#include "dyld_bind_info.h"
#include "ofile_print.h"
#include "coff/base_relocs.h"
#include "coff/bytesex.h"

static unsigned char
get_sect_info(
char *segname,				/* input */
char *sectname,
struct load_command *load_commands,
uint32_t ncmds,
uint32_t sizeofcmds,
uint32_t filetype,
enum byte_sex load_commands_byte_sex,
char *object_addr,
uint32_t object_size,
char **sect_pointer,			/* output */
uint64_t *sect_size,
uint64_t *sect_addr,
struct relocation_info **sect_relocs,
uint32_t *sect_nrelocs,
uint32_t *sect_flags,
uint64_t *seg_addr)
{
    enum byte_sex host_byte_sex;
    unsigned char found, swapped;
    uint32_t i, j, left, size;
    struct load_command *lc, l;
    uint32_t cmd;
    struct segment_command sg;
    struct segment_command_64 sg64;
    struct section s;
    struct section_64 s64;
    char *p;

	*sect_pointer = NULL;
	*sect_size = 0;
	*sect_addr = 0;
	*sect_relocs = NULL;
	*sect_nrelocs = 0;
	*sect_flags = 0;

	found = FALSE;
	cmd = 0;
	host_byte_sex = get_host_byte_sex();
	swapped = host_byte_sex != load_commands_byte_sex;

	lc = load_commands;
	for(i = 0 ; found == FALSE && i < ncmds; i++){
	    memcpy((char *)&l, (char *)lc, sizeof(struct load_command));
	    if(swapped)
		swap_load_command(&l, host_byte_sex);
	    if(l.cmdsize % sizeof(int32_t) != 0)
		printf("load command %u size not a multiple of "
		       "sizeof(int32_t)\n", i);
	    if((char *)lc + l.cmdsize >
	       (char *)load_commands + sizeofcmds)
		printf("load command %u extends past end of load "
		       "commands\n", i);
	    left = sizeofcmds - ((char *)lc - (char *)load_commands);

	    switch(l.cmd){
	    case LC_SEGMENT:
		memset((char *)&sg, '\0', sizeof(struct segment_command));
		size = left < sizeof(struct segment_command) ?
		       left : sizeof(struct segment_command);
		memcpy((char *)&sg, (char *)lc, size);
		if(swapped)
		    swap_segment_command(&sg, host_byte_sex);

		if((filetype == MH_OBJECT && sg.segname[0] == '\0') ||
		   strncmp(sg.segname, segname, sizeof(sg.segname)) == 0){
                    *seg_addr = sg.vmaddr;
		    p = (char *)lc + sizeof(struct segment_command);
		    for(j = 0 ; found == FALSE && j < sg.nsects ; j++){
			if(p + sizeof(struct section) >
			   (char *)load_commands + sizeofcmds){
			    printf("section structure command extends past "
				   "end of load commands\n");
			}
			left = sizeofcmds - (p - (char *)load_commands);
			memset((char *)&s, '\0', sizeof(struct section));
			size = left < sizeof(struct section) ?
			       left : sizeof(struct section);
			memcpy((char *)&s, p, size);
			if(swapped)
			    swap_section(&s, 1, host_byte_sex);

			if(strncmp(s.sectname, sectname,
				   sizeof(s.sectname)) == 0 &&
			   strncmp(s.segname, segname,
				   sizeof(s.segname)) == 0){
			    found = TRUE;
			    cmd = LC_SEGMENT;
			    break;
			}

			if(p + sizeof(struct section) >
			   (char *)load_commands + sizeofcmds)
			    return(FALSE);
			p += size;
		    }
		}
		break;
	    case LC_SEGMENT_64:
		memset((char *)&sg64, '\0', sizeof(struct segment_command_64));
		size = left < sizeof(struct segment_command_64) ?
		       left : sizeof(struct segment_command_64);
		memcpy((char *)&sg64, (char *)lc, size);
		if(swapped)
		    swap_segment_command_64(&sg64, host_byte_sex);

		if((filetype == MH_OBJECT && sg64.segname[0] == '\0') ||
		   strncmp(sg64.segname, segname, sizeof(sg64.segname)) == 0){
                    *seg_addr = sg.vmaddr;
		    p = (char *)lc + sizeof(struct segment_command_64);
		    for(j = 0 ; found == FALSE && j < sg64.nsects ; j++){
			if(p + sizeof(struct section_64) >
			   (char *)load_commands + sizeofcmds){
			    printf("section structure command extends past "
				   "end of load commands\n");
			}
			left = sizeofcmds - (p - (char *)load_commands);
			memset((char *)&s64, '\0', sizeof(struct section_64));
			size = left < sizeof(struct section_64) ?
			       left : sizeof(struct section_64);
			memcpy((char *)&s64, p, size);
			if(swapped)
			    swap_section_64(&s64, 1, host_byte_sex);

			if(strncmp(s64.sectname, sectname,
				   sizeof(s64.sectname)) == 0 &&
			   strncmp(s64.segname, segname,
				   sizeof(s64.segname)) == 0){
			    found = TRUE;
			    cmd = LC_SEGMENT_64;
			    break;
			}

			if(p + sizeof(struct section_64) >
			   (char *)load_commands + sizeofcmds)
			    return(FALSE);
			p += size;
		    }
		}
		break;
	    }
	    if(l.cmdsize == 0){
		printf("load command %u size zero (can't advance to other "
		       "load commands)\n", i);
		break;
	    }
	    lc = (struct load_command *)((char *)lc + l.cmdsize);
	    if((char *)lc > (char *)load_commands + sizeofcmds)
		break;
	}
	if(found == FALSE)
	    return(FALSE);

	if(cmd == LC_SEGMENT){
	    if((s.flags & SECTION_TYPE) == S_ZEROFILL){
		*sect_pointer = NULL;
		*sect_size = s.size;
	    }
	    else{
		if(s.offset > object_size){
		    printf("section offset for section (%.16s,%.16s) is past "
			   "end of file\n", s.segname, s.sectname);
		}
		else{
		    *sect_pointer = object_addr + s.offset;
		    if(s.size > object_size ||
		       s.offset + s.size > object_size){
			printf("section (%.16s,%.16s) extends past end of "
			       "file\n", s.segname, s.sectname);
			*sect_size = object_size - s.offset;
		    }
		    else
			*sect_size = s.size;
		}
	    }
	    if(s.reloff >= object_size){
		printf("relocation entries offset for (%.16s,%.16s): is past "
		       "end of file\n", s.segname, s.sectname);
	    }
	    else{
		*sect_relocs = (struct relocation_info *)(object_addr +
							  s.reloff);
		if(s.reloff + s.nreloc * sizeof(struct relocation_info) >
								object_size){
		    printf("relocation entries for section (%.16s,%.16s) "
			   "extends past end of file\n", s.segname, s.sectname);
		    *sect_nrelocs = (object_size - s.reloff) /
				    sizeof(struct relocation_info);
		}
		else
		    *sect_nrelocs = s.nreloc;
	    }
	    *sect_addr = s.addr;
	    *sect_flags = s.flags;
	}
	else{
	    if((s64.flags & SECTION_TYPE) == S_ZEROFILL){
		*sect_pointer = NULL;
		*sect_size = s64.size;
	    }
	    else{
		if(s64.offset > object_size){
		    printf("section offset for section (%.16s,%.16s) is past "
			   "end of file\n", s64.segname, s64.sectname);
		}
		else{
		    *sect_pointer = object_addr + s64.offset;
		    if(s64.size > object_size ||
                       s64.offset + s64.size > object_size){
			printf("section (%.16s,%.16s) extends past end of "
			       "file\n", s64.segname, s64.sectname);
		    }
		    else
			*sect_size = s64.size;
		}
	    }
	    if(s64.reloff >= object_size){
		printf("relocation entries offset for (%.16s,%.16s): is past "
		       "end of file\n", s64.segname, s64.sectname);
	    }
	    else{
		*sect_relocs = (struct relocation_info *)(object_addr +
							  s64.reloff);
		if(s64.reloff + s64.nreloc * sizeof(struct relocation_info) >
								object_size){
		    printf("relocation entries for section (%.16s,%.16s) "
			   "extends past end of file\n", s64.segname,
			   s64.sectname);
		    *sect_nrelocs = (object_size - s64.reloff) /
				    sizeof(struct relocation_info);
		}
		else
		    *sect_nrelocs = s64.nreloc;
	    }
	    *sect_addr = s64.addr;
	    *sect_flags = s64.flags;
	}
	return(TRUE);
}

void
print_coff_reloc_section(
struct load_command *load_commands,
uint32_t ncmds,
uint32_t sizeofcmds,
uint32_t filetype,
enum byte_sex object_byte_sex,
char *object_addr,
uint32_t object_size,
#if defined(_WIN32) || defined(WIN32)
BOOL verbose)
#else
unsigned char verbose)
#endif
{
    UNREFERENCED_PARAMETER(verbose);

    char *sect, *p;
    uint64_t sect_addr, sect_size, size, seg_addr;
    int64_t left;
    struct relocation_info *sect_relocs;
    uint32_t sect_nrelocs, sect_flags, block_size;
    enum byte_sex host_byte_sex;
    unsigned char swapped;
    struct base_relocation_block_header h;
    struct base_relocation_entry b;

	printf("Contents of (__RELOC,__reloc) section\n");
	if(get_sect_info("__RELOC", "__reloc", load_commands,
	    ncmds, sizeofcmds, filetype, object_byte_sex,
	    object_addr, object_size, &sect, &sect_size, &sect_addr,
	    &sect_relocs, &sect_nrelocs, &sect_flags, &seg_addr) == TRUE){

	    host_byte_sex = get_host_byte_sex();
	    swapped = host_byte_sex != object_byte_sex;

	    p = sect;
	    left = sect_size;
	    while(left > 0){
		memset(&h, '\0', sizeof(struct base_relocation_block_header));
		size = left < sizeof(struct base_relocation_block_header) ?
		       left : sizeof(struct base_relocation_block_header);
		memcpy(&h, p, (size_t)size);
		if(swapped)
		    swap_base_relocation_block_header(&h, host_byte_sex);
		printf("Page RVA   0x%08x\n", h.page_rva);
		printf("Block Size 0x%08x\n", h.block_size);

		p += sizeof(struct base_relocation_block_header);
		left -= sizeof(struct base_relocation_block_header);
		block_size = sizeof(struct base_relocation_block_header);
		while(block_size < h.block_size && left > 0){
		    memset(&b, '\0', sizeof(struct base_relocation_entry));
		    size = left < sizeof(struct base_relocation_entry) ?
			   left : sizeof(struct base_relocation_entry);
		    memcpy(&b, p, (size_t)size);
		    if(swapped)
			swap_base_relocation_entry(&b, 1, host_byte_sex);
		    switch(b.type){
		    case IMAGE_REL_BASED_ABSOLUTE:
			printf("    Type   IMAGE_REL_BASED_ABSOLUTE\n");
			break;
		    case IMAGE_REL_BASED_HIGHLOW:
			printf("    Type   IMAGE_REL_BASED_HIGHLOW\n");
			break;
		    case IMAGE_REL_BASED_DIR64:
			printf("    Type   IMAGE_REL_BASED_DIR64\n");
			break;
		    default:
			printf("    Type   %u\n", b.type);
			break;
		    }
		    printf("    Offset 0x%0x\n", b.offset);

		    p += sizeof(struct base_relocation_entry);
		    left -= sizeof(struct base_relocation_entry);
		    block_size += sizeof(struct base_relocation_entry);
		}
	    }
	}
}
