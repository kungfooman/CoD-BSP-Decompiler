#include "console.hpp"

#include <stdio.h>

Console console;

void initialise_console(const char *title)
{
    console.handle = GetStdHandle(STD_OUTPUT_HANDLE);

    system("cls");
    SetConsoleTitle(title);

    printf("%s\n", title);
    for (unsigned int i = 0; i < strlen(title); i++)
    {
        printf("-");
    }
    printf("\n\n");
}

void retrieve_cursor_position()
{
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(console.handle, &info);

    console.position = info.dwCursorPosition;
}

void restore_cursor_position()
{
    SetConsoleCursorPosition(console.handle, console.position);
}
