export USE_APPLE_PB_SUPPORT = all

EFITOOLS=efitools

ifeq "$(EFITOOLS)" "efitools"
	EFI_SUPPORT = -DEFI_SUPPORT
endif

ifneq "" "$(SDKROOT)"
  SDK = -isysroot $(SDKROOT)
  CC = $(shell xcrun -find -sdk $(SDKROOT) cc)
  CXX = $(shell xcrun -find -sdk $(SDKROOT) c++)
  DSYMUTIL = $(shell xcrun -find -sdk ${SDKROOT} dsymutil)
  AR = $(shell xcrun -find -sdk ${SDKROOT} ar)
  RANLIB = $(shell xcrun -find -sdk ${SDKROOT} ranlib)
else
  DSYMUTIL = dsymutil
  AR = ar
  RANLIB = ranlib
endif

OFLAG = -Os -fPIC
LTO = -DLTO_SUPPORT
# apply otool-ng patches
OTOOL_NG = -DOTOOL_NG_SUPPORT
LEGACY = -D_MACH_I386_THREAD_STATUS_FPSTATE_LEGACY_FIELD_NAMES_ \
	 -D_ARCHITECTURE_I386_FPU_FPSTATE_LEGACY_FIELD_NAMES_
CFLAGS = $(OFLAG) $(LTO) $(OTOOL_NG) -g -Wall -I$(SRCROOT)/../include \
	 -I$(SRCROOT)  -I/usr/include/llvm-c-3.8 -I/usr/include/llvm-3.8 $(LEGACY) $(EFI_SUPPORT) $(SDK)

LIBSTUFF = $(SRCROOT)/../libstuff/libstuff.a
LIBSTUFFBIN = -L$(SRCROOT)/../libstuff -lstuff
CXXLIB = -lc -ldl

MKDIRS = /bin/mkdir -p
SRCROOT = .
SYMROOT = .
OBJROOT = .
OFILE_DIR = $(OBJROOT)
VPATH = $(OFILE_DIR)

BINDIR = /Developer/bin
USRBINDIR = /Developer/usr/bin

CFILES = main.c ofile_print.c m68k_disasm.c i860_disasm.c \
	 m88k_disasm.c i386_disasm.c ppc_disasm.c hppa_disasm.c \
	 sparc_disasm.c print_objc.c notify.c arm_disasm.c \
	 print_objc2_64bit.c print_objc2_32bit.c dyld_bind_info.c \
	 arm64_disasm.c print_bitcode.c
LIBCFILES = ofile_print.c m68k_disasm.c i860_disasm.c \
	 m88k_disasm.c i386_disasm.c ppc_disasm.c hppa_disasm.c \
	 sparc_disasm.c print_objc.c notify.c arm_disasm.c \
	 print_objc2_64bit.c print_objc2_32bit.c dyld_bind_info.c arm64_disasm.c \
	../libstuff/SymLoc.c ../libstuff/allocate.c ../libstuff/arch.c \
	../libstuff/arch_usage.c ../libstuff/best_arch.c ../libstuff/breakout.c \
	../libstuff/bytesex.c ../libstuff/checkout.c ../libstuff/coff_bytesex.c \
	../libstuff/crc32.c ../libstuff/dylib_roots.c ../libstuff/dylib_table.c \
	../libstuff/dynamic_obj/apple_version.c ../libstuff/errors.c ../libstuff/execute.c \
	../libstuff/fatal_arch.c ../libstuff/fatals.c ../libstuff/get_arch_from_host.c \
	../libstuff/get_toc_byte_sex.c ../libstuff/guess_short_name.c ../libstuff/hash_string.c \
	../libstuff/hppa.c ../libstuff/llvm.c ../libstuff/lto.c \
	../libstuff/macosx_deployment_target.c ../libstuff/ofile.c ../libstuff/ofile_error.c \
	../libstuff/ofile_get_word.c ../libstuff/print.c ../libstuff/profile_obj/apple_version.c \
	../libstuff/reloc.c ../libstuff/rnd.c ../libstuff/seg_addr_table.c \
	../libstuff/set_arch_flag_name.c \
	../libstuff/static_obj/apple_version.c ../libstuff/swap_headers.c ../libstuff/symbol_list.c \
	../libstuff/unix_standard_mode.c ../libstuff/version_number.c ../libstuff/vm_flush_cache.c \
	../libstuff/writeout.c

