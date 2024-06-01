/*
define before include this header:
example:
// arg parser including
#define REQ_ARG_INPUT
#define REQ_ARG_OUTPUT

#include "args_parser.h"
//

requared:
#define ARG_INPUT
OR
#define REQ_ARG_INPUT

#define ARG_OUTPUT
OR
#define REQ_ARG_OUTPUT

#define ARG_COUNT
OR
#define REQ_ARG_COUNT

#define ARG_ED_TIME
OR
#define REQ_ARG_ED_TIME


optional:
#define ARG_START_TIME    0x04
#define ARG_END_TIME      0x05
#define ARG_MD_TIME      0x06

#define ARG_TEST_FLAG     0x10
*/
/*
global variables:
	char *input_file;
	char *output_file;

	char *start_time;
	char *end_time;
	char *mix_duration;
	char *expected_duration;
	
	char *count;
	int test_flag;
*/


#include <stdio.h>
#include <getopt.h>




#ifdef REQ_ARG_INPUT
#define ARG_INPUT
#endif

#ifdef REQ_ARG_OUTPUT
#define ARG_OUTPUT
#endif

#ifdef REQ_ARG_COUNT
#define ARG_COUNT
#endif

#ifdef REQ_ARG_ED_TIME
#define ARG_ED_TIME
#endif

#ifdef ARG_INPUT
char *input_file = NULL;
#endif
#ifdef ARG_OUTPUT
char *output_file = NULL;
#endif
#ifdef ARG_START_TIME
char *start_time = NULL;
#endif
#ifdef ARG_END_TIME
char *end_time = NULL;
#endif
#ifdef ARG_MD_TIME
char *mix_duration = NULL;
#endif
#ifdef ARG_ED_TIME
char *expected_duration = NULL;
#endif

#ifdef ARG_COUNT
char *count = NULL;
#endif

#ifdef ARG_TEST_FLAG
int test_flag = 0;
#endif

// Функция для печати использования
void print_usage(){
	printf("\nПараметры:\n");
	#ifdef ARG_INPUT
		printf(" [ -i | --input ] <входной_файл>  \n");
	#endif
	#ifdef ARG_START_TIME
		printf(" [ -s | --start ] <начало> \n");
	#endif
	#ifdef ARG_END_TIME
		printf(" [ -e | --end ]   <конец> \n");
	#endif
	#ifdef ARG_MD_TIME
		printf(" [ -m | --mix_duration ] <длительность наложения> \n");
	#endif
	#ifdef ARG_ED_TIME
		printf(" [ -d | --expected_duration ] <ожидаемая длительность> \n");
	#endif
	#ifdef ARG_OUTPUT
		printf(" [ -o | --output ] <выходной_файл> \n");
	#endif

	#ifdef ARG_COUNT
		printf(" [ -c | --count ] <повторения> \n");
	#endif
	#ifdef ARG_TEST_FLAG
		printf(" --test   вывод статистики предполагаемого результата \n");
	#endif
	#if defined(ARG_START_TIME) || \
	    defined(ARG_END_TIME) || \
	    defined(ARG_MD_TIME) || \
	    defined(ARG_ED_TIME)
		printf("\nФормат времени:\n");
		printf("HH:MM:SS.MS\n");
		printf("HH - часы (опционально)\n");
		printf("MM - минуты (опционально)\n");
		printf("SS - секунды (обязательно)\n");
		printf("MS - миллисекунды (опционально)\n");
	#endif
	printf("\n");
}

// Функция для разбора аргументов командной строки
int parse_arguments(int argc, char *argv[]) {
	int opt;

	// Определение длинных опций
	static struct option long_options[] = {
		#ifdef ARG_INPUT
		{"input", required_argument, NULL, 'i'},
		#endif
		#ifdef ARG_OUTPUT
		{"output", required_argument, NULL, 'o'},
		#endif
		#ifdef ARG_START_TIME
		{"start", required_argument, NULL, 's'},
		#endif
		#ifdef ARG_END_TIME
		{"end", required_argument, NULL, 'e'},
		#endif
		#ifdef ARG_MD_TIME
		{"mix_duration", required_argument, NULL, 'm'},
		#endif
		#ifdef ARG_ED_TIME
		{"expected_duration", required_argument, NULL, 'd'},
		#endif

		#ifdef ARG_COUNT
		{"count", required_argument, NULL, 'c'},
		#endif

		#ifdef ARG_TEST_FLAG
		{"test", no_argument, NULL, 't'},
		#endif
		{NULL, 0, NULL, 0}
	};

	while ((opt = getopt_long(argc, argv, "i:o:s:e:m:d:t", long_options, NULL)) != -1) {
		switch (opt) {
		#ifdef ARG_INPUT
			case 'i':
				input_file = optarg;
				break;
		#endif
		#ifdef ARG_OUTPUT
			case 'o':
				output_file = optarg;
				break;
		#endif
		#ifdef ARG_START_TIME
			case 's':
				start_time = optarg;
				break;
		#endif
		#ifdef ARG_END_TIME
			case 'e':
				end_time = optarg;
				break;
		#endif
		#ifdef ARG_MD_TIME
			case 'm':
				mix_duration = optarg;
				break;
		#endif
		#ifdef ARG_ED_TIME
			case 'd':
				expected_duration = optarg;
				break;
		#endif
		#ifdef ARG_TEST_FLAG
			case 't':
				test_flag = 1;
				break;
		#endif
		#ifdef ARG_COUNT
			case 'c':
				count = optarg;
				break;
		#endif
			case '?':
				fprintf(stderr, "Unknown option or missing argument: '-%c'\n", optopt);
				/* fall through */
			default:
				print_usage(argv[0]);
				return -1;
		}
	}


	#ifdef REQ_ARG_INPUT
	if (input_file == NULL) {
		fprintf(stderr, \
		"Ошибка: обязательный параметр -i <входной_файл> отсутствует.\n");
		print_usage(argv[0]);
		return -1;
	}
	#endif
	#ifdef REQ_ARG_OUTPUT
	if (output_file == NULL) {
		fprintf(stderr, \
		"Ошибка: обязательный параметр -o <выходной_файл> отсутствует.\n");
		print_usage(argv[0]);
		return -1;
	}
	#endif
	#ifdef REQ_ARG_COUNT
	if (count == NULL) {
		fprintf(stderr, \
		"Ошибка: обязательный параметр -c <повторения> отсутствует.\n");
		print_usage(argv[0]);
		return -1;
	}
	#endif

	return 0;
}

/*
EXAMPLE
int main(int argc, char *argv[]) {
	if (parse_arguments(argc, argv) != 0) {
		return EXIT_FAILURE;
	}

	// main code

	return EXIT_SUCCESS;
}
*/
