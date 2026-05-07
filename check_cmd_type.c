#include "header.h"

// Function to determine whether a command is BUILTIN, EXTERNAL, or invalid
int check_command_type(char *command){

	// Check against list of built-in commands
	for(int i = 0;builtins[i] != NULL;i++){
	
		if(strcmp(builtins[i],command) == 0)
			return BUILTIN;	// Found return BUILTIN 

	}

	// Check against list of external commands
	for(int i = 0;external_commands[i] != NULL;i++){

		if(strcmp(external_commands[i],command) == 0)
			return EXTERNAL;	// Found return EXTERNAL

	}	

	// If not found in either list, return NO_COMMAND
	return NO_COMMAND;
	
}

