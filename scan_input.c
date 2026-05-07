#include "header.h"

// Array to store external commands loaded from file
char *external_commands[153];

// Global process ID and status variables
int pid = 0;
int st = 0;

// Function to continuously scan user input and execute commands
void scan_input(char *prompt,char *input_string){

	// Register signal handlers for Ctrl+C (SIGINT) and Ctrl+Z (SIGTSTP) and (SIGCHLD)
	signal(SIGINT,signal_handler);
	signal(SIGTSTP,signal_handler);
	signal(SIGCHLD,signal_handler);

	// Load external commands from ext_cmd.txt into external_commands[]
	extract_external_commands(external_commands);

	// Infinite loop to keep shell running
        while(1){

		// Display prompt in green color
                printf(GREEN "%s: " RESET,prompt);

		// Read user input until newline	
                if(scanf("%[^\n]",input_string) == 0){
			getchar();	 // Consume newline
			continue;	 // Retry prompt
		}	

                getchar();	// Consume newline after input

		// Handle prompt change (PS1=...)
                if(strncmp(input_string,"PS1=",4) == 0){
                        int valid = 1;

			// Check if space immediately after '='
                        if(input_string[4] == ' ')
                                valid = 0;

			// Validate that no spaces exist in new prompt string
                        for(int i = 4;input_string[i] != '\0' && valid ;i++){

                                if(input_string[i] == ' '){
                                        if(input_string[i+1] != '\0')
                                        valid = 0;
                                        break;
                                }
                        }

			// If valid, update prompt string
                        if(valid)
                                strcpy(prompt,input_string+4);
                        else{
                                printf("ERROR: %s command not found\n",input_string+4);
                        }

                }else{
			// Extract first word (command) from input
			char *command = get_command(input_string);

			// Determine command type (BUILTIN / EXTERNAL / INVALID)
			int type = check_command_type(command);	
	
			if(type == BUILTIN){
				
				// Execute built-in command
				//printf("INFO: Command is builtins\n");
				execute_internal_commands(input_string);
			}
			else if(type == EXTERNAL){

				// Execute external command
				//printf("INFO: Command is external\n");
				
				pid = fork();	// Create child process
				if(pid > 0){
					// Parent waits for child process
					waitpid(pid,&st,WUNTRACED);
					pid = 0;
				}else{
					// Child resets signals to default
					signal(SIGINT,SIG_DFL);
					signal(SIGTSTP,SIG_DFL);
					execute_external_commands(input_string);
				}
			}
			else{
				// Invalid command
				printf("ERROR: command not found\n"); 
 			}
       		}
	}
}

