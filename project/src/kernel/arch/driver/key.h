#pragma once

#include <stdint.h>
#include <arch/i686/irq.h>

void init_keyboard();
char getchar();
char* gets();
char getchar_locking();
