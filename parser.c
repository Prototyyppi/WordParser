#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <locale.h>
#include <ctype.h>
#include "parser.h"

struct word* hashArray[SIZE];
struct word* secondArray[SIZE];
static int asd = 0;
FILE* document;

int main(int argc, char** argv) {
	//char *locale;
	//locale = setlocale(LC_ALL, "fi_FI.UTF-8");
	char* sana;
	int ret;
	ret = init_file(argc, argv);
	if (ret != OK)
		return NOK;
	while (((sana = get_word()) )!= NULL) {
	if (sana == NULL) return 0;
	//1. get word
	int key = hash(sana);
	//2. check is it listed if it is continue, else add
	ret = find(key, sana);
	//3. save
	//if (asd==100) return 0;
	}
	int max_ind = strip_nulls();
	quick_sort(0, max_ind);
	print_result();
	return EXIT_SUCCESS;
}

void insert(char* sana) {

	struct word *item = malloc(sizeof(struct word));
	strcpy(sana, item->word);
	item->key = hash(sana);

	//get the hash 
	int hashIndex = 0;

	//move in array until an empty or deleted cell
	while (hashArray[hashIndex] != NULL && hashArray[hashIndex]->key != -1) {
		//go to next cell
		++hashIndex;
		
		//wrap around the table
		hashIndex %= SIZE;
	}
	
	hashArray[hashIndex] = item;
}

int find(int key, char* sana) {
	int ret;
	//item->key = key;
	int index = key;

	while (hashArray[index] != NULL) {
	//printf("%s", sana);
	//printf("%s", hashArray[index]->word);
	if (!(strcmp(hashArray[index]->word, sana))) {
		hashArray[index]->occurences++;
		//printf("Found new %s (%d)\n",hashArray[index]->word, hashArray[index]->occurences);
		asd++;
		return NOK;
	}
	//go to next cell
	++index;
	//wrap around as needed
	index %= SIZE;
	//printf("index %d",index);
	}
	//printf("New %s\n", sana);
	asd++;
	
	ret = add(index, key, sana);
	return OK;
}
int add(int index, int key, char* sana) {

	struct word *item = malloc(sizeof(struct word));

	item->key = key;
	strcpy(item->word, sana);
	item->occurences = 1;
	while (hashArray[index] != NULL)
		index++;// %= SIZE;
	hashArray[index] = item;
	//printf("%d index", index);
	return OK;
}

int hash(char* sana) {
	int hash = 0;
	hash = sana[0] << 24 | sana[1] << 16 | sana[2] << 8 | sana[3];
	return hash % SIZE;
}

char* get_word() {
	// A format specifier as [=%[*][width][modifiers]type=]
	char character;
	char charray[2];
	char* ret_str = calloc(50,1);
	int ret, ok = 0;
	while (ok == 0) {
	while (((character = fgetc(document)) ) && ((character>='A'&&character<='Z')||(character>='a'&&character<='z'))){
	//ret = fscanf(document, "%99[a-zA-Z]", ret_str);
	character = toupper(character);
	charray[0] = character;
	charray[1] = 0;
	strcat(ret_str, charray);
	ok = 1;
	}
	//printf("loop %d", character);
	if (character == EOF) break;
	else continue;
	}
	if (character == EOF) return NULL;
	//printf("Current word %s\n", ret_str);
	return ret_str;
}
int init_file(int argc, char** argv) {
	if (argc < 1) return NOK;
	if ((document = fopen(argv[1], "r")) == NULL){
		printf("Please check filename (%s)\n", argv[1]);
		return NOK;
	}
	rewind(document);
	return OK;
}

int print_result() {
	for (int index = 0; index<MAX_PRINT_SIZE; index++){
		if(hashArray[index] != NULL)
		printf("%d %s %d\n", index, hashArray[index]->word, hashArray[index]->occurences);
	}
	return OK;
}

int strip_nulls() {
	int scan = 0;
	int prev = 0;
	int index;
	for (index = 0; index<SIZE; index++) {
		if (hashArray[scan] == NULL){
			while (hashArray[scan] == NULL)
				scan++;
		
		}
		if (scan >= SIZE)
			break;
		if (index != scan)
			hashArray[index] = hashArray[scan];
		scan++;
		//printf("(%d scan)", scan);
		//printf("(%d VALUE)", index);
	}
	int sec_index = index;
	for (; sec_index<SIZE; sec_index++) {
		hashArray[sec_index] = NULL;
	}
	//printf("%d VALUE", index);
	return index-1;
}

int quick_sort(int low_index, int high_index) {
	if (low_index < high_index) {

		int middle_index = partition(low_index, high_index);
		
		quick_sort(low_index, middle_index - 1);  // Before pi

		quick_sort(middle_index + 1, high_index); // After pi

	}
	return OK;
}


int partition(int low_index, int high_index)
{

	// pivot (Element to be placed at right position)
	struct word* pivot = hashArray[high_index];
	struct word* temp;

	int i = low_index-1;
	//while (hashArray[low_index] == NULL) ++low_index;

	for (int j = low_index; j <= high_index - 1; j++)
	{

		if (hashArray[j]->occurences >= pivot->occurences) {
			++i;
			if (i != j){
			temp = hashArray[i];
			hashArray[i] = hashArray[j];
			hashArray[j] = temp;
			}
			
		}
	}

	//while (hashArray[i+1] == NULL) ++i;
	temp = hashArray[i+1];
	hashArray[i+1] = hashArray[high_index];
	hashArray[high_index] = temp;
	return i+1;
}