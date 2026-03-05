# Mini Shell - Command Line Interpreter
A lightweight UNIX-like shell built in C that supports basic command execution, piping, signal handling, and keyboard shortcuts. This project mimics fundamental features of the BASH shell and demonstrates low-level system programming using Linux system calls.

# Features
- Execute built-in and external commands
- Support for piping (|) between commands
- Graceful handling of Ctrl+C (SIGINT)
- Process creation using fork(), execvp(), and wait()
- Scalable foundation for features like command history and redirection
