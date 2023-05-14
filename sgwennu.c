#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct termios orig_termios;

void disableRawMode() {  // Switch back to normal terminal input.
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode() {	// Turns off echoing in the terminal.
	tcgetattr(STDIN_FILENO, &orig_termios);
	atexit(disableRawMode);  // Switch echoing back on when exit.

	// Store original terminal state.
	struct termios raw = orig_termios;

	/*
	~ is the bitwise NOT operator, which here results in echo mode
	being disabled. It also disables canonical mode, which means
	input is read byte-by-byte rather than line-by-line.
	*/ 
	raw.c_lflag &= ~(ECHO | ICANON);

	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main() {
	enableRawMode();

	char c;
	// Displays keypresses unless q pressed, in which case quit.
	while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q') {
		if (iscntrl(c)) {
			printf("%d\n", c);
		} else {
			printf("%d ('%c')\n", c, c);
		}
	}

	return 0;
}
