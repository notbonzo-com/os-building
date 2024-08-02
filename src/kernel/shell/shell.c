#include "shell.h"
#include <arch/driver/fs/fat.h>
#include <arch/driver/key.h>
#include <arch/i686/vga_text.h>
#include <memory.h>
#include <debug.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

char pwd[256] = "/";

void ls(char* path) {
    FAT_File* fd = FAT_Open(path);
    if (fd == NULL) {
        printf("Failed to open directory\n");
        return;
    } else if (fd->IsDirectory == false) {
        printf("Not a directory\n");
        return;
    }
    const char* files = FAT_ListDir(fd);
    printf("%s\n", files);
    FAT_Close(fd);
}

void cd(char* path) {
    FAT_File* fd = FAT_Open(path);
    if (fd == NULL) {
        printf("");
        return;
    } else if (fd->IsDirectory == false) {
        printf("Not a directory\n");
        return;
    }
    for (int i = 0; i < sizeof(path); i++) {
        path[i] = toupper(path[i]);
    }
    strcpy(pwd, path);
    FAT_Close(fd);
}

bool shell_loop() {
    printf("%s> ", pwd);

    char input[256] = {0}; // Initialize input buffer with zeros
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

    if (strncmp(input, "clear", 5) == 0) {
        VGA_clrscr();
        memset(input, 0, sizeof(input));
        return true;
    }

    if (strncmp(input, "pwd", 3) == 0) {
        printf("%s\n", pwd);
        memset(input, 0, sizeof(input));
        return true;
    }

    if (strncmp(input, "ls", 2) == 0) {
        if (strncmp(input, "ls ", 3) == 0) {
            ls(input + 3);
            memset(input, 0, sizeof(input));
            return true;
        } else {
            ls(pwd);
            memset(input, 0, sizeof(input));
            return true;
        }
    }

    if (strncmp(input, "cd", 2) == 0) {
        if (strncmp(input, "cd ", 3) == 0) {
            cd(input + 3);
            memset(input, 0, sizeof(input));
            return true;
        } else {
            printf("Expected path\n");
            memset(input, 0, sizeof(input)); // Reset input buffer
            return true;
        }
    }

    printf("Unknown command\n");
    memset(input, 0, sizeof(input)); // Reset input buffer
    return true;
}

void init_shell() {
    while (shell_loop())
        ; // Do nothing

    printf("Shell terminated\n");
    return;
}
