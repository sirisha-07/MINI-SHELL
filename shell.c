#include "shell.h"

int status, pid, bg_pid;
extern char prompt[100], inputString[1024];
stoppedJobs *head = NULL;

/* Signal Handler */
void signal_handler(int sig_num) {
    int ret;
    // if SIGINT just print prompt
    if(sig_num == SIGINT) {
        if(pid == 0) {
            printf(ANSI_COLOR_GREEN "\n%s " ANSI_COLOR_RESET, prompt);
            fflush(stdout);
        }
    }
    // if SIGSTP
    else if(sig_num == SIGTSTP) {
        // if parent print prompt
        if(pid == 0) {
            printf(ANSI_COLOR_GREEN "\n%s " ANSI_COLOR_RESET, prompt);
            fflush(stdout);
        }
        // if child stopped insert it to stopped job list
        else{
            insert_job(&head, pid, inputString);
            pid = 0;
        }
    }
    // if SIGCHLD
    else if(sig_num == SIGCHLD) {
        // clear the resources of resumed child
        ret = waitpid(bg_pid, &status, WNOHANG);
        if(ret != 0 && ret != -1) {
            delete_job(&head);
        }
    }
}

/* Function for reading commands */
void scan_input(char *prompt, char *input_string)
{
    char *command = NULL, ch;
    int ret, i;
    // regester signals for new action
    signal(SIGINT, signal_handler);
    signal(SIGTSTP, signal_handler);
    // do the action till exit
    while (1)
    {
        i = 0;
        // print prompt with color
        printf(ANSI_COLOR_GREEN "%s "ANSI_COLOR_RESET, prompt);
        __fpurge(stdin);
        // read the command
        while ((ch = getc(stdin)) != '\n')
        {
            input_string[i++] = ch;
        }
        // if enter is pressed print prompt again
        if (!i)
            continue;
        input_string[i] = '\0';

        // remove trailing spaces
        int j = strlen(input_string) - 1;
        while(input_string[j] == ' ')
            j--;
        input_string[j + 1] = '\0';
        
        // check for PS1 variable 
        if (!strncmp(input_string, "PS1=", 4))
        {
            // if any space is in between print message
            if (!ps1Validate(input_string))
            {
                printf("Command 'PS1' not found\n");
                continue;
            }
            // else change the prompt
            strcpy(prompt, input_string + 4);
            continue;
        }

        // get the command without options/arguments
        command = get_command(input_string);

        // jobs command
        if(!strcmp(command, "jobs")) {
            // print stopped jobs
            jobs(head);
            continue;
        }

        // fg command
        if(!strcmp(command, "fg")) {
            // if no stopped jobs
            if(head == NULL) {
                printf("fg: current: no such job\n");
            }
            else {
                // get the latest stopped job pid and command
                pid = last_job_pid(head, inputString);
                // delete the job from list
                delete_job(&head);
                // print the command
                printf("%s\n", inputString);
                // resume the stopped process
                kill(pid, SIGCONT);
                // wait for it to terminate
                waitpid(pid, &status, WUNTRACED);
                pid = 0;
            }
            continue;
        }

        // bg command
        if(!strcmp(command, "bg")) {
            // if no jobs
            if(head == NULL) {
                printf("bg: current: no such job\n");
            }
            else {
                // get the latest stopped job pid and command
                bg_pid = last_job_pid(head, inputString);
                // print the command
                printf("%s\n", inputString);
                // resume the stopped process
                kill(bg_pid, SIGCONT);
                // clear the resources by registering SIGCHLD signal for new action
                signal(SIGCHLD, signal_handler);
            }
            continue;
        }

        // check the command type
        ret = check_command_type(command);
        // if external command
        if (ret == EXTERNAL)
        {
            // create a child process
            pid = fork();
            // validate
            if (pid < 0)
            {
                perror("fork");
            }
            // child process
            else if (pid == 0)
            {
                // register SIGINT and SIGTSTP for default action
                signal(SIGINT, SIG_DFL);
                signal(SIGTSTP, SIG_DFL);
                // execute the external command
                execute_external_command(input_string);
                // if failed exit
                exit(EXIT_SUCCESS);
            }
            // parent process
            else
            {
                // wait for the child process to terminate
                waitpid(pid, &status, WUNTRACED);
                if(status != 0)
                    printf("\n");
                pid = 0;
            }
        }
        // if internal command
        else if (ret == BUILTIN)
        {
            // execute separately
            execute_internal_command(input_string);
        }
        // else print command not found
        else
        {
            printf("%s: Command not found\n", command);
        }
        // free the command
        free(command);
    }
}

