/*
 *                                 *** HW2 ***
 *
 * (1) Write a multi-processed C program that does the following:
 *
 * The program takes two command line parameters. The first one is a program
 * name; the second one is a file name. If the user enters a wrong number of
 * parameters, display a “usage” message and exit.
 *
 * The program creates the child process, using fork(), and runs the specified
 * program in the child process, using exec(). You also need to figure out
 * a way to redirect the standard output file of the child process to the file.
 *
 * For example, if your program name is “myprg1”, then the following
 * command line will run “/bin/ps” as a new process and redirect the
 * output to “myresult.txt”:
 *
 *      myprg1 /bin/ps   myresult.txt
 *
 * Check the lecture slides to see how to redirect stdin/stdout (hints:
 * use dup() or similar system calls).
 *
 * You can find the full pathname of any unix command by typing “which
 * program_name” in command line.
 */

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define MAX_BUF 80

void
run_cmd(int fd, const char *cmd);

void
err_msg(const char *str);

char *
get_cmd_path(const char *cmd);

int
main(int argc, char *argv[])
{
        const char *name = *argv, *cmd = *++argv, *file = *++argv;

        int file_des;

        if (argc != 3) {
                fprintf(stderr, "usage: %s <program name> <file name>\n", name);
                exit(1);
        }

        if ((file_des = open(file,
                             O_RDWR | O_CREAT | O_APPEND,
                             S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
                err_msg(file);

        run_cmd(file_des, cmd);

        return EXIT_SUCCESS;
}

void
run_cmd(int fd, const char *cmd)
{
        pid_t status;
        /* ***Must use switch statement.***
         * otherwise junk gets written to <file name>. */
        switch (fork()) {
        case 0:                 /* Child process. */
                /* fd becomes the standard output */
                dup2(fd, STDOUT_FILENO);

                /* Run <program name> */
                if (execl(get_cmd_path(cmd), cmd, NULL) < 0)
                        err_msg("execl() error");
        default:                              /* Parent process. */
                while (wait(&status) != -1) ; /* pick up dead children */
                break;
        case -1:
                err_msg("fork error");
        }
        return;
}

char *get_cmd_path(const char *cmd)
{
	FILE *fp;
	char *cmdstring =
		(char *)malloc(sizeof(char *) * (strlen(cmd) + strlen("which ")));
	char *cmdpath = (char *)malloc(sizeof(char *) * MAX_BUF);

	/* Create a string for finding <program name>'s path. */
	strcat(cmdstring, "which ");
        strcat(cmdstring, cmd);

        /* Execute "which <program name>". */
        fp = popen(cmdstring, "r");
        if (!fp)
                err_msg("popen() failed");

        /* fp stream gives system path to <program name>. */
        fgets(cmdpath, MAX_BUF, fp);
        if (ferror(fp))
                err_msg("fgets failed");
        /* Close file pointer. */
        pclose(fp);

        /* Remove newline character. */
        cmdpath[strcspn(cmdpath, "\n")] = '\0';

        return cmdpath;
}

void err_msg(const char *str)
{
        perror(str);
        exit(EXIT_FAILURE);
}
