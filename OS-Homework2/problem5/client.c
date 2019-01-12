/* (5)  Re-implement Q4, using mmap() to create a shared memory area between
 *      the client and the server.
 *
 *      ,ad8888ba,  88 88
 *     d8"'    `"8b 88 ""                         ,d
 *    d8'           88                            88
 *    88            88 88  ,adPPYba, 8b,dPPYba, MM88MMM
 *    88            88 88 a8P_____88 88P'   `"8a  88
 *    Y8,           88 88 8PP""""""" 88       88  88
 *     Y8a.    .a8P 88 88 "8b,   ,aa 88       88  88,
 *      `"Y8888Y"'  88 88  `"Ybbd8"' 88       88  "Y888
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
#include <ctype.h>

#define errmessage(m)                                                          \
	{                                                                      \
		perror(m);                                                     \
		exit(EXIT_FAILURE);                                            \
	}

/* uppercase -- convert str to uppercase. */
void uppercase(char *str);

const char file[] = "LoremIpsum.txt";

int main(int argc, char *argv[])
{
	int fd, fd_shm;
	char *addr_shm;
	size_t pgsize, mblock;
	FILE *fp;

	struct stat statbuf;

	/* Find page size. */
	if ((pgsize = sysconf(_SC_PAGE_SIZE)) < 0)
		errmessage("sysconf() error");
	char buf[pgsize];
	mblock = 10 * pgsize;

	/* Create file descriptor. */
	if ((fd = open(file, O_RDONLY)) < 0)
		errmessage("open() error");

	/* Determine file size from file descriptor
	 * (stored instatbuf.st_size) */
	if (fstat(fd, &statbuf) < 0)
		errmessage("fstat error");

	/* Create stream from file descriptor. */
	fp = fdopen(fd, "r");


	/*
	 * Open existing object
	 */
	if ((fd_shm = shm_open("shared_memory", O_RDWR, S_IRWXU)) == MAP_FAILED)
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

	/* Open tmp object for flagging the server reads.
	 */
	char *addr_mem;
	int fdes_mem;
	if ((fdes_mem = shm_open("temp", O_RDWR, 0)) == MAP_FAILED)
		errmessage("shm_open() error");
	if ((addr_mem = mmap(NULL, mblock, PROT_READ | PROT_WRITE, MAP_SHARED,
			     fdes_mem, 0))
	    == MAP_FAILED)
		close(fdes_mem);


	while (fgets(buf, pgsize, fp)) {
		uppercase(buf);
		memcpy(addr_shm, buf, pgsize);
		*addr_mem = 'R';
		while (*addr_mem != 'W')
			sleep(1);
		memset(addr_shm, '0', mblock);
	}

	memcpy(addr_mem, "STOP", sizeof("STOP"));

	return 0;

} /* main */

/* uppercase -- convert str to uppercase. */
void uppercase(char *str)
{
	char *s = str;
	while (*s) {
		*s = toupper((unsigned char)*s);
		s++;
	}
}
