/*
define before include this header:
#define ARG_INPUT    0x01
#define ARG_OUTPUT   0x02

#define ARG_START_TIME    0x04
#define ARG_END_TIME      0x05
#define ARG_MD_TIME      0x06
#define ARG_ED_TIME      0x07

#define ARG_TEST_FLAG     0x10
*/


#include <stdio.h>
#include <getopt.h>


#ifdef ARG_INPUT
char *input_file;
#endif
#ifdef ARG_OUTPUT
char *output_file;
#endif
#ifdef ARG_START_TIME
char *start_time;
#endif
#ifdef ARG_END_TIME
char *end_time;
#endif
#ifdef ARG_MD_TIME
char *md_time;
#endif
#ifdef ARG_ED_TIME
char *ed_time;
#endif
#ifdef ARG_TEST_FLAG
int test_flag;
#endif

// Функция для печати использования
void print_usage(char *program_name){
	printf("Использование: %s", program_name);
	#ifdef ARG_INPUT
		printf(" [ -i | --input ] <входной_файл> ");
	#endif
	#ifdef ARG_START_TIME
		printf(" [ -s | --start ] <начало повтора> ");
	#endif
	#ifdef ARG_END_TIME
		printf(" [ -e | --end ] <конец повтора> ");
	#endif
	#ifdef ARG_MD_TIME
		printf(" [ -m | --mix_duration ] <длительность наложения> ");
	#endif
	#ifdef ARG_ED_TIME
		printf(" [ -d | --expected_duration ] <ожидаемая длительность> ");
	#endif
	#ifdef ARG_OUTPUT
		printf(" [ -o | --output ] <выходной_файл> ");
	#endif
	#ifdef ARG_TEST_FLAG
		printf(" --test - вывод предполагаемый результат");
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
				md_time = optarg;
				break;
		#endif
		#ifdef ARG_ED_TIME
			case 'd':
				ed_time = optarg;
				break;
		#endif
		#ifdef ARG_TEST_FLAG
			case 't':
				test_flag = 1;
				break;
		#endif
			case '?':
				fprintf(stderr, "Unknown option or missing argument: '-%c'\n", optopt);
			default:
				print_usage(argv[0]);
				exit(EXIT_FAILURE);
				return -1;
		}
	}
	return 0;
}

int main(int argc, char *argv[]) {
	// Пример вызова функции разбора аргументов
	if (parse_arguments(argc, argv) != 0) {
		return EXIT_FAILURE;
	}

	// Ваш основной код здесь
	return EXIT_SUCCESS;
}

