/*** Includes ***/

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <termios.h>
#include <stdlib.h>
#include <ctype.h>


/*** Data ***/

struct termios orig_termios;


/*** Terminal ***/

void die(const char *s) {
  // Function to kill the current program
  // Print an error message.
  perror(s);

  // Exit with return status 1
  exit(1);
}


void disableRawMode() {
   if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) die("Error while disabling the raw mode. \ndisableRawMode > tcsetattr");   // Set the terminal attributes to the original
}


void enableRawMode() {  
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("Error while readint terminal attributes. \nenableRawMode > tcgetattr");  // Store the current terminal attributes in orig_termios
    atexit(disableRawMode);  // At exit disable the Raw mode
    struct termios raw = orig_termios;  // Store a copy of original terminal attributes in raw struct
    // Turning off some of the input and output flags to enabld the Raw mode in the terminal
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("Error while setting the terminal attributes. \nenableRawMode > tcsetattr");  // Set the terminal attributes
}


/*** Init ***/

int main() {
    // Enable RAW Mode
    enableRawMode();
    
    char c;
    while (1) {
        char c = '\0';
        if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("Error while reading input");
        
        if (iscntrl(c)) {
            printf("%d\r\n", c);    // Printing the ASCII value
        } 
        else {
            printf("%d ('%c')\r\n", c, c);    // Printing the ASCII value and the character
        }
        if (c == 'q') break;
    }   
    return 0;
}
