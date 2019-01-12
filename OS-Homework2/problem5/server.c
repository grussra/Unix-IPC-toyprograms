/* (5)  Re-implement Q4, using mmap() to create a shared memory area between
 *      the client and the server.
 *
 * The client program reads a line from a text file, converts everything to
 * upper case letters, and send it to the server. It pauses for 1 second, and
 * repeats the above process, until the end of file.
 *
 * The client sends a "Stop" string at the end.
 *
 * The server displays any thing it receives. The server quits when it
 * sees "Stop".
 *
 *     ad88888ba
 *    d8"     "8b
 *    Y8,
 *    `Y8aaaaa,    ,adPPYba, 8b,dPPYba, 8b       d8  ,adPPYba, 8b,dPPYba,
 *      `"""""8b, a8P_____88 88P'   "Y8 `8b     d8' a8P_____88 88P'   "Y8
 *            `8b 8PP""""""" 88          `8b   d8'  8PP""""""" 88
 *    Y8a     a8P "8b,   ,aa 88           `8b,d8'   "8b,   ,aa 88
 *     "Y88888P"   `"Ybbd8"' 88             "8"      `"Ybbd8"' 88
 *
 */
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h> /* mmap() is defined in this header */
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define errmessage(m) { perror(m); exit(EXIT_FAILURE); }

int
main(int argc, char *argv[])
{
        char         *addr_shm;
        int           fd_shm;
        size_t        pgsize, mblock;
        struct  stat  statbuf;

	if ((pgsize = sysconf(_SC_PAGE_SIZE)) < 0)
		errmessage("sysconf() error");
	mblock = 10 * pgsize;

	/* Create a named shared memory. */
	if ((fd_shm = shm_open("shared_memory", O_CREAT | O_RDWR,
			       S_IRWXU | S_IRWXG | S_IRWXO))
	    == MAP_FAILED)
		errmessage("shm_open() error");

        if (-1 != fstat(fd_shm, &statbuf) && statbuf.st_size == 0) {
                ftruncate(fd_shm, mblock);
        }

	/* if (ftruncate(fd_shm, mblock) == -1) */
	/* 	errmessage("ftruncate() error"); */

	if ((addr_shm = mmap(NULL, mblock, PROT_READ | PROT_WRITE, MAP_SHARED,
			     fd_shm, 0))
	    == MAP_FAILED)
		errmessage("mmap() error");

	if (close(fd_shm) == -1) /* fd_shm no longer needed. */
		errmessage("close() error");

	char *addr_mem;
	int fdes_mem;
	if ((fdes_mem = shm_open("temp", O_CREAT | O_RDWR,
				 S_IRWXU | S_IRWXG | S_IRWXO))
	    == MAP_FAILED)
		errmessage("shm_open() error");
        if (-1 != fstat(fdes_mem, &statbuf) && statbuf.st_size == 0) {
                ftruncate(fdes_mem, mblock);
        }
        /* if (ftruncate(fdes_mem, mblock) == -1) */
	/* 	errmessage("ftruncate() error"); */
	if ((addr_mem = mmap(NULL, mblock, PROT_READ | PROT_WRITE, MAP_SHARED,
			     fdes_mem, 0))
	    == MAP_FAILED)
		errmessage("mmap() error");
	close(fdes_mem);
	/*
	 * convention: the client will notify the server a write has occurred by
	 * changing a symbol set in another area of shared memory (flag.p)
	 */

	while (strcmp(addr_mem, "STOP")) {
		switch (*addr_mem) {
		case 'R':
			printf("%s", addr_shm);
			*addr_mem = 'W';
			break;

		default:
			break;
		}
	}

	shm_unlink("shared_memory");
	shm_unlink("temp");
	return 0;

} /* main */
