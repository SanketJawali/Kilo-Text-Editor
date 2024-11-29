/*** Includes ***/

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <ctype.h>


/*** defines ***/

#define CTRL_KEY(k) ((k) & 0x1f)


/*** Data ***/

struct editorConfig {
    int screenrows;
    int screencols;
    struct termios orig_termios;
};

struct editorConfig E;


/*** Terminal ***/

void die(const char *s) {
    // Function to kill the current program
    
    // Clear the scren and reposition the cursor before exiting
    write(STDOUT_FILENO, "\x1b[2J", 4);  
    write(STDOUT_FILENO, "\x1b[H", 3);

    // Print an error message.
    perror(s);

    // Exit with return status 1
    exit(1);
}

void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1) die("Error while disabling the raw mode. \ndisableRawMode > tcsetattr");   // Set the terminal attributes to the original
}

void enableRawMode() {  
    if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1) die("Error while readint terminal attributes. \nenableRawMode > tcgetattr");  // Store the current terminal attributes in orig_termios
    atexit(disableRawMode);  // At exit disable the Raw mode
    struct termios raw = E.orig_termios;  // Store a copy of original terminal attributes in raw struct

    // Turning off some of the input and output flags to enabld the Raw mode in the terminal
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("Error while setting the terminal attributes. \nenableRawMode > tcsetattr");  // Set the terminal attributes
}

char editorReadKey() {
    // Function to read the keypresses and return them
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO,  &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN) die("Error while reading keyboard input."); 
    }
    return c;
}

int getWindowSize(int *rows, int *cols) {
    struct winsize ws;

    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, %ws) == -1 || ws.ws_col == 0) {
        return -1;
    }
    else {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}

/*** output ***/

void editorDrawRows() {
    int y;
    for (y = 0; y < E.screenrows; y++) {
        write(STDOUT_FILENO, "~\r\n", 3);  // Draw ~
    }
}

void editorRefreshScreen() {
    // Clear the screen
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    // Draw the ~ and reset the cursor position
    editorDrawRows();
    write(STDOUT_FILENO, "\x1b[H", 3);
}


/*** Input ***/

void editorProcessKeypress() {
    // Function to read and process the keypresses
    // Read keypresses
    char c = editorReadKey();

    // Process the keypresses
    switch (c) {
        case CTRL_KEY('q'):
            // Clear the screen and reset the cursor position
            write(STDOUT_FILENO, "\x1b[2J", 4);  
            write(STDOUT_FILENO, "\x1b[H", 3);

            // Exit if ctrl+q is pressed
            exit(0);
            break;
    }
}


/*** Init ***/

void initEditor() {
    if (getWindowSize(&E.screenrows, &E.screencols) == -1) die("Error while getting terminal dimensions.");  // Get terminal window dimensions
}

int main() {
    enableRawMode();  // Enable Raw mode
    initEditor();  // Get terminal window size
    
    while (1) {
        // Clear the screen
        editorRefreshScreen();
        // Take keypresses and process them
        editorProcessKeypress();
    }   
    return 0;
}
