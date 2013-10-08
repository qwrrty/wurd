/* $Id: dict.c,v 1.2 2003/03/05 08:01:28 twp Exp $ */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include "dict.h"
#include "ui-curses.h"

#define DEFAULT_DICTFILE "/usr/share/dict/words"
#define MAXWORDLEN 80

extern DICT Wurd_Dictionary;

struct word_list sixwords = { 0, 0, NULL };

/*
 * init_dictionary
 *
 * Reads words from `dictfile' and stores them in the global Wurd_Dictionary.
 */
void
dict_initialize (char *dictfile)
{
  FILE *fdict;
  char *word = NULL;
  size_t word_len, buffer_size;

  if (dictfile == NULL)
    dictfile = DEFAULT_DICTFILE;

  /*
   * Read words from `dictfile' and add all 3- 4- 5- and 6-letter words
   * to the word tree.  Six letter words also get added to the
   * `sixwords' list.
   */

  fdict = fopen (dictfile, "r");
  if (fdict == NULL) {
    fprintf (stderr, "could not open %s: %s\n", dictfile, strerror(errno));
    return;
  }

  Wurd_Dictionary = dict_new();
  while ((word_len = getline(&word, &buffer_size, fdict)) != -1) {
    if (word[word_len-1] == '\n') {
      word[--word_len] = '\0';
    }
    if (dict_legalword(word)) {
      /* add six-letter words to sixwords */
      if (word_len == 6) {
	wlist_add (&sixwords, word);
      }

      /* put it in the dictionary */
      dict_addword (Wurd_Dictionary, word);
    }

    free(word);
    word = NULL;
  }

  fclose(fdict);
}

/*
 * newgame
 *
 * Starts a new game of Wurd.
 *
 * A six-letter word is randomly chosen from the dictionary, and all
 * legal anagrams are calculated.
 *
 * arguments:
 *   * gameword: the buffer in which to store the target
 *       six-letter word for this game
 *   * anagrams: pointer to the dictionary where the valid anagrams for
 *       the game word should be stored.
 *
 * side effects:
 *   * replaces the contents of gameword.
 *   * replaces anagrams with a newly populated dictionary of valid solutions
 */
void
newgame (char *gameword, DICT *anagrams)
{
  int i;
  char *workspace;

  /* choose a six-letter word */
  srand(time(NULL));
  i = rand() % sixwords.freeidx;
  strncpy(gameword, sixwords.words[i], 6);
  gameword[6] = '\0';

  /* get the anagrams */
  *anagrams = dict_new();
  workspace = strdup(sixwords.words[i]);
  dict_anagram(Wurd_Dictionary, *anagrams, workspace);
  free(workspace);

  return;
}

/*
 * dict_legalword
 *
 * Returns 1 if a word is "legal" (between 3 and 6 letters in length,
 * and consisting only of ASCII lowercase characters).
 */
int
dict_legalword (char *word)
{
  int word_len = strlen(word);
  int i;

  /* we only want words 3-6 letters long */
  if (word_len < 3 || word_len > 6) {
    return 0;
  }
  /* skip proper names and non-ASCII words */
  for (i = 0; i < word_len; ++i) {
    if (isupper(word[i]) || !isascii(word[i]) || !isalpha(word[i])) {
      return 0;
    }
  }
  return 1;
}

/*
 * dict_addword
 *
 * Adds a word to the dictionary if not already present.
 */
void
dict_addword (DICT dict, char *word)
{
  char *p;

  p = word;

  while (*p && !isspace(*p)) {
    int letter = *p - 'a';
    if (dict->next[letter] == NULL) {
      dict->next[letter] = dict_new();
    }
    dict = dict->next[letter];
    ++p;
  }

  if (dict->word == NULL)
    dict->word = strdup(word);
}

/*
 * dict_delword
 *
 * Removes a word from the dictionary.
 */
void
dict_delword (DICT dict, char *word)
{
  char *p;
  int i;

  for (p = word; *p; ++p) {
    i = *p - 'a';
    if (dict->next[i] == NULL)
      return;
    dict = dict->next[i];
  }
  if (dict->word && strcmp(dict->word, word) == 0) {
    free(dict->word);
    dict->word = NULL;
  }
}

/*
 * dict_lookup
 *
 * Returns 1 if a word is found in the dictionary, 0 otherwise.
 */
int
dict_lookup (DICT dict, char *word)
{
  char *p;
  int i;

  for (p = word; *p; ++p) {
    i = *p - 'a';
    if (dict->next[i] == NULL)
      return 0;
    dict = dict->next[i];
  }
  return (dict->word && strcmp(word, dict->word) == 0);
}

/*
 * dict_anagram
 *
 * Identifies all anagrams that can be made from a given word.
 *
 * For each letter in the word, use it as the starting letter of
 * an anagram, then recursively determine if the remaining letters
 * can be added to the anagram to make a valid dictionary word.
 *
 * `letters' is used as a workspace; each time a letter is selected,
 * it is changed to _ to mark it as unavailable for the recursive
 * calls, and is then changed back before returning.
 *
 * The worst case behavior for an N-letter word is 26^N. When run
 * against a real human dictionary, however, many letter combinations
 * will be eliminated quickly, when it becomes clear that they do not
 * lead to a valid word in the trie.
 *
 * arguments:
 *   * dictionary: trie of all valid dictionary words
 *   * anagrams: dictionary of valid anagrams that can be made
 *   * letters: our target word
 *
 * returns: none
 *
 * side effects: populates the `subwords' dictionary with anagrams
 *   that can be made from `letters'
 */
void
dict_anagram (DICT dictionary, DICT anagrams, char *letters)
{
  int i;
  char done[255];

  if (dictionary->word != NULL) {
    dict_addword (anagrams, dictionary->word);
  }

  memset (done, 0, 255);
  for (i = 0; letters[i] != '\0'; ++i) {
    unsigned char c = letters[i];
    int next_letter = c - 'a';
    if (isalpha(c) && dictionary->next[next_letter] != NULL && !done[c]) {
      letters[i] = '_';
      dict_anagram(dictionary->next[next_letter], anagrams, letters);
      letters[i] = c;
      done[c] = 1;
    }
  }
}

/*
 * dict_traverse
 *
 * Walk through each node of a given dictionary and call `thunk'
 * on each word.
 */
void
dict_traverse (DICT dict, DTHUNK thunk)
{
  int i;

  if (dict == NULL)
    return;

  if (dict->word != NULL) {
    (*thunk)(dict->word);
  }

  for (i = 0; i < 26; ++i) {
    if (dict->next[i] != NULL)
      dict_traverse(dict->next[i], thunk);
  }
}

/*
 * dict_new
 *
 * Returns a new (empty) dictionary object.
 */
DICT
dict_new (void)
{
  DICT dict;
  int i;

  dict = (DICT) malloc (sizeof(struct dictionary_node));
  dict->word = NULL;
  for (i = 0; i < 26; ++i) {
    dict->next[i] = NULL;
  }
  return dict;
}

/*
 * wlist_add
 *
 * Add `word' to word list `wlist'.
 */
void
wlist_add (struct word_list *wlist, char *word)
{
  if (wlist->freeidx >= wlist->size) {
    wlist->size += 100;
    wlist->words = (char **) realloc (wlist->words, wlist->size * sizeof(char *));
  }
  wlist->words[wlist->freeidx++] = strdup(word);
}

