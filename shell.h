#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

// Macros
#define TRUE        1
#define FALSE       0

#define BUILTIN		1
#define EXTERNAL	2
#define NO_COMMAND  3

#define EXIT        4
#define PWD         5
#define CD          6
#define ECHO        7

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// structure for maintaining stopped jobs
typedef struct jobs {
    int pid;
    char command[1024];
    struct jobs* next;
}stoppedJobs;

/* Function Prototypes */
void scan_input(char *prompt, char *input_string);
char *get_command(char *input_string);

int check_command_type(char *command);
void echo(char *input_string);
void execute_internal_command(char *input_string);
void execute_external_command(char *input_string);
void signal_handler(int sig_num);
void extract_external_commands(char **external_commands);

int ps1Validate(char* input_string);
void convert22D(char* input_string, char** terminalCom);
int ispipe(char** terminalCom);
int npipe_execute(char** terminalCom);

int check_internal_command(char* input_string);

void insert_job(stoppedJobs** head, int pid, char* input_string);
void delete_job(stoppedJobs** head);
int last_job_pid(stoppedJobs* head, char* input_string);
void jobs(stoppedJobs* head);
#endif 

