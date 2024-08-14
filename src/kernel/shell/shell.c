#include "shell.h"
#include <arch/driver/fs/fat.h>
#include <arch/driver/key.h>
#include <arch/i686/vga_text.h>
#include <arch/driver/pmm.h>
#include <memory.h>
#include <debug.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

#define MAX_INPUT_SIZE 256
#define MAX_PATH_SIZE 256
#define SHELL_VERSION "1.0.0"  // Define the shell version

char pwd[MAX_PATH_SIZE] = "/";

typedef void (*command_func)(char* args);

typedef struct {
    const char* name;
    command_func func;
    const char* help;
} Command;

void cmd_ls(char* args);
void cmd_cd(char* args);
void cmd_pwd(char* args);
void cmd_clear(char* args);
void cmd_cat(char* args);
void cmd_help(char* args);
void cmd_version(char* args);
void cmd_exit(char* args);

Command commands[] = {
    {"ls",      cmd_ls,      "List directory contents"},
    {"cd",      cmd_cd,      "Change the shell working directory"},
    {"pwd",     cmd_pwd,     "Print name of current/working directory"},
    {"clear",   cmd_clear,   "Clear the terminal screen"},
    {"cat",     cmd_cat,     "Concatenate and display file contents"},
    {"help",    cmd_help,    "Display this help message"},
    {"version", cmd_version, "Display shell version"},
    {"exit",    cmd_exit,    "Exit the shell"},
    {NULL,      NULL,        NULL}
};

void cmd_ls(char* args) {
    const char* path = (args && *args) ? args : pwd;
    FAT_File* fd = FAT_Open(path);
    if (!fd) {
        printf("ls: cannot access '%s': No such file or directory\n", path);
        return;
    }
    if (!fd->IsDirectory) {
        printf("ls: '%s' is not a directory\n", path);
        FAT_Close(fd);
        return;
    }
    const char* files = FAT_ListDir(fd);
    printf("%s\n", files);
    FAT_Close(fd);
}

void cmd_cd(char* args) {
    if (!args || !*args) {
        printf("cd: expected argument\n");
        return;
    }
    FAT_File* fd = FAT_Open(args);
    if (!fd) {
        printf("cd: no such file or directory: %s\n", args);
        return;
    }
    if (!fd->IsDirectory) {
        printf("cd: not a directory: %s\n", args);
        FAT_Close(fd);
        return;
    }
    strncpy(pwd, args, MAX_PATH_SIZE);
    FAT_Close(fd);
}

void cmd_pwd(char* args) {
    printf("%s\n", pwd);
}

void cmd_clear(char* args) {
    VGA_clrscr();
}

void cmd_cat(char* args) {
    if (!args || !*args) {
        printf("cat: expected filename\n");
        return;
    }
    FAT_File* fd = FAT_Open(args);
    if (!fd) {
        printf("cat: %s: No such file or directory\n", args);
        return;
    }
    if (fd->IsDirectory) {
        printf("cat: %s: Is a directory\n", args);
        FAT_Close(fd);
        return;
    }
    char* buffer = alloc_pages(1); // 4096
    FAT_Read(fd, 4096, buffer);
    printf("%s", buffer);
    free_pages(buffer, 1);
    printf("\n");
    FAT_Close(fd);
}

void cmd_help(char* args) {
    printf("Available commands:\n");
    for (Command* cmd = commands; cmd->name != NULL; cmd++) {
        printf("  %s: %s\n", cmd->name, cmd->help);
    }
}

void cmd_version(char* args) {
    printf("Crazy-Shell version: %s, (C) NotBonzo 2024\n", SHELL_VERSION);
}

void cmd_exit(char* args) {
    // This function is just a placeholder; its existence prevents the null dereference.
    // The actual exit logic is handled elsewhere.
}

void execute_command(char* input) {
    char* args = NULL;
    char* command = strtok_r(input, " ", &args);
    if (!command) return;

    for (Command* cmd = commands; cmd->name != NULL; cmd++) {
        if (strcmp(cmd->name, command) == 0) {
            if (cmd->func) {
                cmd->func(args);
            }
            return;
        }
    }
    printf("shell: command not found: %s\n", command);
}

bool shell_loop() {
    char input[MAX_INPUT_SIZE];

    printf("%s> ", pwd);
    int i = 0;
    for (;;) {
        char c = getchar();
        if (c == -1)
            continue;

        if (c == '\x7F') { // My code for backspace
            if (i > 0) {
                i--;
                input[i] = '\0';
                printf("\x7F \x7F");
            }
            continue;
        }

        if (c == '\n') {
            input[i] = '\0';
            printf("%c", c);
            break;
        }

        if (i < sizeof(input) - 1) { // Prevent buffer overflow
            input[i++] = c;
            printf("%c", c);
        }
    }

    if (strncmp(input, "exit", 4) == 0)
        return false;

    debugf("\"%s\"\n", input);
    execute_command(input);
    return true;
}

void init_shell() {
    printf("Welcome to the Crazy-Shell. Type 'help' for a list of commands.\n");
    debugf("Addr: %p\n", commands[1]);
    while(shell_loop());
}
