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
	~ is the bitwise NOT operator. This reverses the bits of 
	flags within parentheses, which effectively disables each
	one.
	There are two groups of flags:
		* Input flags (c_iflag)
		* Output flags (c_oflag)
		* Local flags (c_lflag)
	ECHO: disable printing to the screen
	ICANON: disable canonical mode, which means input is now read
	byte-by-byte rather than line-by-line
	ISIG: disable SIGINT signals like Ctrl-C and Ctrl-Z
	IXON: disable flow control with Ctrl-S & Ctrl-Q (XON/XOFF)
	IEXTEN: disable Ctrl-V being reserved for literal input
	ICRNL: disable Ctrl-M as newline (NL) not carriage return (CR)
	OPOST: disable all output processing, which stops \n -> \r\n
	Misc flags are BRKINT, INPCK, ISTRIP and CS8. None of these
	probably required in modern terminal emulators but are part
	of enabling "raw mode" for historical reasons.
	*/
	raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	raw.c_oflag &= ~(OPOST);
	raw.c_cflag |= (CS8); // | is bitwise-OR operator
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main() {
	enableRawMode();

	char c;
	// Displays keypresses unless q pressed, in which case quit.
	while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q') {
		if (iscntrl(c)) {
			printf("%d\r\n", c); // Need carriage return + newline
		} else {
			printf("%d ('%c')\r\n", c, c);
		}
	}

	return 0;
}
