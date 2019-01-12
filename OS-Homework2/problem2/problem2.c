/*
 * (2) Write a program that uses popen() to do the following:
 *
 *
 *   The program recursively finds all the ".c" files under the current directly
 *   (you have to search all the subdirectories also).
 *
 *    It sends all the file names it found in an email message to an email
 *    address provided via the command line.  The subject line must contains
 *    the number of files found.
 *
 *   hints:
 *
 *      (a) Create two pipes
 *
 *      (b)  use mailx to send email
 *
 *      (c)   use find to find files. Check the following:
 *
 *             https://kb.iu.edu/d/admm
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

/* getbuffer -- Stores buffer of text from stream. */
/*           -- On success, returns the number of characters read. */
ssize_t getbuffer(char **content, size_t *n, FILE *stream);

void errmessage(const char *str);

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "usage: %s <email@example.com>\n", *argv);
		exit(EXIT_FAILURE);
	}

	const char *name = *argv, *toemail = *++argv;
	size_t bufsize, charactersread;
	char *foundbuf = NULL, *sendemail = NULL;
	FILE *findstream;

	/* Find all .c files in present directory and subdirectories. */
	if (!(findstream = popen("find . -name \"*.c\"", "r")))
		errmessage("popen error");
	fflush(findstream);

	/* Store foundbuf .c files in foundbuf buffer. */
	if ((charactersread = getbuffer(&foundbuf, &bufsize, findstream)) < 0)
		perror("getbuffer error");

	int filecount = 0; /* Count number of files foundbuf. */
	char *mover = foundbuf;
	for (; (mover = strpbrk(mover, "\n")); mover++)
		filecount++;


	int length = snprintf(NULL, 0, "echo \"%s\" | mailx -s \"%i\" %s",
			      foundbuf, filecount, toemail);
	char *str = malloc(length + 1);
	snprintf(str, length + 1, "echo \"%s\" | mailx -s \"%i\" %s", foundbuf,
		 filecount, toemail);


	popen(str, "w");

	pclose(findstream);


	return EXIT_SUCCESS;
}
void errmessage(const char *str)
{
	perror(str);
	exit(EXIT_FAILURE);
}
/* getbuffer -- On success, returns the number of characters read. */
ssize_t
getbuffer(char **content, /* input/output - storage address for input stream */
	  size_t *n,      /* input/output - string bufsize in bytes */
	  FILE *stream)   /* input - text stream */
{
	char *buffer_tmp, *position;
	size_t block, offset;
	int c;

	if (!stream || !content || !n) {
		errno = EINVAL;
		return -1;
		/* Minimum length for strings is 2 bytes. */
	} else if (*content == NULL || *n <= 1) {
		*n = 128;
		if (!(*content = malloc(*n))) {
			errno = ENOMEM;
			return -1;
		}
	}
	block = *n;
	position = *content;

	while ((c = fgetc(stream)) != EOF) {
		*position++ = c;
		offset = position - *content;
		if (offset >= *n) {
			buffer_tmp = realloc(*content, *n += block);
			if (!buffer_tmp) { /* Do not free. Return *content. */
				errno = ENOMEM;
				return -1;
			}
			*content = buffer_tmp;
			position = *content + offset;
		}
	}
	*position = '\0';
	return (position - *content - 1);
}
