#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h> // Highest int value
#include <locale.h> // Prequiosite for scandi characters
#include <wchar.h> // for scandi characters
#include "parser.h" //The header file

#if EXPERIMENTAL_GRAPHICAL
#include <unistd.h>
#include <signal.h> //signals
#include <time.h> //nanosleep
#endif

struct word* hashArray[SIZE];
FILE* document;
int datapoints = 0, wordcount = 0, filelen_block = 0, filelen_block_size = 0, trig = 1;

int main(int argc, char** argv) {

#ifdef WINDOWS
	printf("Please define exp. graphical to 0 on windows environment\n");
	return EXIT_FAILURE;
#endif

	wchar_t* sana;
	int ret;
	char *locale;

	locale = setlocale(LC_ALL, ""); // Use the default locale (prob unicode (max 16 bit))
	if (locale == NULL)
		printf("Could not find locale\n"); // also text file encoding should be unicode

	if (init_file(argc, argv) != OK)
		return EXIT_FAILURE;

	printf("\n------------------------------------\n");
	printf("CALCULATING 100 MOST COMMON WORDS...");
	printf("\n------------------------------------\n");
	printf("~~~~~~~~~~~~PLEASE WAIT!~~~~~~~~~~~~\n");

#if EXPERIMENTAL_GRAPHICAL
	pid_t pid = fork();
	if (pid == -1){
		printf("Fork failed\n");
	}
	if (pid == 0)
		printfunc();
#endif

	while ((sana = get_word()) != NULL) {
	uint32_t key = hash(sana);
	find(key, sana);
	free(sana);
	if (datapoints > SIZE-1) {
		remove_rare();
	}

#if GRAPHICAL
	int curr_pos = ftell(document);
	if (((curr_pos > filelen_block - 70) && (curr_pos < filelen_block + 70)) && trig) {
		filelen_block += filelen_block_size;
		
		if (trig == 35)
			trig = 0;
		trig++;
		printf("%lc", 0x2588);
		fflush(stdout);
	}
#endif
	}

	int max_ind = strip_nulls(); // remove nulls to make things easy for quicksort, return max index

	quick_sort(0, max_ind);

#if EXPERIMENTAL_GRAPHICAL
	if (pid != 0 && pid != -1)
		kill(pid, SIGTERM);
	printf("%s", colors(0));
#endif

	print_result();
	return EXIT_SUCCESS;
}

#if EXPERIMENTAL_GRAPHICAL
int printfunc() {
	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = (180000000L);
	srand(time(NULL));
	int i = 0;
	printf("\n");
	fflush(stdout);
	while(1) {
		printf("%s%lc", colors(rand() % 7), 0x2588); // if locale failed, these will too
		nanosleep(&ts, NULL);
		fflush(stdout);
		i++;
		if (i==35){
			printf("%lc\n",0x2588);
			nanosleep(&ts, NULL);
			fflush(stdout);
			i = 0;
	}
	}
}
#endif

int find(uint32_t key, wchar_t* sana) {

	int ret;
	uint32_t index = key;

	while (hashArray[index] != NULL) {
		if (!(wcscmp(hashArray[index]->word, sana))) {
			hashArray[index]->occurences++;
			//printf("Found new %s (%d)\n",hashArray[index]->word, hashArray[index]->occurences);
			return FOUND;
		}
	++index;
	index %= SIZE; //wrap around as needed
	}
	add(index, key, sana); // Word not yet occurred, add it
	return OK;
}

int add(uint32_t index, uint32_t key, wchar_t* sana) {

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
	free(hashArray[index]);
	hashArray[index] = NULL;
	datapoints--;
}

uint32_t hash(wchar_t* sana) {
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

wchar_t* get_word() {

	wchar_t character;
	wchar_t charray[2];
	wchar_t* ret_str = calloc(WORD_MAX_LEN,sizeof(wchar_t));
	int ret, ok = 0;
	while (ok == 0) {
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
		if (character == WEOF)
			return NULL;
		else
			continue;
	}
#if DEBUG
	printf("Current word %ls\n", ret_str);
#endif
	wordcount++;
	return ret_str;
}
int init_file(int argc, char** argv) {
	if (argc <= 1) {
		printf("USAGE: %s [text file as argument]\n", argv[0]);
		return NOK;
	}
	if ((document = fopen(argv[1], "r")) == NULL) {
		printf("Please check filename !\n");
		return NOK;
	}
#if GRAPHICAL
	fseek(document, 0, SEEK_END);
	filelen_block = ftell(document) / 36L;
	filelen_block_size = filelen_block;
#endif
	rewind(document);
	return OK;
}

int print_result() {
	const int MAX_PRINT = maximum_print_size(datapoints);
	printf("\nFOUND %d DIFFERENT WORDS:\n", wordcount);
	printf("[INDEX]  [HITS]  [STRING]\n");
	for (uint32_t index = 0; index < MAX_PRINT; index++) {
		if(hashArray[index] != NULL)
			printf("%-8d %-7d %ls\n", index,hashArray[index]->occurences, hashArray[index]->word);
	} // formatting print does not seem to work too well with finnish chars
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

int partition(int low_index, int high_index) {
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
