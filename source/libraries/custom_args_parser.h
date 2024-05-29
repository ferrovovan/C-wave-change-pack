#include <stdio.h>


void print_usage(char *program_name){
	/* input, start repetition, end repetition, output
	*/
	printf("Использование: %s -i <входной_файл> -st <начало обрезки> -end <конец обрезки> -o <выходной_файл>  [опциональные параметры]\n", program_name);
	printf("Опциональные параметры:\n--test\t - предполагаемый результат\n");
}



int main(int argc, char *argv[]) {

#pragma GCC diagnostic ignored "-Wunused-but-set-parameter"
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
#pragma GCC diagnostic ignored "-Wint-conversion"
#pragma GCC diagnostic push

	char *in = NULL, *out = NULL, *start_time = NULL, *end_time = NULL;
	short test=0;

	#define CASE_ARG(ptr, flag, flag_len) \
	if (strncmp(argv[arg_num], flag, flag_len) == 0) { \
		arg_num++; \
		if (arg_num < argc){ \
			ptr = argv[arg_num]; \
		} \
		else \
			printf("Неверное использование параметра %s\n", flag); \
	}
	
#pragma GCC diagnostic pop
	
	for (int arg_num=1; arg_num < argc; ++arg_num){
		CASE_ARG(in, "-i", 2)
		else CASE_ARG(start_time, "-st", 3)
		else CASE_ARG(end_time, "-end", 4)
		else CASE_ARG(out, "-o", 2)
		else if (strncmp(argv[arg_num], "--test", 6) == 0 ){
			test = 1;
		} else {
			
			print_usage(argv[0]);
			return -1;
		}
	}
	
	// Валидация считанных данных
	if (in == NULL){
		printf("Не указан входной файл.\n-i <входной_файл>\n");
		return -2;
	} else if (test != 1 && out == NULL){
		printf("Не указан выходной файл.\n-o <выходной_файл>\n");
		return -3;
	}
	

	FILE *inputFile = fopen(in, "rb");
	if (inputFile == NULL) {
		perror("Ошибка открытия входного файла");
		return -1;
	}

	FILE *outputFile;
	if (test == 0){
		outputFile = fopen(out, "wb");
		if (outputFile == NULL) {
			perror("Ошибка открытия выходного файла");
			fclose(inputFile);
			return -1;
		}
	}
	// Передача аргументов в функцию
	cut_wav_file(inputFile, outputFile, start_time, end_time, test);
	return 0;
}
