#pragma once //__PARSER_FILE

/*
 * Parser - 2018
 *
 * Parses text files and reports how many times each word has appeared.
 * Still to do, check if memory could be allocated better
 */

//Size should be over wanted value to make indexes unique and to fit the wanted words
#define SIZE 5000
#define WORD_MAX_LEN 100
#define maximum_print_size(x) ((x) > 100 ? 100 : SIZE-1)
#define DEBUG 0

struct word {
	wchar_t word[WORD_MAX_LEN];
	int occurences;
	int key;
};

enum err {
	OK,
	NOK,
	OK_EOF
};

//inline may increase speed depending on compiler
static inline const char* err2str(enum err err)
{
	static const char* strings[] = { "OK", "NOK", "OK_EOF"};
	return strings[err];
}

/* check if word is already found */
int find(int key, wchar_t sana[]);

/* word was not foud, add it */
int add(int index, int key, wchar_t* sana);

/* perform removal */
void remove_datapoint(int index);

/* if dataset is constrained, we need to delete least common words, note that
 * this may result in wrong output and size should be kept high enough
 */
int remove_rare(void);

/* calculate hash that should be unique for each value. It helps find values faster */
int hash(wchar_t* sana);

/*Initialize text file, take argument from command line parameter */
int init_file(int, char**);

/* Get next word. Scandi charwacters are ok too */
wchar_t* get_word();

/* Prints resulting 100 most relevant values */
int print_result();

/* Strip nulls to help with sorting */
int strip_nulls();

/* The quick sort algorithm */
int quick_sort(int low_index, int high_index);

/* Quick sort get last value and sort smaller and bigger on either side of it */
/* Excellent illustration of quicksort can be found at https://en.wikipedia.org/wiki/Quicksort */
int partition(int low_index, int high_index);
