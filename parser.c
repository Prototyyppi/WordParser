#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <locale.h>
#include <ctype.h>
#include <wchar.h> // for scandi characters
#include "parser.h"

struct word* hashArray[SIZE];
FILE* document;
int datapoints = 0;

int main(int argc, char** argv) {
	char *locale;
	locale = setlocale(LC_ALL, "fi_FI.UTF-8");

	wchar_t* sana;
	int ret;
	if (init_file(argc, argv) != OK)
		return NOK;
	while (((sana = get_word()) )!= NULL) {
	
	if (sana == NULL) return OK_EOF;
	int key = hash(sana);
	ret = find(key, sana);
	if (datapoints > SIZE-1) {
		remove_rare();
	}
	}
	int max_ind = strip_nulls(); // remove nulls to make things easy for quicksort, return max index
	quick_sort(0, max_ind);
	print_result();
	return EXIT_SUCCESS;
}

int find(int key, wchar_t* sana) {

	int ret;
	int index = key;

	#if DEBUG
	printf("%ls", sana);
	printf("%ls", hashArray[index]->word);
	#endif

	while (hashArray[index] != NULL) {
		if (!(wcscmp(hashArray[index]->word, sana))) {
			hashArray[index]->occurences++;
			//printf("Found new %s (%d)\n",hashArray[index]->word, hashArray[index]->occurences);
			return NOK;
		}
	++index;
	index %= SIZE; //wrap around as needed
	}
	add(index, key, sana); // Word not yet occurred, add it
	return OK;
}

int add(int index, int key, wchar_t* sana) {

	struct word *item = malloc(sizeof(struct word));
	datapoints += 1;
	item->key = key;
	wcscpy(item->word, sana);
	item->occurences = 1;
	while (hashArray[index] != NULL)
		index++;// %= SIZE;
	hashArray[index] = item;
	//printf("%d index", index);
	return OK;
}

int remove_rare() {
	int index = 0;
	int ok, curr_min = INT32_MAX, min_index;
	while (index < SIZE) {
		if (hashArray[index] != NULL) {
			if (hashArray[index]->occurences < curr_min)
				min_index = index;
		}
		index++;
		ok = 1;
	}
	//printf("I sHALL REMOVE");
	if (!ok)
		return NOK;
	remove_datapoint(min_index);
	return OK;
}

void remove_datapoint(int index) {
	//printf("I sHALL REMOVE");
	hashArray[index]->key = -1;
	wcscpy(hashArray[index]->word, L"removed");
	hashArray[index]->occurences = -1;
	hashArray[index] = NULL;
	datapoints--;

}

int hash(wchar_t* sana) {
	int hash;
	for (int i = 0; i < 4; i++){
		if (sana[i] != 0) {
			hash *= sana[i];
			hash >>= 2;
		}
	}
	//int hash = (int)sana[0];
	//hash = sana[0] << 24 | sana[1] << 16 | sana[2] << 8 | sana[3]; only for char's
	return hash % SIZE;
}

wchar_t* get_word() {

	wint_t character;
	wint_t charray[2];
	wint_t* ret_str = calloc(50,sizeof(wint_t));
	int ret, ok = 0;
	
	while (ok == 0) {
		while (((character = fgetwc(document)) ) && ((character>=L'A'&&character<=L'Ö')||(character>=L'a'&&character<=L'ö'))){
			//ret = fscanf(document, "%99[a-zA-Z]", ret_str);
			character = towupper(character);
			charray[0] = character;
			charray[1] = 0;
			wcscat(ret_str, charray);
			ok = 1;
		}
		if (character == EOF)
			return NULL;
		else
			continue;
	}

	#if DEBUG
	printf("Current word %ls\n", ret_str);
	#endif

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
	const int MAX_PRINT = maximum_print_size(datapoints);
	for (int index = 0; index < MAX_PRINT; index++){
		if(hashArray[index] != NULL)
			printf("%d %ls %d\n", index, hashArray[index]->word, hashArray[index]->occurences);
	}
	fclose(document);
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
		#if DEBUG
		printf("(%d scan)", scan);
		printf("(%d VALUE)", index);
		#endif
	}
	int sec_index = index;
	for (; sec_index<SIZE; sec_index++) {
		hashArray[sec_index] = NULL;
	}
	return index-1;
}

int quick_sort(int low_index, int high_index) {
	if (low_index < high_index) {

		int middle_index = partition(low_index, high_index);

		quick_sort(low_index, middle_index - 1); // Sort values before middle
		quick_sort(middle_index + 1, high_index); // Sort values after middle

	}
	return OK;
}


int partition(int low_index, int high_index)
{

	// pivot (Element to be placed at right position)
	struct word* pivot = hashArray[high_index];
	struct word* temp;
	int i = low_index-1;

	for (int j = low_index; j <= high_index - 1; j++)
	{
		if (hashArray[j]->occurences >= pivot->occurences) {
			++i;
			if (i != j) {
			temp = hashArray[i];
			hashArray[i] = hashArray[j];
			hashArray[j] = temp;
			}
		}
	}
	temp = hashArray[i+1];
	hashArray[i+1] = hashArray[high_index];
	hashArray[high_index] = temp;
	return i+1;
}
