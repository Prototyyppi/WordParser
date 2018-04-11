/*
 * Header for parser
 *
 */

//Size should be over wanted value to make indexes unique and to fit the wanted words
#define SIZE 1000
#define WORD_MAX_LEN 100
#define MAX_PRINT_SIZE 100
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

void insert(char* sana);

int find(int key, wchar_t sana[]);

int add(int index, int key, wchar_t* sana);

int hash(wchar_t* sana);

int init_file(int, char**);

wchar_t* get_word();

int print_result();

int strip_nulls();

int quick_sort(int low_index, int high_index);

int partition(int low_index, int high_index);