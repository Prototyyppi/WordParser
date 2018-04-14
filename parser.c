#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h> // Highest int value
#include <locale.h> // Prequiosite for scandi characters
#include <errno.h>
#include <wchar.h> // for scandi characters
#include "parser.h" //The header file

#if GRAPHICAL
#ifdef WIN32
#define WINDOWS 1
#else
#include <unistd.h>
#include <signal.h> //signals
#include <time.h> //nanosleep
#endif
#endif

struct word* hashArray[SIZE];
FILE* document;
int datapoints = 0;
int wordcount = 0;


int main(int argc, char** argv) {

	#ifdef WINDOWS
	printf("Please define graphical to 0 on windows environment\n");
	return EXIT_FAILURE;
	#endif

	wint_t* sana;
	int ret;
	char *locale;

	locale = setlocale(LC_ALL, "fi_FI.UTF-8");
	if (locale == NULL)
		printf("Could not find finnish locale\n");

	if (init_file(argc, argv) != OK)
		return NOK;
	printf("\n------------------------------------\n");
	printf("\nCALCULATING 100 MOST COMMON WORDS...\n");
	printf("\n------------------------------------\n");

	#if GRAPHICAL
	pid_t pid = fork();
	if (pid == -1){
		printf("Fork failed\n");
	}
	if (pid == 0)
		printfunc();
	#endif

	while ((sana = get_word()) != NULL) {
	uint32_t key = hash(sana);
	ret = find(key, sana);
	free(sana);
	if (datapoints > SIZE-1) {
		remove_rare();
	}
	}
	int max_ind = strip_nulls(); // remove nulls to make things easy for quicksort, return max index
	quick_sort(0, max_ind);

	#if GRAPHICAL
	if (pid != 0 && pid != -1)
		kill(pid, SIGKILL);
	#endif

	print_result();
	return EXIT_SUCCESS;
}

#if GRAPHICAL
int printfunc(){
	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = (180000000L);
	printf("PLEASE WAIT: ");
	while(1) {
		printf("#");
		nanosleep(&ts, NULL);
		fflush(stdout);
	}
}
#endif

int find(uint32_t key, wint_t* sana) {

	int ret;
	uint32_t index = key;

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

int add(uint32_t index, uint32_t key, wint_t* sana) {

	struct word *item = malloc(sizeof(struct word));
	datapoints += 1;
	item->key = key;
	wcscpy(item->word, sana);
	item->occurences = 1;
	while (hashArray[index] != NULL)
		index++;// %= SIZE;
	hashArray[index] = item;
	//printf("%d index", index);
	wordcount++;
	return OK;
}

int remove_rare() {
	uint32_t index = 0;
	int ok, curr_min = INT32_MAX, min_index;
	while (index < SIZE) {
		if (hashArray[index] != NULL) {
			if (hashArray[index]->occurences < curr_min)
				min_index = index;
		}
		index++;
		ok = 1;
	}
	if (!ok)
		return NOK;
	remove_datapoint(min_index);
	return OK;
}

void remove_datapoint(uint32_t index) {
	//printf("I sHALL REMOVE");
	hashArray[index]->key = -1;
	wcscpy(hashArray[index]->word, L"removed");
	hashArray[index]->occurences = -1;
	hashArray[index] = NULL;
	datapoints--;

}

uint32_t hash(wint_t* sana) {
	uint32_t hash;
	for (int i = 0; i < 4; i++){
		if (sana[i] != 0) {
			hash *= sana[i];
			//hash >>= 2;
		}
	}
	//uint32_t hash = (uint32_t)sana[0];
	//hash = sana[0] << 24 | sana[1] << 16 | sana[2] << 8 | sana[3]; only for char's
	return hash % SIZE;
}

wint_t* get_word() {

	wint_t character;
	wint_t charray[2];
	wint_t* ret_str = calloc(WORD_MAX_LEN,sizeof(wint_t));
	int ret, ok = 0;
	while (ok == 0) {
		//while (((character = fgetwc(document)) ) && ((character>=L'A'&&character<=L'Ö')||(character>=L'a'&&character<=L'ö'))){
			while (((character = fgetwc(document)) ) && ((character>=L'A'&&character<=L'Z')||
			(character>=L'a'&&character<=L'z') || ((character > 0xC0) && (character <= 0xFF ))
																		|| (character == '\''))) {
			//UTF-8 All latin characters are from 0xC0 -> 0xFF
			character = towupper(character);
			charray[0] = character;
			charray[1] = 0;
			wcsncat(ret_str, charray, WORD_MAX_LEN);
			//wprintf(L"%ls", charray);
			//overflow can happen, that is why based on strncat
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
	/*
	fseek(document, 0, SEEK_END);
	filelen = ftell(document);
	*/
	rewind(document);
	return OK;
}

int print_result() {
	const int MAX_PRINT = maximum_print_size(datapoints);
	printf("\nFound %d different words:\n", wordcount);
	for (uint32_t index = 0; index < MAX_PRINT; index++) {
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
