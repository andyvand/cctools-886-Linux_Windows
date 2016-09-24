/* <dir.h> -- definitions for 4.2BSD-compatible directory access.
   last edit:	09-Jul-1983	D A Gwyn. */

#if defined (VMS)
#  if !defined (FAB$C_BID)
#    include <fab.h>
#  endif
#  if !defined (NAM$C_BID)
#    include <nam.h>
#  endif
#  if !defined (RMS$_SUC)
#    include <rmsdef.h>
#  endif
#  include "dir.h"
#endif /* VMS */

/* Size of directory block. */
#ifndef DIRBLKSIZ
#define DIRBLKSIZ 512
#endif /* DIRBLKSIZ */

/* NOTE:  MAXNAMLEN must be one less than a multiple of 4 */

#if defined (VMS)
#  define MAXNAMLEN (DIR$S_NAME + 7)	/* 80 plus room for version #.  */
#  define MAXFULLSPEC NAM$C_MAXRSS	/* Maximum full spec */
#else
# if defined(MAX_PATH)
#  define MAXNAMLEN MAX_PATH
 #else
#  define MAXNAMLEN 15			/* Maximum filename length. */
# endif
#endif /* VMS */

/* Data from readdir (). */
typedef struct direct {
  long d_ino;			/* Inode number of entry. */
  unsigned short d_reclen;	/* Length of this record. */
  unsigned short d_namlen;	/* Length of string in d_name. */
  char d_name[MAXNAMLEN + 1];	/* Name of file. */
} direct;

/* Stream data from opendir (). */
typedef struct DIR {
  int dd_fd;			/* File descriptor. */
  int dd_loc;			/* Offset in block. */
  int dd_size;			/* Amount of valid data. */
  char	dd_buf[DIRBLKSIZ];	/* Directory block. */
} DIR;

extern DIR *opendir (const char *name);
extern struct direct *readdir ();
extern long telldir (DIR *dir);
extern void seekdir(DIR *dir, size_t offset);
extern void closedir(DIR *dir);

#ifndef rewinddir
#define rewinddir(dirp) seekdir (dirp, 0)
#endif
