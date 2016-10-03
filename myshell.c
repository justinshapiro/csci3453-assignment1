#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define MAX_CMD_SZ 1024
#define MAX_ARG_SZ 64

const char* SHELL_NAME = "myshell> ";

void redefine_args(char** args, char** new_args, int split_point) {
	int i;
	for (i = 0; i < split_point; ++i) 
		new_args[i] = args[i];
	
	new_args[i] = '\0';
}

int find_in_args(char** args, char* target) {
	int found = -1;
	int i = 0;
	while (args[i] != NULL) {
		if (strcmp(args[i], target) != 0)
			++i;
		else {
			found = i;
			break;
		}
	}
	
	return found;
}

void execute(char** args) {
	// Special Case: the exit command (must use the parent process)
	if (find_in_args(args, "exit") != -1) {
		while (1)
			exit(0);
	}
	
	// Special Case: the cd command (must use the parent process)
	else if (find_in_args(args, "cd") != -1) {
			int status = chdir(args[1]);
			if (status == -1) 
				printf("%s: No such file or directory.\n", args[1]);
	}
		
	// Special Case: redirection to output
	else if (find_in_args(args, ">") != -1) {
		int redirection_idx = find_in_args(args, ">");
		char* file = args[redirection_idx + 1];
		char* new_args[redirection_idx];
		redefine_args(args, new_args, redirection_idx);
		
		pid_t pid = fork();
		if (pid == 0) {
			int outfile = open(file, O_CREAT | O_WRONLY, S_IRWXU);
			dup2(outfile, STDOUT_FILENO);
			close(outfile);
			
			execvp(args[0], new_args);
			abort();
		}
		else
			wait(0);
	}
	
	// Special Case: redirection from input
	else if (find_in_args(args, "<") != -1) {
		int redirection_idx = find_in_args(args, "<");
		char* file = args[redirection_idx + 1];
		char* new_args[redirection_idx];
		redefine_args(args, new_args, redirection_idx);
		
		pid_t pid = fork();
		if (pid == 0) {
			int infile = open(file, O_RDONLY, S_IRWXU);
			if (infile == -1) {
				printf("%s: File Not Found. \n", file);
			}
			else {
				dup2(infile, STDIN_FILENO);
				close(infile);
				
				execvp(args[0], new_args);
				abort();
			}
		}
		else
			wait(0);
	}
	
	// Special Case: piped commands (dual-only, non-recursive)
	else if (find_in_args(args, "|") != -1) {
		int pipe_idx = find_in_args(args, "|");
		int in = STDIN_FILENO;
		int out = STDOUT_FILENO;
		int table[2];
		pipe(table);
		
		out = table[1];
		
		args[pipe_idx] = '\0';
		
		pid_t pid_0 = fork();
		if (pid_0 == 0) {
			if (in != STDIN_FILENO) 
				dup2(in, STDIN_FILENO);
			if (out != STDOUT_FILENO)
				dup2(out, STDOUT_FILENO);
			execvp(args[0], args);
			abort();
		}
		else 
			wait(0);
		
		close(table[1]);
		
		in = table[0];
		out = STDOUT_FILENO;
		
		pid_t pid_1 = fork();
		if (pid_1 == 0) {
			if (in != STDIN_FILENO) 
				dup2(in, STDIN_FILENO);
			if (out != STDOUT_FILENO)
				dup2(out, STDOUT_FILENO);
			execvp((args + pipe_idx + 1)[0], args + pipe_idx + 1);
			abort();
		}
		else
			wait(0);
	}
	
	// Regular command with argumets
	else {
		pid_t pid = fork();
		if (pid == 0) {
			int status = execvp(args[0], args);
			if (status < 0)
				printf("%s: Command not found.\n", args[0]);
		}
		else
			wait(0);
	}
}

void parse(char* cmd, char** args) {
	while (*cmd != '\0') { 
          while (*cmd == ' ' || *cmd == '\t' || *cmd == '\n')
               *cmd++ = '\0'; 
		   
		  if (*cmd)
			*args++ = cmd;
		
          while (*cmd != '\0' && *cmd != ' ' && *cmd != '\t' && *cmd != '\n') 
               cmd++;       
     }
	 
     *args = '\0';  
}

int main() {
	while (1) {
		char cmd[MAX_CMD_SZ];
		char* args[MAX_ARG_SZ];

		// get user-inputted command
		printf(SHELL_NAME);
		fgets(cmd, MAX_CMD_SZ, stdin);
		
		// parse command arguments and get the size of the base command
		parse(cmd, args);
	
		// execute given command with arguments
		execute(args);
	}
}