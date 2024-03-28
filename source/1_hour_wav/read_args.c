#include <string.h>


void print_usage(char *program_name){
	/* input, start repetition, end repetition, mix duration, output
	*/
	printf("Использование: %s -i <входной_файл> -sr <начало повтора> -er <конец повтора> -md <длительность наложения> -ed <ожидаемая длительность> -o <выходной_файл>  [опциональные параметры]\n", program_name);
	printf("Опциональные параметры:\n--test\t - предполагаемый результат\n");
}

#pragma GCC diagnostic ignored "-Wunused-but-set-parameter"
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
#pragma GCC diagnostic ignored "-Wint-conversion"
#pragma GCC diagnostic push
int read_params(int argc, char *argv[],
	char **in, char **out, char **start_rep, char **end_rep, char **mix_dlit, char **expected_dur, short *test){


	#define CASE_ARG(ptr, flag, flag_len) \
	if (strncmp(argv[arg_num], flag, flag_len) == 0) { \
		arg_num++; \
		if (arg_num < argc){ \
			*ptr = argv[arg_num]; \
		} \
		else \
			printf("Неверное использование параметра %s\n", flag); \
	}
	
#pragma GCC diagnostic pop
	
	
	for (int arg_num=1; arg_num < argc; ++arg_num){
		CASE_ARG(in, "-i", 2)
		else CASE_ARG(start_rep, "-sr", 3)
		else CASE_ARG(end_rep, "-er", 3)
		else CASE_ARG(mix_dlit, "-md", 3)
		else CASE_ARG(expected_dur, "-ed", 3)
		else CASE_ARG(out, "-o", 2)
		else if (strncmp(argv[arg_num], "--test", 6) == 0 ){
			*test = 1;
		} else {
			
			print_usage(argv[0]);
			return -1;
		}
	}
	
	// Валидация считанных данных
	if (*in == NULL){
		printf("Не указан входной файл.\n-i <входной_файл>\n");
		return -2;
	} else if (*test != 1 && *out == NULL){
		printf("Не указан выходной файл.\n-o <выходной_файл>\n");
		return -3;
	}
	return 0;
}

