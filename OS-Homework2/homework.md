HW2 Note : this assignment is worth 250 points! Each program is 50 points.


(1) Write a multi-processed C program that does the following:

The program takes two command line parameters. The first one is a program
name; the second one is a file name. If the user enters a wrong number of
parameters, display a “usage” message and exit.

The program creates the child process, using fork(), and run the specified
program in the child process, using exec(). You also need to figure out
a way to redirect the standard output file of the child process to the file.

For example, if your program name is “myprg1”, then the following
command line will run “/bin/ps” as a new process and redirect the
output to “myresult.txt”:

     myprg1 /bin/ps   myresult.txt

Check the lecture slides to see how to redirect stdin/stdout (hints:
use dup() or similar system calls).

You can find the full pathname of any unix command by typing “which
program_name” in command line.


(2) Write a program that uses popen() to do the following:


  The program recursively find all the ".c" files under the current directly
  (you have to search all the subdirectories also).

   It sends all the file names it found in an email message to an email
   address provided via the command line.  The subject line must contains
   the number of files found.

  hints:

     (a) Create two piles

     (b)  use mailx to send email

     (c)   use find to find files. Check the following:

            https://kb.iu.edu/d/admm


(3)  Create a server program and a client program.   The client program
reads a line from a  text file, converts everything to upper case letters,
and send it to the server, using a named pipe. It pauses for 1 second,
and repeats the above process, until the end of file.

The client sends a "Stop" string at the end.

     The server displays any thing it receives. The server quits when it
     sees "Stop". The server needs to cleanup by deleting the named pipe.

Your programs  (client and server) need to create the name pipe first,
if it does not exist.


(4)  Re-implement Q3, using SysV shared memory.

Here the difficult is that the client needs to find a way to let the server
knows that its have written something to the shared buffer.  You cannot just
modify the buffer and hope the server will find out --- what will happen if
the new line of text is identical to the contents already in the buffer? (I
want you to test this by addling two identical lines in the testing file)

In the examples I showed you in the class, I used sleep(), but clearly it
is not an elegant way to things and it may fail.

I suggest you reserve an area in the shared memory section (or create a
new shared memory section) to hold an integer.  Each time AFTER the client
updates the text buffer, its increments this value. Server will notice
the change.


(5)  Re-implement Q4, using mmap() to create a shared memory area between
the client and the server.
