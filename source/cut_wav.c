#include "read_wav.h"
#include <string.h>


WavHeader create_CutHeader(WavHeader *header_in, uint32_t result_bytes) {
	WavHeader cutHeader = *header_in;  // Копируем все поля из входного заголовка

	cutHeader.chunkSize = result_bytes + 44;

	#pragma GCC diagnostic ignored "-Wstringop-truncation"
	#pragma GCC diagnostic push
	strncpy(cutHeader.subchunk2Id, "data", 4);
	#pragma GCC diagnostic pop

	cutHeader.subchunk2Size = result_bytes;

	return cutHeader;
}

int cut_wav_file(FILE *inputFile, FILE *outputFile, char* start_time, char* end_time, short info){

	WavHeader header_in;
	readWavHeader(inputFile, &header_in);


#define CALCULATE_BYTES_COUNT(prefix, default_dur) \
	uint32_t prefix##_bytes_count; \
	if (prefix != NULL) { \
		long double seconds = HHMMSS_to_seconds(prefix); \
		prefix##_bytes_count = seconds_to_bytes_count(seconds, 	&header_in); \
	} else { \
		prefix##_bytes_count = seconds_to_bytes_count(default_dur, &header_in); \
	}

	CALCULATE_BYTES_COUNT(start_time, 0);
	CALCULATE_BYTES_COUNT(end_time, -1);

	const uintptr_t ptr_start_cut = (uintptr_t)start_time_bytes_count + calc_header_size(&header_in);
	const uintptr_t ptr_end_cut = (uintptr_t)end_time_bytes_count + calc_header_size(&header_in);


	if (ptr_end_cut <= ptr_start_cut){
		perror("Конец меньше начала.\n");
		return -1;
	}
	const uint32_t cut_dlit = ptr_end_cut - ptr_start_cut;
	

	if (info){
		fclose(inputFile);
		printf("\n");
		printf("\nfirst header:\n");
		printWavHeader(&header_in);

		printf("Byte block:\n");
		printf("start_time_bytes_count = %d;\n", start_time_bytes_count);
		printf("end_time_bytes_count = %d;\n", end_time_bytes_count);


		printf("\nTime block:\n");
		float time = bytes_count_to_seconds(cut_dlit, &header_in);
		printf("duration = %f seconds;\n", time);
		
		return 0;
	} else {
		// Создание нового заголовка для выходного файла
		WavHeader cutHeader = create_CutHeader(&header_in, cut_dlit);
		
		// Запись нового заголовка в выходной файл
		fwrite(&cutHeader, sizeof(WavHeader), 1, outputFile);


		/* алгоритм */
		uint8_t *buffer = (uint8_t *)malloc(cut_dlit);

		fseek(inputFile,  ptr_start_cut , SEEK_SET);
		fread(buffer, cut_dlit, sizeof(uint8_t), inputFile);
		fwrite(buffer, cut_dlit, sizeof(uint8_t), outputFile);

		free(buffer);

		// Закрытие файлов
		fclose(inputFile);
		fclose(outputFile);
		printf("\nУспешная запись.\n");
		return 0;
	}
}



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
	//printf("%s\n\n", outputFile);
	// Передача аргументов в функцию
	cut_wav_file(inputFile, outputFile, start_time, end_time, test);
	return 0;
}

