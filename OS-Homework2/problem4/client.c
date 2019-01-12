/*
 * (4) Create a server program and a client program using SysV shared memory.
 *
 * Here the difficulty is that the client needs to find a way to let the server
 * know that it has written something to the shared buffer.  You cannot just
 * modify the buffer and hope the server will find out --- what will happen if
 * the new line of text is identical to the contents already in the buffer? (I
 * want you to test this by addling two identical lines in the testing file)
 *
 * In the examples I showed you in the class, I used sleep(), but clearly it
 * is not an elegant way to things and it may fail.
 *
 * I suggest you reserve an area in the shared memory section (or create a
 * new shared memory section) to hold an integer.  Each time AFTER the client
 * updates the text buffer, its increments this value. Server will notice
 * the change.
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
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#define errmessage(m) { perror(m); exit(EXIT_FAILURE); }

/* uppercase -- convert str to uppercase. */
void uppercase(char *str);

const char FILE_KEY[] = "/tmp";
const char FLAG_FILE_KEY[] = "LoremIpsum.txt";

const int SEG_SZ = 512;

struct info {
        key_t  key;
        int    seg_id;
        char  *p;
};

void getsharedmemory(struct info *data, const char *file)
{
        /* Make the key: */
        if ((data->key = ftok(file, 1)) == -1)
                errmessage("ftok");

        /* Locate the segment. */
        if ((data->seg_id = shmget(data->key, SEG_SZ, 0666)) < 0)
                errmessage("shmget");

        /* Now we attach the segment to our data space. */
        if ((data->p = shmat(data->seg_id, NULL, 0)) == (char *) -1)
                errmessage("shmat");

}
int main()
{

        struct info data, flag;

        getsharedmemory(&data, FILE_KEY);
        getsharedmemory(&flag, FLAG_FILE_KEY);

        /*
         * Now put some things into the memory for the other process to read.
         */
        FILE *fp;
        char  buf[SEG_SZ];
        char *s;

        /* Open text file for reading. */
        if (!(fp = fopen("LoremIpsum.txt", "r")))
                errmessage("fopen error");
        size_t offset = 0;
        s = data.p;
        while (fgets(buf, SEG_SZ, fp)) {
                uppercase(buf); /* buf now all uppercase. */
                strcpy(s, buf);
                *flag.p = 'R';         /* notify server that read is okay */
                while (*flag.p != 'W') /* wait until server allows read. */
                        sleep(1);
                memset(s, '\0', SEG_SZ); /* Cleanup for next write. */
        }


        /*
         * Finally, write "STOP" to segment to indicate we are finished.
         */
        strcpy(flag.p, "STOP");

        shmdt(flag.p);
        shmdt(data.p);

        shmctl(data.seg_id, IPC_RMID, NULL);
        shmctl(flag.seg_id, IPC_RMID, NULL);

        exit(0);
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
