#include "shell.h"

extern char* external_commands[200];
char prompt[100] = "minishell:~$";
char inputString[1024];

int main() {
    // clear the terminal
    system("clear");
    // extract all the external commands
    extract_external_commands(external_commands);
    // read the command from user
    scan_input(prompt, inputString);
}