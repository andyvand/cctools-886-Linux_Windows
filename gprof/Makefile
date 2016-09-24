ifneq "" "$(SDKROOT)"
  SDK = -isysroot $(SDKROOT)
  CC = $(shell xcrun -find -sdk $(SDKROOT) cc)
  DSYMUTIL = $(shell xcrun -find -sdk ${SDKROOT} dsymutil)
else
  DSYMUTIL = dsymutil
endif

OFLAG = -Os
CFLAGS = $(OFLAG) -g -I../include -Wall -DNeXT_MOD -DDEBUG $(SDK) -fPIC -fPIE
LIBSTUFF = -L$(SYMROOT)/../libstuff -lstuff_p -lc -ldl
MKDIRS = /bin/mkdir -p
OBJROOT = .
OFILE_DIR = $(OBJROOT)
VPATH = $(OFILE_DIR)
SYMROOT = .

BINDIR = /Developer/usr/ucb
USRBINDIR = /Developer/usr/bin
LIBDIR    = /Developer/usr/share

HFILES = gprof.h m68k.h vax.h
CFILES = gprof.c arcs.c dfn.c lookup.c calls.c hertz.c printgprof.c \
	 printlist.c getnfile.c scatter.c
OBJS = $(CFILES:.c=.o)
INSTALL_FILES = $(CFILES) $(HFILES) gprof.flat gprof.callg Makefile notes

PRODUCT = gprof
all: $(OFILE_DIR) $(SYMROOT) $(PRODUCT).NEW

$(PRODUCT).NEW:	$(OFILE_DIR) $(SYMROOT) $(OBJS)
	$(CC) $(CFLAGS) $(RC_CFLAGS) $(SDK) -o $(SYMROOT)/$@ $(OBJS) $(LIBSTUFF)
#	$(DSYMUTIL) $(SYMROOT)/$@

.c.o:
	$(CC) $(CFLAGS) $(RC_CFLAGS) -c -o $(OFILE_DIR)/$*.o $<

clean: shlib_clean
	-rm -f $(SYMROOT)/$(PRODUCT).NEW
	-rm -f $(SYMROOT)/$(PRODUCT).NEW.private.o
	-rm -r -f *dSYM

shlib_clean:
	-cd $(OFILE_DIR); rm -f $(OBJS)
	-rm -f make.out

install: all
# No longer install gprof and its files.
#	$(MKDIRS) $(DSTROOT)$(USRBINDIR)
#	install -c -s -m 555 $(SYMROOT)/$(PRODUCT).NEW \
#		$(DSTROOT)$(USRBINDIR)/$(PRODUCT)
#	$(MKDIRS) $(DSTROOT)$(LIBDIR)
#	install -c -m 444 gprof.flat $(DSTROOT)$(LIBDIR)
#	install -c -m 444 gprof.callg $(DSTROOT)$(LIBDIR)

installsrc:
	$(MKDIRS) $(SRCROOT)
	chmod 755 $(SRCROOT)
	gnutar cf - $(INSTALL_FILES) | (cd $(SRCROOT); gnutar xf -)
	chmod 444 $(SRCROOT)/*

$(OFILE_DIR) $(SYMROOT):
	$(MKDIRS) $@
