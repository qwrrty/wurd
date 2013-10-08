/* $Id: dict.h,v 1.2 2003/03/05 08:01:28 twp Exp $ */

/*
 * dict.h -- dictionary management for Wurd.
 *
 * A Wurd "dictionary" is a 26-way trie. Each node in the trie represents a
 * partial dictionary word; each child link represents a letter that can
 * be added to that word to form another valid prefix. If a node has a
 * non-NULL value for the 'word' field, that represents a complete word found
 * in the dictionary.
 *
 * e.g. from the root node, the word "apple" is found by:
 *
 * root->next['a']->next['p']->next['p']->next['l']->next['e']
 *
 * The 'word' field at this node is set to the string "apple".
 *
 * Adding or looking up an N-letter word in the dictionary is therefore
 * linear in N.
 */

struct dictionary_node {
  char *word;
  struct dictionary_node *next[26];
};

/* 
 * A simple dynamic array for storing the complete wordlist.
 */
struct word_list {
  int size;      // total number of allocated words
  int freeidx;   // index of the next available slot
  char **words;  // array of words
};

typedef struct dictionary_node *DICT;

typedef void (*DTHUNK)(char *);

void	dict_initialize (char *);
void	newgame (char *, DICT *);
int     dict_legalword (char* word);
void	dict_addword (DICT dictionary, char *word);
void	dict_delword (DICT dictionary, char *word);
int	dict_lookup (DICT, char *);
DICT	dict_new (void);
void	dict_anagram (DICT dictionary, DICT anagrams, char *letters);
void	dict_traverse (DICT dictionary, DTHUNK thunk);
void	wlist_add (struct word_list *wlist, char *word);
void	countlengths (char *word);

extern DICT Wurd_Dictionary;
