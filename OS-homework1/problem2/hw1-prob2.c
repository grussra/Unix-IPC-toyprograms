/*
 * (2) Write a Unix program that does the following
 *
 *      You run the program in command line using the following syntax:
 *
 *                       you_program_name file_name
 *
 *       Where file_name is the name of a text file under the current directory
 *
 *      When the program starts, it does the following
 *
 *      If filename is not specified in the command line, or if there are too
 *      many parameters, display the correct usage and then exit. Otherwise,
 *
 *      It forks three (3) child processes
 *
 *
 *      The parent process then displays its own PID information only once,
 *      then waits for all its child processes die (hint: use wait() for
 *      this.. The wait() system call suspends the calling process until
 *      one of its child processes dies.)
 *
 *
 *      Let one child-process run the "ls -l" command (using the "execl"
 *      system call); Let another child-process run the "ps -ef" command;
 *      Let the third child-process display the content of the file (specified
 *      by file_name). You can use the program "more" or "cat" to display it.
 *      After all child processes terminate, the main process displays "main
 *      process terminates" then exits.  This should be the last piece of
 *      information displayed by your program.
 */
#include <stdio.h>
#include <stdlib.h> /* exit() */
#include <ctype.h>  /* isdigit() */
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> /* fork(), sleep() */

int errmessage(const char *progname);

int main(int argc, char **argv)
{
	const char *progname = *argv;
	const char *fname = *++argv;
	pid_t pid;
	int status;

	/* Only a single argument is accepted; otherwise trigger usage message
	 * and exit. */
	if (argc != 2)
		errmessage(progname);

	/* Determine whether or not file exists */
	if (access(fname, F_OK) != -1) {
		/* File exists. */
	} else {
		/* Something went wrong while accessing file. */
		errmessage(progname);
	}

        printf("\n\nParent process PID is %i\n", getpid());
        /* ensures that "Parent Proc .." prints first. */
        sleep(1);

	/* Fork three (3) child processes */
	int i;
	for (i = 0; i < 3; ++i) {

		/* Create child process. */
		pid = fork();

		/* Parent process. */
		if (pid > 0) {


			wait(&status);
			continue;

		/* Child process. */
		} else if (pid == 0) {

			switch (i) {
			case 0:
				execl("/bin/ls", "ls", "-l", NULL);
				exit(EXIT_FAILURE);
			case 1:
				execl("/bin/ps", "ps", "-ef", NULL);
				exit(EXIT_FAILURE);
			case 2:
				execl("/bin/cat", "cat", fname, NULL);
				exit(EXIT_FAILURE);
			default:
				break;
			}

		/* Catch failure, print error message. */
		} else {
			printf("Error creating child process");
			break;
		}
	}

	printf("\nmain process terminates\n");
}

int errmessage(const char *progname)
{
	printf("correct usage: %s <integer>\n", progname);
	/* end program. */
	exit(EXIT_FAILURE);
}
