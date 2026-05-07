#include "header.h"

// Global buffer to store extracted command
char cmd[30];

// Function to extract the first command (before any space) from input string
char *get_command(char *input_string){

	int j = 0;	// Index for cmd buffer

	// Traverse the input string until end or space
	for(int i = 0;input_string[i]!= '\0';i++){
		
		// Stop copying when a space is encountered
		if(input_string[i] == ' ')
			break;

		// Copy character into cmd buffer
		cmd[j++] = input_string[i];

	}

	// Null-terminate the command string
	cmd[j] = '\0';

	//printf("cmd_command -> %s\n",cmd);
	
	// Return pointer to the command buffer
	return cmd;

}
