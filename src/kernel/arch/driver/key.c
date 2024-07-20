#include "key.h"
#include <stdint.h>
#include <stdio.h>
#include <arch/i686/irq.h>
#include <arch/i686/io.h>

char buffer[256];
int index = 0;
int count = 0;
int shift_pressed = 0;
int ctrl_pressed = 0;

const char* scancodeToASCII[256] = {
    "", "\x1B", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "\x7F", // ESC, BKSP
    "\t", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "\n",     // TAB, ENTER as \n \t
    "", "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "'", "`",
    "", "\\", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "",
    "", "", " ", "", "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "", "", "", "", ""
};

const char* scancodeToASCII_shift[256] = {
    "", "\x1B", "!", "@", "#", "$", "%", "^", "&", "*", "(", ")", "_", "+", "\x7F", // ESC, BKSP
    "\t", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "{", "}", "\n",     // TAB, ENTER as \n \t
    "", "A", "S", "D", "F", "G", "H", "J", "K", "L", ":", "\"", "~",
    "", "|", "Z", "X", "C", "V", "B", "N", "M", "<", ">", "?", "",
    "", " ", "", "", "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "", "", "", "", ""
};

void keyboardIRQHandler(Registers*)
{
    uint8_t scancode = i686_inb(0x60);

    if (scancode & 0x80) {
        scancode &= 0x7F;
        if (scancode == 0x2A || scancode == 0x36) {
            shift_pressed = 0;
        } else if (scancode == 0x1D) {
            ctrl_pressed = 0;
        }
    } else {
        if (scancode == 0x2A || scancode == 0x36) {
            shift_pressed = 1;
        } else if (scancode == 0x1D) {
            ctrl_pressed = 1;
        } else {
            const char* key = shift_pressed ? scancodeToASCII_shift[scancode] : scancodeToASCII[scancode];
            char c = key[0];

            if (c != '\0' && count < 256) {
                buffer[index++] = c;
                count++;
                index %= 256;
            }
        }
    }
}


void init_keyboard()
{
    i686_IRQ_RegisterHandler(1, keyboardIRQHandler);
}

char getchar()
{
    if (count == 0)
        return -1;

    char c = buffer[(index - count + 256) % 256];
    count--;

    return c;
}

char getchar_locking()
{
    while (count == 0)
        ;

    char c = buffer[(index - count + 256) % 256];
    count--;

    return c;
}

char* gets()
{
    static char str[MAX_INPUT_STRING_SIZE];
    char c;
    int i = 0;

    while (i < MAX_INPUT_STRING_SIZE - 1) {
        c = getchar_locking();
        if (c == '\n') break;
        str[i++] = c;
    }
    
    str[i] = '\0';

    return str;
}