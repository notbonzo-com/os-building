#include <stdio.h>
#include <arch/i686/io.h>

#include <stdarg.h>
#include <stdbool.h>

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define DEFAULT_COLOR 0x7

uint8_t* g_ScreenBuffer = (uint8_t*)0xB8000;
int g_ScreenX = 0, g_ScreenY = 0;

void VGA_putchr(int x, int y, char c)
{
    g_ScreenBuffer[2 * (y * SCREEN_WIDTH + x)] = c;
}

void VGA_putcolor(int x, int y, uint8_t color)
{
    g_ScreenBuffer[2 * (y * SCREEN_WIDTH + x) + 1] = color;
}

char VGA_getchr(int x, int y)
{
    return g_ScreenBuffer[2 * (y * SCREEN_WIDTH + x)];
}

uint8_t VGA_getcolor(int x, int y)
{
    return g_ScreenBuffer[2 * (y * SCREEN_WIDTH + x) + 1];
}

void VGA_setcursor(int x, int y)
{
    int pos = y * SCREEN_WIDTH + x;

    i686_outb(0x3D4, 0x0F);
    i686_outb(0x3D5, (uint8_t)(pos & 0xFF));
    i686_outb(0x3D4, 0x0E);
    i686_outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void VGA_clrscr()
{
    for (int y = 0; y < SCREEN_HEIGHT; y++)
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            VGA_putchr(x, y, '\0');
            VGA_putcolor(x, y, DEFAULT_COLOR);
        }

    g_ScreenX = 0;
    g_ScreenY = 0;
    VGA_setcursor(g_ScreenX, g_ScreenY);
}

void VGA_scrollback(int lines)
{
    for (int y = lines; y < SCREEN_HEIGHT; y++)
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            VGA_putchr(x, y - lines, VGA_getchr(x, y));
            VGA_putcolor(x, y - lines, VGA_getcolor(x, y));
        }

    for (int y = SCREEN_HEIGHT - lines; y < SCREEN_HEIGHT; y++)
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            VGA_putchr(x, y, '\0');
            VGA_putcolor(x, y, DEFAULT_COLOR);
        }

    g_ScreenY -= lines;
}

void VGA_delchar()
{
    if (g_ScreenX == 0 && g_ScreenY == 0)
        return;

    if (g_ScreenX == 0)
    {
        g_ScreenY--;
        g_ScreenX = SCREEN_WIDTH - 1;
    }
    else
    {
        g_ScreenX--;
    }

    for (int y = g_ScreenY; y < SCREEN_HEIGHT; y++)
    {
        for (int x = (y == g_ScreenY ? g_ScreenX : 0); x < SCREEN_WIDTH; x++)
        {
            int nextX = x + 1;
            int nextY = y;
            if (nextX >= SCREEN_WIDTH)
            {
                nextX = 0;
                nextY++;
            }

            if (nextY < SCREEN_HEIGHT)
            {
                VGA_putchr(x, y, VGA_getchr(nextX, nextY));
                VGA_putcolor(x, y, VGA_getcolor(nextX, nextY));
            }
            else
            {
                VGA_putchr(x, y, '\0');
                VGA_putcolor(x, y, DEFAULT_COLOR);
            }
        }
    }

    VGA_setcursor(g_ScreenX, g_ScreenY);
}

void VGA_putc(char c)
{
    switch (c)
    {
        case '\n':
            g_ScreenX = 0;
            g_ScreenY++;
            break;
    
        case '\t':
            for (int i = 0; i < 4 - (g_ScreenX % 4); i++)
                VGA_putc(' ');
            break;

        case '\r':
            g_ScreenX = 0;
            break;

        case '\x7F':
            VGA_delchar();
            break;

        default:
            VGA_putchr(g_ScreenX, g_ScreenY, c);
            g_ScreenX++;
            break;
    }

    if (g_ScreenX >= SCREEN_WIDTH)
    {
        g_ScreenY++;
        g_ScreenX = 0;
    }
    if (g_ScreenY >= SCREEN_HEIGHT)
        VGA_scrollback(1);

    VGA_setcursor(g_ScreenX, g_ScreenY);
}