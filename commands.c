#include "shell.h"

char* external_commands[200];

/* Function to extract external commands from the file provided */
void extract_external_commands(char **external_commands) {
    // open the file
    int fd = open("commands.txt", O_RDONLY);

    // validate
    if (fd < 0)
    {
        printf("Error: File %s cannot be opened", "commands.txt");
        exit(1);
    }

    char ch, str[1024];
    int i = 0, com_i = 0;
    // read one by one character till EOF
    while (read(fd, &ch, 1))
    {
        // when new line is encountered
        if (ch == '\n')
        {
            // add null characte rat the end of string
            str[i] = '\0';
            // allocate memory dynamically and link it to 2D array pointer
            external_commands[com_i] = malloc(strlen(str) + 1);
            // copy the extracted string to allocated memory
            strcpy(external_commands[com_i], str);
            com_i++;
            i = 0;
        }
        else
        {
            // if not \n store it
            str[i++] = ch;
        }
    }
    // last command before EOF
    str[i] = '\0';
    external_commands[com_i] = malloc(strlen(str) + 1);
    strcpy(external_commands[com_i], str);
    // add NULL at the end to denote the end
    external_commands[++com_i] = NULL;
}

/* Function to validate PS1 variable modification/prompt modification */
int ps1Validate(char* input_string) {
    int index = 4;

    while(input_string[index] != '\0') {
        // if space is encountered after PS1 return FALSE
        if(input_string[index] == ' ')
            return FALSE;
        index++;
    }
    // else return TRUE
    return TRUE;
}

/* Fucntion to get the command without options/arguments */
char *get_command(char *input_string) {
    int index = 0;
    // allocate memory dynamically
    char* command = malloc(1024);
    // get the first word and store it in allocated memory
    while(input_string[index] != ' ' && input_string[index] != '\0') {
        command[index] = input_string[index];
        index++;
    }
    command[index++] = '\0';
    // resize the memory to remove unnecessary memory
    command = realloc(command, index);
    // return the command
    return command;
}

/* Function to check command type */
int check_command_type(char *command) {
    // builtin command array
    char *builtins[] = {"echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval",
						"set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source",
						"exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help", NULL};
    
    int index = 0;
    // check for internal command
    while(builtins[index] != NULL) {
        if(!strcmp(builtins[index], command))
            return BUILTIN;
        index++;
    }

    // check for external command
    index = 0;
    while(external_commands[index] != NULL) {
        if(!strcmp(external_commands[index], command))
            return EXTERNAL;
        index++;
    }
    //else return no command
    return NO_COMMAND;
}

/* Function to convert input string to 2D array of strings */
void convert22D(char* input_string, char **terminalCom) {
    char str[1024];
    int i = 0, com_i = 0, j = 0;

    // get word by word and convert them to individual strings then build a 2D array
    while(input_string[i] != '\0') {
        if(input_string[i] == ' ') {
            str[j++] = '\0';
            terminalCom[com_i] = malloc(j);
            strcpy(terminalCom[com_i], str);
            com_i++;
            j = 0;
        }
        else {
            str[j++] = input_string[i];
        }
        i++;
    }
    str[j++] = '\0';
    terminalCom[com_i] = malloc(j);
    strcpy(terminalCom[com_i], str);
    terminalCom[++com_i] = NULL;
}

/* Function to check pipe is passed or not */
int ispipe(char** terminalCom) {
    int index = 0;
    // traverse the 2D array of input string and compare each word with "|"
    while(terminalCom[index] != NULL) {
        if(terminalCom[index][0] == '|' && terminalCom[index][1] == '\0') {
            return TRUE;
        }
        index++;
    }
    return FALSE;
}

/* checking for supported internal command */
int check_internal_command(char* input_string) {
    // compare and return appropriate MACRO
    if(!strcmp(input_string, "exit"))
        return EXIT;
    
    if(!strcmp(input_string, "pwd"))
        return PWD;
    
    if(!strncmp(input_string, "cd", 2))
        return CD;

    if(!strncmp(input_string, "echo", 4))
        return ECHO;
        
    return NO_COMMAND;
}

/* Function to add stopped process to jobs list */
void insert_job(stoppedJobs** head, int pid, char* input_string) {
    // create the new job
    stoppedJobs* new_job = malloc(sizeof(stoppedJobs));
    // validate
    if(new_job == NULL) {
        printf("Error: Job not stored\n");
        return;
    }

    // update info
    new_job->pid = pid;
    strcpy(new_job->command, input_string);
    new_job->next = NULL;

    // first job creation
    if(*head == NULL) {
        *head = new_job;
        return;
    }

    // else traverse and add last
    stoppedJobs* temp = *head;

    while(temp->next != NULL) {
        temp = temp->next;
    }

    temp->next = new_job;
    return;
}

/* Function to delete the stopped process */
void delete_job(stoppedJobs** head) {
    // check for only one job
    if((*head)->next == NULL) {
        free(*head);
        *head = NULL;
        return;
    }

    // else delete the latest stopped job/process
    stoppedJobs* temp = *head;

    while(temp->next->next != NULL) {
        temp = temp->next;
    }
    free(temp->next);
    temp->next = NULL;
}

/* Function to get latest stopped process pid */
int last_job_pid(stoppedJobs* head, char* input_string) {
    // trverse through the list to last node
    while(head->next != NULL) {
        head = head->next;
    }
    // if input string address is passed copy the stopped job command
    if(input_string != NULL)
        strcpy(input_string, head->command);
    // return the pid
    return head->pid;
}