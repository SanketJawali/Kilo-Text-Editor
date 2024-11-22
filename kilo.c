#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <ctype.h>


struct termios orig_termios;


void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);  // Set the terminal attributes to the original
}


void enableRawMode() {  
    tcgetattr(STDIN_FILENO, &orig_termios);  // Store the current terminal attributes in orig_termios
    atexit(disableRawMode);  // At exit disable the Raw mode
    struct termios raw = orig_termios;  // Store a copy of original terminal attributes in raw struct
    // Turning off some of the input and output flags to enabld the Raw mode in the terminal
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);  // Set the terminal attributes
}


int main() {
    // Enable RAW Mode
    enableRawMode();
    
    char c;
    while (read(STDIN_FILENO, &c, 1) == 1 && c!= 'q') {
        if (iscntrl(c)) {
            printf("%d\r\n", c);  // Print only the ASCII Value if the input char is non alphanumeric
        } 
        else {
            printf("%d ('%c')\r\n", c, c);  // Print both ASCII Value and the character if the input is alphanumeric  
        }
    }
    return 0;
}
