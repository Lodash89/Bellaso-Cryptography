#ifndef _TERMINAL_H_
    #define _TERMINAL_H_

    #if defined _WIN32 || defined _WIN64
        #include <conio.h>
        #ifdef _MSC_VER
            #pragma warning(disable: 4996) // for getch()
        #endif
    #else
        #include <stdio.h>
        #include <termios.h>

        int getche(){
            char c;
            struct termios old, current;

            tcgetattr(0, &old);

            current = old;
            current.c_lflag &= ~ICANON; // disable buffered input
            
            tcsetattr(0, TCSANOW, &current); // apply new settings
            
            c = getchar();

            tcsetattr(0, TCSANOW, &old); // restore old settings

            return (c == '\n') ? '\r' : c;
        }

    #endif

#endif