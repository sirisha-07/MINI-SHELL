#include "header.h"

// Function to execute built-in shell commands
void execute_internal_commands(char *input_string){

	// Exit the shell
	if(strcmp(input_string,"exit") == 0){
		exit(0);
	}

	// Print current working directory
	else if(strcmp(input_string,"pwd") == 0){
		char buff[100];
		
		getcwd(buff,sizeof(buff));	// Get current directory path
		printf("%s\n",buff);		

	}

	// Change directory (cd <path>)
	else if(strncmp(input_string,"cd ",3) == 0){

		int ret = chdir(input_string+3);	// Change to specified path
			
		if(ret == -1){
			perror("cd");	// Print error if directory change fails
			return;
		}else{
			char buff1[100];
			getcwd(buff1,sizeof(buff1));	// Print new working directory
			printf("%s\n",buff1);
		}

	}

	// Print current process ID
	else if(strcmp(input_string,"echo $$") == 0){
		printf("Current Process Pid -> %d\n",getpid());
		
	}

	// Print exit status of last process
	else if(strcmp(input_string,"echo $?") == 0){

		if(WIFEXITED(st))
			printf("Process is terminated normally with %d\n",WEXITSTATUS(st));
		else 
			printf("Process is terminated abnormally with %d\n",WEXITSTATUS(st));

	}

	// Print value of environment variable (echo $VAR)
	else if(strncmp(input_string,"echo $",6) == 0){

		//char buff[15] = "SHELL";
		char *ptr = getenv(input_string + 6);	// Get environment variable
		if(ptr != NULL)
			printf("%s\n",ptr);
		else
			printf("Error: command not found\n");

	}

	// Print list of background jobs
	else if(strcmp(input_string,"jobs") == 0){
			print_command_list(head);
	}

	 // Bring job to foreground
	else if(strcmp(input_string,"fg") == 0){
			foreground_command(&head);
	}

	// Resume job in background
	else if(strcmp(input_string,"bg") == 0){
			background_command(&head);
	}

	// Invalid built-in command
	else{
		printf("ERROR: command not found\n");
	}
}
