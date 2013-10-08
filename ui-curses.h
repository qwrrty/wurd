/* $Id: ui-curses.h,v 1.2 2003/03/05 08:01:28 twp Exp $ */

#define ANSWER_BORDER_COLUMN 10
#define ANSWER_START_COLUMN 2
#define ANSWER_START_ROW 2
#define JUMBLE_START_X 33
#define JUMBLE_START_Y 20
#define GUESS_START_Y 15

extern int GUESS_START_X[6];

void ui_print_gameboard();
void ui_command_loop(void);
void ui_update_answers(void);
void ui_update_guess(void);
void ui_finish(void);
