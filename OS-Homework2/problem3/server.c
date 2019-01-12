/*
 *             (3)  CREATE A SERVER PROGRAM AND A CLIENT PROGRAM.
 *
 * The client program reads a line from a text file, converts everything
 * to upper case letters, and send it to the server, using a named pipe. It
 * pauses for 1 second, and repeats the above process, until the end of file.
 *
 * The client sends a "Stop" string at the end.
 *
 * The server displays any thing it receives. The server quits when it
 * sees "Stop". The server needs to cleanup by deleting the named pipe.
 *
 * Your programs  (client and server) need to create the name pipe first,
 * if it does not exist.
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
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void errmessage(const char *str);

#define RWRWRW S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH
#define MAX_BUF 1024

int
main(int   argc,
     char *argv[])
{
        int fd;
        char *fifo = "/tmp/fifo";
        char buf[MAX_BUF];

        /* create the FIFO (named pipe) */
        if (mkfifo(fifo, RWRWRW) < 0)
                errmessage("mkfifo error");

        /* open, read, and display the message from the FIFO */
        fd = open(fifo, O_RDONLY);

	while (read(fd, buf, MAX_BUF), (strcmp(buf, "Stop") != 0))
                printf("Received: %s\n", buf);
        /* Delete fifo. */
        unlink(fifo);

        return 0;
}

/* errmessage -- report error and exit program. */
void errmessage(const char *str)
{
        perror(str);
        exit(EXIT_FAILURE);
}
