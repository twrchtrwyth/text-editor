/*** includes ***/

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

/*** defines ***/

/* This is a macro. 0x1f in hexadecimal = 00011111 in binary.
This sets the upper 3 bits of the character to 0, which is 
similar to what the Ctrl key does in the terminal. */
#define CTRL_KEY(k) ((k) & 0x1f)

/*** data ***/

struct termios orig_termios;

/*** terminal ***/

void die(const char *s) {
  perror(s);  // Prints descriptive error message in event of failure.
  exit(1);  // Exits the program with exit status 1 (indicating failure)
}

void disableRawMode() {  // Switch back to normal terminal input.
  /* Note the syntax here to call `die` in event of failure.
  When tcsetattr fails it returns -1. */
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) die("tcsetattr");
}

void enableRawMode() {	// Turns off echoing in the terminal.
  if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
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
  /* VMIN sets minimum number of bytes of input needed before the
  read() function can return. 0 means that returns as soon as
  there is any input to be read. cc = control characters. */
  raw.c_cc[VMIN] = 0;
  /* VTIME sets maximum amount of time to wait before read()
  returns. Given in tenths of seconds so 1 = 100ms. If read()
  times out, it will return 0. */
  raw.c_cc[VTIME] = 1;

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

char editorReadKey() { //Waits for keypress then returns it. 
  int nread;
  char c;
  while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    // Ignore EAGAIN for Cygwin compatibility.
    if (nread == -1 && errno != EAGAIN) die("read");
  }
  return c;
}

/*** input ***/

void editorProcessKeypress() { //Waits for keypress then handles.
  char c = editorReadKey();

  switch (c) {
    case CTRL_KEY('q'):
      exit(0);
      break;
  }
}

/*** init ***/

int main() {
  enableRawMode();

  while (1) {
    editorProcessKeypress();
  }

  return 0;
}
