/*
 * HW1 (1) Write a Unix program that does the following
 *
 *      You run the program in command line using the following syntax:
 *
 *                       you_program_name N
 *
 *       Where N is a number
 *
 *      When the program starts, it does the following
 *
 *      1) If N is not specified in the command line, or the parameter is not
 *      a number, or if there are too many parameters, display the correct
 *      usage and then exit.
 *
 *      2) It  forks three (3) child processes
 *
 *          2a)  The main process then enters N loops that repeatedly displays
 *               one line of information like "This is the main process,
 *                 my PID is ...."  then pauses for about 2 seconds, then
 *                 displays the above information again
 *          2b)  Each of the three child processes repeatedly displays one
 *               line of information
 *                 like "This is a child process, my PID is ..., my parent
 *                 PID is ...", then pauses for about 2 seconds, then displays
 *                 the above information again, for a total of N times.
 *
 */
#include <stdio.h>
#include <stdlib.h>             /* exit() */
#include <ctype.h>              /* isdigit() */
#include <sys/types.h>
#include <unistd.h>             /* fork(), sleep() */

int errmessage(const char *progname); /* print usage info. */
void loop(const pid_t pid, const int upbound);

int main(int argc, char **argv)
{
	const char *progname = *argv; /* program name. */
	const char *arg = *++argv;    /* program argument. */
	int N;			      /* integer equivalent of *arg */

	/* Part 1.1 input verification. */

	/* Only a single argument is accepted; otherwise trigger usage message
	 * and exit. */
	if (argc != 2)
		errmessage(progname);

	/* Verify that single argument is strictly numeric; otherwise trigger
	 * usage message and exit. */
	for (; **argv != '\0'; ++*argv) {
		if (!isdigit(**argv))
			errmessage(progname);
	}

	/* finished with verifying input arguments. Since argument is numeric,
	 * store it as an integer. */
	N = atoi(arg);

	/* Part 1.2 processes */

	pid_t pid;
	pid_t children[3];
	int status;

	/* Fork three (3) child processes */
	int i;
	for (i = 0; i < 3; ++i) {

                /* Create child process. */
		pid = fork();
		/* If Parent process... */
		if (pid > 0) {
                        /* Wait for each child process to finish. */
			children[i] = wait(&status);
			continue;

		/* Child process test. */
		} else if (pid == 0) {
                        /* Part 1.2b */
                        loop(pid, N), sleep(2);
                        loop(pid, N), sleep(2);

                        exit(EXIT_SUCCESS);

		/* Catch failure, print error message. */
		} else {
			printf("Error creating child process");
			break;
		}
	}

	/* main process only past this point. */

        /* Part 1.2a */
        loop(pid, N), sleep(2);
        loop(pid, N);
}

int errmessage(const char *progname)
{
	printf("correct usage: %s <integer>\n", progname);
	/* end program. */
	exit(EXIT_FAILURE);
}

void loop(const pid_t pid, const int upbound)
{
        int i;
        for (i = 0; i < upbound; ++i) {
                if (pid == 0) {
                        printf("This is the child process, my PID is %i "
                               "my parent process is %i\n",
                               getpid(), getppid());
                } else {
                        printf("This is the main process, my PID is %i\n",
                               getpid());
                }
        }
}
