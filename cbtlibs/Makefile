RC_OS = macos
OFLAG = -Os
ifneq "" "$(SDKROOT)"
  SDK = -isysroot $(SDKROOT)
  CC = $(shell xcrun -find -sdk $(SDKROOT) cc)
  LIBTOOL = $(shell xcrun -find -sdk $(SDKROOT) libtool)
  AR = $(shell xcrun -find -sdk $(SDKROOT) ar)
  RANLIB = $(shell xcrun -find -sdk $(SDKROOT) ranlib)
else
  LIBTOOL = libtool
  AR = ar
  RANLIB = ranlib
endif
	
CFLAGS = $(OFLAG) -g -I$(SRCROOT)/../include -Wall $(SDK) -fPIC
MKDIRS = /bin/mkdir -p
SRCROOT = .
SYMROOT = .
OBJROOT = .
OFILE_DIR = $(OBJROOT)
VPATH = $(OFILE_DIR)

CFILES = libsyminfo.c
OBJS = $(CFILES:.c=.o)
INSTALL_FILES = $(CFILES) Makefile notes
LOCLIBDIR = /usr/local/lib
LIBSTUFF = -L$(SYMROOT)/../libstuff -lstuff

all: $(OFILE_DIR) $(SYMROOT) lib_ofiles

lib_ofiles: $(OFILE_DIR) $(SYMROOT) libsyminfo.a libsyminfo.so

.c.o:
	$(CC) $(CFLAGS) $(RC_CFLAGS) $(COPTS) -c -o $(OFILE_DIR)/$*.o $<

clean shlib_clean:
	-rm -r -f $(OFILE_DIRS)
	-rm -f libsyminfo.a libsyminfo.o libsyminfo.o.tmp
	-rm -r -f *dSYM

install: all

installsrc:
	$(MKDIRS) $(SRCROOT)
	chmod 755 $(SRCROOT)
	gnutar cf - $(INSTALL_FILES) | (cd $(SRCROOT); gnutar xf -)
	chmod 444 $(SRCROOT)/*

$(SYMROOT):
	$(MKDIRS) $@

libsyminfo.o: libsyminfo.c
	$(CC) $(CFLAGS) $(RC_CFLAGS) -c \
		-o $(OFILE_DIR)/$*.o $(SRCROOT)/libsyminfo.c -DLIBRARY_API

libsyminfo.so: libsyminfo.o
	$(CC) $(CFLAGS) $(RC_CFLAGS) -shared -o $(SYMROOT)/libsyminfo.so libsyminfo.o $(LIBSTUFF)

libsyminfo.a: libsyminfo.o
	$(AR) cr -o $(SYMROOT)/libsyminfo.a \
		libsyminfo.o
	$(RANLIB) $(SYMROOT)/libsyminfo.a
#	$(LIBTOOL) -static -o $(SYMROOT)/libsyminfo.a \
#		$(OBJROOT)/libsyminfo.o.tmp


lib_ofiles_install: lib_ofiles
	$(MKDIRS) $(DSTROOT)$(LOCLIBDIR)
	install -p -c -m 444 $(SYMROOT)/libsyminfo.a \
		$(DSTROOT)$(LOCLIBDIR)/libsyminfo.a
	$(RANLIB) $(DSTROOT)$(LOCLIBDIR)/libsyminfo.a

$(OFILE_DIR) $(SYMROOT):
	$(MKDIRS) $@

