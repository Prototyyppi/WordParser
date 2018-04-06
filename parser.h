
#define SIZE 100
#define WORD_MAX_LEN 100

struct word {
	char word[WORD_MAX_LEN];
	int occurences;
	int key;
};


void insert(char* sana);

int find(int key, char sana[]);

int add(int index, int key, char* sana);

int hashCode(char* sana);

char* get_word();