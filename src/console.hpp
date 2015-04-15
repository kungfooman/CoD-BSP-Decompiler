#ifndef CONSOLE_H
#define CONSOLE_H

#include <windows.h>

struct Console
{
    HANDLE handle;
    COORD position;
};

void initialise_console(const char *title);

void retrieve_cursor_position();
void restore_cursor_position();

#endif // CONSOLE_H
