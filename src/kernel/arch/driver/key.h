#pragma once

#include <stdint.h>
#include <arch/i686/irq.h>

#define MAX_INPUT_STRING_SIZE 256

void init_keyboard();
char getchar();
char* gets();
char getchar_locking();
