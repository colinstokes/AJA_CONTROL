#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include "TerminalUtility.h"

// Function to set terminal attributes for non-blocking input
void set_terminal_attributes()
{
   struct termios term_attr;
   tcgetattr(STDIN_FILENO, &term_attr);
   term_attr.c_lflag &= ~(ICANON | ECHO);
   tcsetattr(STDIN_FILENO, TCSANOW, &term_attr);
}

// Function to restor original terminal attributes
void restore_terminal_attributes()
{
   struct termios term_attr;
   tcgetattr(STDIN_FILENO, &term_attr);
   term_attr.c_lflag |= ICANON | ECHO;
   tcsetattr(STDIN_FILENO, TCSANOW, &term_attr);
}