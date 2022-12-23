#ifndef _EXEC_PAGE_H
#define _EXEC_PAGE_H

#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>

#define PAGE_START(P) ((intptr_t)(P) & ~(pagesize - 1))
#define PAGE_END(P) (((intptr_t)(P) + pagesize - 1) & ~(pagesize - 1))

	/* The execpage() function shall change the specified memory pages
	 * permissions into executable.
	 *
	 * void *ptr  = pointer to start of memory buff
	 * size_t len = memory buff size in bytes
	 *
	 * The function returns 0 if successful and -1 if any error is encountered.
	 * errono may be used to diagnose the error.
	 */
	static int execpage(void *ptr, size_t len) {
		int ret;

		const long pagesize = sysconf(_SC_PAGE_SIZE);
		if (pagesize == -1)
			return -1;

		ret = mprotect((void *)PAGE_START(ptr),
						 PAGE_END((intptr_t)ptr + len) - PAGE_START(ptr),
						 PROT_READ | PROT_WRITE | PROT_EXEC);
		if (ret == -1)
			return -1;

		return 0;
	}

#undef PAGE_START
#undef PAGE_END

#endif

