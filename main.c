/*
Description: The main() function is the driver of the MiniShell program. It:

             - Displays a shell prompt and continuously accepts user commands.
	     - Reads and parses the input string into command and arguments.
	     - Checks whether the command is built-in or external.

	     - Executes built-in commands internally such as:
    	     	• cd (change directory)
    	     	• pwd (print working directory)
    	     	• exit (terminate shell)

	     - Executes external commands by:
    		• Creating a child process using fork()
    		• Replacing process image using exec()
    		• Waiting for completion using wait() (for foreground processes)

- Handles multiple commands using pipes for inter-process communication.

- Supports input/output redirection using file descriptors and dup2().

- Allows background execution of processes without blocking the shell.

- Handles signals like:
    • SIGINT (Ctrl + C)
    • SIGTSTP (Ctrl + Z)
  to ensure smooth shell operation.

- Maintains basic job control for managing foreground and background processes.

- Ensures continuous execution of the shell until the user explicitly exits.
*/

#include "header.h"

// Global buffers and linked list head

char input_string[50];		// Stores user input
char prompt[30] = "minishell";	// Default shell prompt
Slist *head = NULL;		// Head pointer for job list

int main(){

	// Clear the terminal screen at startup
	system("clear");

	// Start scanning user input and executing commands
	scan_input(prompt,input_string);

	// Exit program
	return 0;
}