/* Function to execute external command */
void execute_external_command(char *input_string)
{
    char *terminalCom[50];
    // convert the input string to 2D array of strings
    convert22D(input_string, terminalCom);
    // check if pipe is there
    if (ispipe(terminalCom))
    {
        // if true execute command through pipe IPC mechanism
        npipe_execute(terminalCom);
    }
    else
    {
        // else execute normally through exec sys call
        execvp(terminalCom[0], terminalCom);
    }
}

/* Function to execute internal command */
void execute_internal_command(char *input_string)
{
    // check for supported commands in this project
    int ret = check_internal_command(input_string);
    char *pwd;
    switch (ret)
    {
    case EXIT:
        // if exit command terminate the shell
        exit(EXIT_SUCCESS);
    case PWD:
        // if pwd is given print the current working directory
        pwd = getcwd(NULL, 0);
        if (pwd == NULL)
        {
            perror("getcwd");
        }
        else
        {
            printf("%s\n", pwd);
            free(pwd);
        }
        break;
    case CD:
        // if cd is given change the directory to specified path
        if (chdir(input_string + 3))
        {
            perror("chdir");
        }
        else
        {
            pwd = getcwd(NULL, 0);
            printf("%s\n", pwd);
            free(pwd);
        }
        break;
    case ECHO:
        // if echo is given then execute according to argument passed
        echo(input_string);
        break;
    default:
        // any other command print not found
        printf("Command '%s' not found\n", input_string);
    }
}

/* Function to execute piped commands */
int npipe_execute(char **terminalCom)
{
    // taking pipe count
    int pipes = 0, i = 0;
    while (terminalCom[i] != NULL)
    {
        if (terminalCom[i][0] == '|' && terminalCom[i][1] == '\0')
            pipes++;
        i++;
    }

    int com_ind[pipes + 1];
    com_ind[0] = 0;
    int ci = 1;
    i = 0;
    while (terminalCom[i] != NULL)
    {
        if (!strcmp(terminalCom[i], "|"))
        {
            // separating commands
            terminalCom[i] = NULL;
            // getting indexes  of command
            com_ind[ci++] = i + 1;
            // other command passed or not
            if (terminalCom[i + 1] == NULL)
            {
                printf("Error: Insufficient arguments passed Usage: ./pipe <command1 > '|' <command2>\n");
                exit(EXIT_FAILURE);
            }
            // first command passed or not
            if (i == 1)
            {
                printf("Error: Insufficient arguments passed Usage: ./pipe <command1 > '|' <command2>\n");
                exit(EXIT_FAILURE);
            }
        }
        i++;
    }

    int fd[2], pid;
    int backup_stdin = dup(0);
    int backup_stdout = dup(1);
    // run a loop till all the commands are covered
    for (int j = 0; j < pipes + 1; j++)
    {
        // if it is not a last command create a pipe
        if (j != pipes)
        {
            if (pipe(fd) == -1)
            {
                perror("pipe");
                exit(EXIT_FAILURE);
            }
        }

        // create a child process
        pid = fork();
        // in child
        if (pid == 0)
        {
            // close the read end
            close(fd[0]);
            // make write end as stdout
            dup2(fd[1], 1);
            // execute the command
            execvp(terminalCom[com_ind[j]], terminalCom + com_ind[j]);
        }
        // in parent if it is not last command
        else if (pid > 0 && j != pipes)
        {
            // close the write end
            close(fd[1]);
            // make read end as stdin
            dup2(fd[0], 0);
            // close the read end
            close(fd[0]);
        }
    }

    dup2(backup_stdin, 0);
    dup2(backup_stdout, 1);

    // in parent wait till all the childs are terminated
    if (pid > 0)
    {
        while (ci)
        {
            wait(NULL);
            ci--;
        }
    }
}

/* Function to execute echo command */
void echo(char *input_string) {
    // if '$' not given then simply print the argument and return
    if(input_string[5] != '$') {
        printf("%s\n", input_string + 5);
        return;
    }

    // "$?" - print the last executed command's exit status
    if(input_string[6] == '?') {
        printf("%d", status);
        printf("%s\n", input_string + 7);
        return;
    }
    
    // "$$" - print the pid of shell
    if(input_string[6] == '$') {
        printf("%d", getpid());
        printf("%s\n", input_string + 7);
        return;
    }

    // "$SHELL" - print the executable path of the shell
    if(!strcmp(input_string + 6, "SHELL")) {
        printf("%s\n", getenv("SHELL"));
        return;
    }
    // nothing means print nothing
    printf(" \n");
}

/* Function to execute jobs command */
void jobs(stoppedJobs* head) {
    // if no stopped jobs simply return
    if(head == NULL)
        return;
    int i = 1;
    // else traverse and print all the stopped jobs one by one
    while(head != NULL) {
        printf("[%d]  %-20s %s\n", i, "Stopped", head->command);
        head = head->next;
        i++;
    }
}