ifeq "$(EFITOOLS)" "efitools"
	CFILES += coff_print.c
endif
HFILES = otool.h ofile_print.h m68k_disasm.h i860_disasm.h \
	 sparc_disasm.h m88k_disasm.h i386_disasm.h ppc_disasm.h hppa_disasm.h \
	 notify.h arm_disasm.h dyld_bind_info.h arm64_disasm.h cxa_demangle.h
OBJS = $(CFILES:.c=.o) ofile.o
LIBOBJS = $(LIBCFILES:.c=.o) libmain.o
INSTALL_FILES = $(CFILES) $(HFILES) Makefile notes

LIBRARY = libdisasm.a
PRODUCT = otool
all: $(OFILE_DIR) $(SYMROOT) $(PRODUCT).NEW llvm-otool.NEW  $(LIBRARY)

$(LIBRARY): $(OFILE_DIR) $(LIBOBJS)
	$(AR) cr $(SYMROOT)/$@ $(LIBOBJS)
	rm -Rf stuff
	$(RANLIB) $@

$(PRODUCT).NEW:	$(OFILE_DIR) $(SYMROOT) $(OBJS)
	$(CXX) $(RC_CFLAGS) $(SDK) -o $(SYMROOT)/$@ $(OBJS) $(LIBSTUFF) $(CXXLIB)
#	$(DSYMUTIL) $(SYMROOT)/$@

llvm-otool.NEW:	$(OFILE_DIR) $(SYMROOT) llvm_otool_main.o
	$(CXX) $(RC_CFLAGS) $(SDK) -o $(SYMROOT)/$@ llvm_otool_main.o $(LIBSTUFF) $(CXXLIB)
#	$(DSYMUTIL) $(SYMROOT)/$@

libmain.o: main.c
	$(CC) $(CFLAGS) $(RC_CFLAGS) -D_BUILD_LIB_=1 -c -o $(OFILE_DIR)/$@ $<

llvm.o: $(SRCROOT)/../libstuff/llvm.c
	$(CC) $(CFLAGS) $(RC_CFLAGS) -c -o $(OFILE_DIR)/$*.o \
		$(SRCROOT)/../libstuff/ofile.c -DOTOOL -D_BUILD_LIB_

llvm_otool_main.o: main.c
	$(CC) $(CFLAGS) $(RC_CFLAGS) -DLLVM_OTOOL -c -o $(OFILE_DIR)/$*.o $<

.c.o:
	$(CC) $(CFLAGS) $(RC_CFLAGS) -c -o $(OFILE_DIR)/$*.o $<

ofile.o: $(SRCROOT)/../libstuff/ofile.c
	$(CC) $(CFLAGS) $(RC_CFLAGS) -c -o $(OFILE_DIR)/$*.o \
		$(SRCROOT)/../libstuff/ofile.c -DOTOOL

clean: shlib_clean
	-rm -f $(SYMROOT)/$(PRODUCT).NEW $(SYMROOT)/llvm-otool.NEW
	-rm -r -f *dSYM

shlib_clean:
	-cd $(OFILE_DIR); rm -f llvm_otool_main.o $(OBJS) private.o \
		llvm-otool-private.o
	-rm -f make.out

install: all
	$(MKDIRS) $(DSTROOT)$(USRBINDIR)
	install -c -s -m 555 $(SYMROOT)/$(PRODUCT).NEW \
		$(DSTROOT)$(USRBINDIR)/$(PRODUCT)-classic
	install -c -s -m 555 $(SYMROOT)/llvm-otool.NEW \
		$(DSTROOT)$(USRBINDIR)/llvm-otool
	(cd $(DSTROOT)$(USRBINDIR); rm -f otool; ln -s otool-classic otool)

installsrc:
	$(MKDIRS) $(SRCROOT)
	chmod 755 $(SRCROOT)
	gnutar cf - $(INSTALL_FILES) | (cd $(SRCROOT); gnutar xf -)
	chmod 444 $(SRCROOT)/*

$(OFILE_DIR) $(SYMROOT):
	$(MKDIRS) $@

