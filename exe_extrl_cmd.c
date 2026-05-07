#include "header.h"

// Function to execute external commands, with or without pipes
void execute_external_commands(char *input_string){

    char *argv[10];        // Array to hold command arguments
    int i = 0, j = 0;      // Index variables for parsing input_string
    int row = 0;           // Tracks number of arguments stored
    char str[100];         // Temporary buffer to build each argument
    int pipe_cmd = 0;      // Counter for number of pipe symbols '|'

    // Parse the input string into arguments
    while(input_string[i])     // Loop until end of input string
    {
        if(input_string[i] == '|')     // Count pipe symbols
            pipe_cmd++;

        if(input_string[i] == ' ')     // Space indicates end of one argument
        {
            str[j] = '\0';  // Terminate current argument string

            argv[row] = malloc(strlen(str)+1); // Allocate memory for argument
            strcpy(argv[row],str);             // Copy argument into argv

            row++;   // Move to next argument slot
            i++;     // Skip space
            j = 0;   // Reset buffer index
        }
        else                          // Collect characters into str
        {
            str[j++] = input_string[i++];
        }
    }

    // Store the last argument after loop ends
    str[j] = '\0';
    argv[row] = malloc(strlen(str)+1);
    strcpy(argv[row],str);
    row++;

    argv[row] = NULL;   // Null-terminate argv list for execvp

    // Case 1: No pipe present → execute single command
    if(pipe_cmd == 0)
    {
        execvp(argv[0],argv);   // Replace process image with command
    }
    else   // Case 2: Pipe(s) present → handle multiple commands
    {
        int cmd[10];    // Array to store starting indices of commands
        cmd[0] = 0;     // First command starts at argv[0]
        int ind = 1;    // Number of commands

        // Identify pipe positions and split argv
        for(int i = 0;i < row;i++)
        {
            if(strcmp(argv[i],"|") == 0)   // Found a pipe
            {
                cmd[ind++] = i + 1;   // Next command starts after pipe
                argv[i] = NULL;       // Replace pipe with NULL to split argv
            }
        }

        int fd[2];   // File descriptors for pipe

        // Execute each command in pipeline
        for(int i = 0;i < ind ;i++)
        {
            if(i != ind - 1)          // Create pipe if not last command
                pipe(fd);

            int ret = fork();         // Fork child process

            if(ret > 0)   // Parent process
            {
                if(i != ind - 1)
                    close(fd[1]);     // Close write end in parent

                waitpid(ret,NULL,0);  // Wait for child to finish

                if(i != ind - 1)      // Redirect input for next command
                {
                    dup2(fd[0],0);    // Replace stdin with pipe read end
                    close(fd[0]);
                    close(fd[1]);
                }
            }
            else   // Child process
            {
                if(i != ind - 1)      // Redirect output if not last command
                {
                    close(fd[0]);     // Close read end in child
                    dup2(fd[1],1);    // Replace stdout with pipe write end
                }

                execvp(argv[cmd[i]],argv+cmd[i]); // Execute command
            }
        }
        exit(0);   // Exit after pipeline execution
    }
}
