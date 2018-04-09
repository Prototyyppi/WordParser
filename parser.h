/*
 * Header for parser
 *
 */

//Size should be over wanted value to make indexes unique and to fit the wanted words
#define SIZE 1000
#define WORD_MAX_LEN 100
#define MAX_PRINT_SIZE 100
struct word {
	char word[WORD_MAX_LEN];
	int occurences;
	int key;
};

enum err {
	OK,
	NOK
};

//inline may increase speed
static inline const char* err2str(enum err err)
{
	static const char* strings[] = { "OK", "NOK"};

	return strings[err];
}

void insert(char* sana);

int find(int key, char sana[]);

int add(int index, int key, char* sana);

int hash(char* sana);

int init_file(int, char**);

char* get_word();

int print_result();

int strip_nulls();

int quick_sort(int low_index, int high_index);

int partition(int low_index, int high_index);