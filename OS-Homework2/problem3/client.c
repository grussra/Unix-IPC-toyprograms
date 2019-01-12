/*
 *             (3)  CREATE A SERVER PROGRAM AND A CLIENT PROGRAM.
 *
 * The client program reads a line from a text file, converts everything
 * to uppercase case letters, and send it to the server, using a named pipe. It
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

#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void errmessage(const char *str);
void uppercase(char *str);

#define MAX_BUF 1024
#define RWRWRW S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH

int
main(int   argc,
     char *argv[])
{
        FILE *fp;
        int fd;
        char *fifo = "/tmp/fifo";
        char buf[MAX_BUF], capitalbuf[MAX_BUF];

        /* Open text file for reading. */
        if (!(fp = fopen("LoremIpsum.txt", "r")))
                errmessage("fopen error");

        /* Open FIFO for writing text file contents. */
        fd = open(fifo, O_WRONLY);

        while (fgets(buf, MAX_BUF, fp)) {
                uppercase(buf);
                write(fd, buf, sizeof(buf));
                sleep(1);
        }
        write(fd, "Stop", sizeof("Stop"));
        close(fd);


        return 0;
}

/* errmessage -- report error and exit program. */
void errmessage(const char *str)
{
        perror(str);
        exit(EXIT_FAILURE);
}
/* uppercase -- convert str to uppercase. */
void uppercase(char *str)
{
        char *s = str;
        while (*s) {
                *s = toupper((unsigned char) *s);
                s++;
        }
}
