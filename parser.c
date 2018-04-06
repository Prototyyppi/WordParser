#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "parser.h"

struct word* hashArray[SIZE];
static int asd =0;

int main(int argc, char** argv) {
	printf("HERE");
	char* sana;
	int ret;
	
	while (((sana = get_word() )!= NULL)){
	//1. get word
	int key = hashCode(sana);
	//2. check is it listed if it is continue, else add
	ret = find(key, sana);
	if(asd > 20) return 0;
	//3. save
	}
	return EXIT_SUCCESS;
}

void insert(char* sana) {

	struct word *item = malloc(sizeof(struct word));
	strcpy(sana, item->word);
	item->key = hashCode(sana);

	//get the hash 
	int hashIndex = 0;

	//move in array until an empty or deleted cell
	while(hashArray[hashIndex] != NULL && hashArray[hashIndex]->key != -1) {
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

	while(hashArray[index] != NULL) {
	//printf("%s", sana);
	//printf("%s", hashArray[index]->word);
	if(!(strcmp(hashArray[index]->word, sana))) {
		hashArray[index]->occurences++;
		printf("\n%d\n", hashArray[index]->occurences);
		asd++;
		return 1;
	}
	//go to next cell
	++index;
	}
	printf("Wasd%d", asd);
	asd++;
	
	ret = add(index, key, sana);
	return 0;
}
int add(int index, int key, char* sana) {

	struct word *item = malloc(sizeof(struct word));

	item->key = key;
	strcpy(item->word, sana);
	item->occurences = 1;
	hashArray[index] = item;
	printf("%d index", index);
	return 0;
}

int hashCode(char* sana) {
	int hash = 0;
	hash = sana[0] << 24 | sana[1] << 16 | sana[2] << 8 | sana[3];
	return hash % SIZE;
}

char* get_word(void) {
	return "String";
}
