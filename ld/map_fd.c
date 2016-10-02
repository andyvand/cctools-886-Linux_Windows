#include <mach/mach.h>
#include <mach/mach_error.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>

#if defined(__linux__)
#include <linux/xattr.h>
#else
#include <sys/attr.h>
#endif

#include <errno.h>

kern_return_t map_fd(int fd,
                     vm_offset_t offset,
                     vm_offset_t *va,
                     boolean_t findspace,
                     vm_size_t size)
{

  void *addr = NULL;

  addr = mmap(0, size, PROT_READ|PROT_WRITE,
	      MAP_PRIVATE|MAP_FILE, fd, offset);

  if(addr == (void *)-1) {
    return 1;
  }

  *va = (vm_offset_t)addr;

  return 0;
}


