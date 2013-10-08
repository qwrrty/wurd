#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dict.h"
#include "ui-curses.h"

/*
 * Wurd: an anagram game.
 *
 * In a game of Wurd, the computer randomly selects a six-letter dictionary
 * word and scrambles the letters.  The player must identify all of the 3-,
 * 4-, 5- and 6-letter words that can be made from the jumbled word.  The
 * computer shows how many words can be made from the source word, sorted
 * by length.
 */

/*
 * An array of valid six-letter words from the dictionary. The computer
 * chooses one randomly at the start of the game.
 *
 * Each element N of the wordlengths array represents the number of N-letter
 * words that may be made from the current target word.  The countlengths
 * function, which is called by dict_traverse, populates this array.
 */
extern struct word_list sixwords;

int wordlengths[7];
void countlengths (char *);

/*
 * Wurd_Dictionary is the dictionary of all possible words.
 * Wurd_Jumble is the jumbled six-letter target word.
 * Wurd_Guess is the user's workspace for guessing a word.
 * Wurd_Answers are the answers the user has guessed so far.
 * Wurd_Answer_Key is the dictionary of legitimate answers
 *	for the current puzzle.
 */
DICT Wurd_Dictionary;
char Wurd_Jumble[7];
char Wurd_Guess[7];
char **Wurd_Answers;
DICT Wurd_Answer_Key;

int
main (int argc, char **argv)
{
  int a, i;

  setlocale(LC_ALL, "");

  /* start a new game */
  dict_initialize(NULL);
  newgame(Wurd_Jumble, &Wurd_Answer_Key);

  /* set up UI data structures */
  memset(wordlengths, 0, 7);
  dict_traverse(Wurd_Answer_Key, countlengths);

  Wurd_Answers = (char **) malloc (sizeof(char *) *
				   (wordlengths[3] + wordlengths[4] +
				    wordlengths[5] + wordlengths[6] + 1));
  a = 0;
  for (i = 0; i < wordlengths[3]; ++i) {
    Wurd_Answers[a++] = strdup("___");
  }
  for (i = 0; i < wordlengths[4]; ++i) {
    Wurd_Answers[a++] = strdup("____");
  }
  for (i = 0; i < wordlengths[5]; ++i) {
    Wurd_Answers[a++] = strdup("_____");
  }
  for (i = 0; i < wordlengths[6]; ++i) {
    Wurd_Answers[a++] = strdup("______");
  }
  Wurd_Answers[a] = NULL;

  /* render the gameboard */
  ui_print_gameboard();
  ui_command_loop();
  ui_finish();

  return 0;
}

/*
 * countlengths
 *
 *	Thunk for `wtree_traverse'.  Update global `wordlengths' array
 *	with length of each word in a dictionary.
 */
void
countlengths (char *word)
{
  wordlengths[strlen(word)]++;
}
