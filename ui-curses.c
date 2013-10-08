/* $Id: ui-curses.c,v 1.2 2003/03/05 08:01:28 twp Exp $ */
#include <ctype.h>
#include <curses.h>
#include <stdlib.h>
#include <string.h>

#include "dict.h"
#include "ui-curses.h"

/* Wurd_Jumble is the set of jumbled letters to choose from.
   Wurd_Guess is the user's workspace where new words are guessed. */
extern char Wurd_Jumble[7];
extern char Wurd_Guess[7];
extern char **Wurd_Answers;
extern DICT Wurd_Answer_Key;
int guess_cursor;

int GUESS_START_X[6] = { 30, 32, 34, 36, 38, 40 };

void ui_draw_box (int, int, int, int);

void
ui_print_gameboard (void)
{
  int p, q;
  char temp;

  initscr();
  cbreak();
  noecho();
  nonl();
  intrflush(stdscr, FALSE);
  keypad(stdscr, TRUE);
  nodelay(stdscr, FALSE);
  clear();

  move(0, ANSWER_BORDER_COLUMN);
  vline(ACS_VLINE, 30);

  ui_update_answers();

  /* Jumble the letters in Wurd_Jumble */
  for (p = 0; p < 6; ++p) {
    q = random() % 6;
    temp = Wurd_Jumble[p];
    Wurd_Jumble[p] = Wurd_Jumble[q];
    Wurd_Jumble[q] = temp;
  }
  memset(Wurd_Guess, 0, 7);
  guess_cursor = 0;

  ui_update_guess();

  ui_draw_box (GUESS_START_X[0]-2,  GUESS_START_Y-1,
	       GUESS_START_X[0]+13, GUESS_START_Y+1);

  refresh();
}

void
ui_command_loop (void)
{
  int ch;
  int i, j;
  while ((ch = getch()) != ERR) {
    /* backspace */
    if ((ch == KEY_DC || ch == KEY_BACKSPACE) && guess_cursor > 0) {
      ch = Wurd_Guess[--guess_cursor];
      Wurd_Guess[guess_cursor] = '\0';
      for (j = 0; j < 6; ++j) {
	if (Wurd_Jumble[j] == '_') {
	  Wurd_Jumble[j] = ch;
	  break;
	}
      }
      ui_update_guess();
      refresh();
    }
    /* Enter/Return */
    else if (ch == '\n' || ch == '\r') {
      if (dict_lookup (Wurd_Answer_Key, Wurd_Guess)) {
	int len = strlen(Wurd_Guess);
	char **p;
	for (p = Wurd_Answers; *p != NULL; ++p) {
	  if (**p == '_' && strlen(*p) == len) {
	    strcpy(*p, Wurd_Guess);
	    dict_delword (Wurd_Answer_Key, Wurd_Guess);
	    ui_update_answers();
	    refresh();
	    break;
	  }
	}
      }
    }
    /* A-Z */
    else if (isalpha(ch)) {
      ch = tolower(ch);
      for (i = 0; Wurd_Jumble[i] != '\0' && !isspace(Wurd_Jumble[i]); ++i) {
	if (ch == Wurd_Jumble[i]) {
	  Wurd_Jumble[i] = '_';
	  Wurd_Guess[guess_cursor++] = ch;
	  ui_update_guess();
	  refresh();
	  break;
	}
      }
    }
  }
}

void
ui_draw_box (int left, int top, int right, int bottom)
{
  mvhline (top,    left,  ACS_HLINE, right-left);
  mvhline (bottom, left,  ACS_HLINE, right-left);
  mvvline (top,    left,  ACS_VLINE, bottom-top);
  mvvline (top,    right, ACS_VLINE, bottom-top);

  mvaddch (top,    left,  ACS_ULCORNER);
  mvaddch (top,    right, ACS_URCORNER);
  mvaddch (bottom, left,  ACS_LLCORNER);
  mvaddch (bottom, right, ACS_LRCORNER);
}

void
ui_update_guess (void)
{
  int i;

  mvaddstr (JUMBLE_START_Y, JUMBLE_START_X, Wurd_Jumble);
  for (i = 0; i < 6; ++i) {
    mvaddch(GUESS_START_Y, GUESS_START_X[i],
	    Wurd_Guess[i] ? Wurd_Guess[i] : '_');
  }
}

void
ui_update_answers (void)
{
  int p;
  for (p = 0; Wurd_Answers[p] != NULL; ++p) {
    move (p + ANSWER_START_ROW, ANSWER_START_COLUMN);
    addstr(Wurd_Answers[p]);
  }
}

void
ui_finish (void)
{
  nl();
  echo();
  endwin();
}
