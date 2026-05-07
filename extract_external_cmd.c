#include "header.h"

// Function to extract external commands from a file and store them in an array
void extract_external_commands(char **external_commands){

	// Open the file containing external commands in read-only mode
	int fd = open("ext_cmd.txt",O_RDONLY);

	char ch;	// Character buffer for reading file
	char str[25];	// Temporary string buffer (max 24 chars + '\0')
	int i = 0;	// Index for building string
	int row = 0;	// Index for external_commands array

	// Read file character by character
	while(read(fd,&ch,1) > 0){

	  	// If newline is encountered, one command is complete
		if(ch == '\n'){

			str[i] = '\0';	// Null-terminate the string
		
			// Allocate memory for the command and copy it	
			external_commands[row] = malloc(strlen(str) + 1);
			strcpy(external_commands[row],str);

			// Reset index for next command
			i = 0;
			row++;

		}else{
			str[i++] = ch;	// Append character to current string
		}

	}

	// Mark end of command list with NULL (important for iteration later)
	external_commands[row] = NULL;
	
	// Close the file after reading
	close(fd);
	
	/* for(int i = 0;i < row;i++)
		printf("%s\n",external_commands[i]); */
}
