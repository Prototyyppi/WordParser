
#define SIZE 1000
#define WORD_MAX_LEN 100

struct word {
	char word[WORD_MAX_LEN];
	int occurences;
	int key;
};

enum err {
	OK,
	NOK
};

void insert(char* sana);

int find(int key, char sana[]);

int add(int index, int key, char* sana);

int hashCode(char* sana);

int init_file(int, char**);

char* get_word();

int print_result();