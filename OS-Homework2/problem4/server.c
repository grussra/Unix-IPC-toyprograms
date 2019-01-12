/*
 * (4) Create a server program and a client program using SysV shared memory.
 *
 * Here the difficulty is that the client needs to find a way to let the server
 * knows that it has written something to the shared buffer.  You cannot just
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
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define errmessage(m) { perror(m); exit(EXIT_FAILURE); }

const char FILE_KEY[] = "/tmp";
const char FLAG_FILE_KEY[] = "LoremIpsum.txt";

const int SEG_SZ = 512;


struct info {
        key_t  key;
        int    seg_id;
        char  *p;
};

void getsharedmemory(struct info *data, const char *file);



int main()
{

        struct info data, flag;

        getsharedmemory(&data, FILE_KEY);
        getsharedmemory(&flag, FLAG_FILE_KEY);

        /*
         * Convention: the client will notify the server a write has occurred by
         * changing a symbol set in another area of shared memory (flag.p)
         */
        while (strcmp(flag.p, "STOP") != 0) { /* signals done. */
               switch (*flag.p) {
               case 'R':
                       printf("%s", data.p);

                       /* Notify client shared memory now ok to write to. */
                       *flag.p = 'W';
                       break;
               default:

                       break;
               }
        }
        shmdt(flag.p);
        shmdt(data.p);

        shmctl(data.seg_id, IPC_RMID, NULL);
        shmctl(flag.seg_id, IPC_RMID, NULL);

        exit(0);
}

void getsharedmemory(struct info *data, const char *file)
{
        /* Make the key. */
        if ((data->key = ftok(file, 1)) == -1)
                errmessage("ftok");

        /* Create a shared memory segment. */
        if ((data->seg_id = shmget(data->key, SEG_SZ, IPC_CREAT | 0666)) < 0)
                errmessage("shmget");

        /* Now we attach the segment to our data space. */
        if ((data->p = shmat(data->seg_id, NULL, 0)) == (char *) -1)
                errmessage("shmat");

}

