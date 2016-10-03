***************************
** README file for Lab 1 **
***************************

myshell.c :: Justin Shapiro
CSCI 3453 :: October 3rd, 2016 

myshell.c implements a basic shell in the C programming language.
C was used for the implementation rather than C++ since system commands fork() and execvp() were written for C and using C++ seems to complicate things.

myshell is an infinitely looping program that can only be stopped by the user's "exit" command. 
At the end of each command execution, the following line will always be there (unless "exit" was the previous command):

myshell> 

myshell can run any command listed in /bin/.
Commands not listed in /bin/ such as "cd" and "exit" are also supported.
The user can redirect command output to a file using the ">" operator.
The user can also read-in a file to a command using the "<" operator.
It is also possible for the user to pipe two commands together using the "|" operator. Piping n commands together is not supported by myshell.

For sample output, please see the hard copy.