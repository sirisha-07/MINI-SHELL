#include "header.h"

// Signal handler function to manage SIGINT (Ctrl+C) and SIGTSTP (Ctrl+Z)
void signal_handler(int sig_num){

	// Handle Ctrl+C (SIGINT)
	if(sig_num == SIGINT){

		// If no child process is running (pid == 0)
		if(pid == 0){

			// Print a fresh prompt line
			printf(GREEN "\n%s: " RESET,prompt);
			fflush(stdout);	 // Ensure prompt is displayed immediately

		}
	}

	// Handle Ctrl+Z (SIGTSTP)
	if(sig_num == SIGTSTP){
		
		// If no child process is running
		if(pid == 0){
	
			// Print a fresh prompt line
			printf(GREEN "\n%s: " RESET,prompt);
			fflush(stdout);
		}else{
			// If a child process is running, add it to job list
			insert_at_last(&head);
		}
	}

	// Handle SIGCHLD (child process termination)
	if(sig_num == SIGCHLD){
		// Reap terminated child processes without blocking
		waitpid(-1,&st,WNOHANG);
	}
}
