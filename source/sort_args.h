/*
#define  INPUT_OPTION
#define  START_OPTION
#define    END_OPTION
#define     MD_OPTION
#define     ED_OPTION
#define OUTPUT_OPTION
*/

typedef struct{
	char *key;
	int key_len;
	int value;
} Dict_elem;

#include <stdio.h>
#include <string.h>


void print_usage(char *program_name){
	printf("Использование: %s", program_name);
	#ifdef INPUT_OPTION
		printf(" -i <входной_файл> "); // input file
	#endif
	#ifdef START_OPTION
		printf(" -st <начало повтора> "); // start repetition
	#endif
	#ifdef END_OPTION
		printf(" -end <конец повтора> "); // end repetition
	#ifdef MD_OPTION
		printf(" -md <длительность наложения> ");  // mix duration
	#endif
	#ifdef ED_OPTION
		printf(" -ed <ожидаемая длительность> "); // expected duration
	#endif
	#ifdef OUTPUT_OPTION
		printf(" -o <выходной_файл> "); // output file
	#endif
	printf(" [опциональные параметры]\n");

	printf("Опциональные параметры:\n--test\t - предполагаемый результат\n");
	printf("--test_rep\t - склейка в повторе\n");
}

int sort_args(int argc, char *argv[],
	int *ptr_massive, Dict_elem *table, int table_len,
	short *test, short *test_rep){

	for (int arg_num=1; arg_num < argc; ++arg_num){
		for (int i=0; i < table; ++i){
			if( strncmp(table[i].key, argv[arg_num], table[i].key_len) ){
				ptr_massive[ table[i].value] = ++arg_num;
			}
		}
		else{
			printf("неизвестный параметр %s\n", argv[arg_num]);
			print_usage(argv[0]);
			return -1;
		}
	}	
	return 0;
}
