#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <locale.h>
#include <ctype.h>
#include "parser.h"

struct word* hashArray[SIZE];
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
	int key = hashCode(sana);
	//2. check is it listed if it is continue, else add
	ret = find(key, sana);
	//3. save
	//if (asd==100) return 0;
	}
	print_result();
	return EXIT_SUCCESS;
}

void insert(char* sana) {

	struct word *item = malloc(sizeof(struct word));
	strcpy(sana, item->word);
	item->key = hashCode(sana);

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
		printf("Found new %s (%d)\n",hashArray[index]->word, hashArray[index]->occurences);
		asd++;
		return NOK;
	}
	//go to next cell
	++index;
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
		index++;
	hashArray[index] = item;
	//printf("%d index", index);
	return OK;
}

int hashCode(char* sana) {
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
	for (int index = 0; index<SIZE; index++){
		if(hashArray[index] != NULL)
		printf("%s %d\n", hashArray[index]->word, hashArray[index]->occurences);
	}
	return OK;
}