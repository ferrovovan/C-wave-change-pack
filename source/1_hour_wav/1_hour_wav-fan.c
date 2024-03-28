#define PRINT_HEADER 0

#include "../read_wav.h"
#include "read_args.c"
#include <string.h>

uint16_t uint8_to_uint16(uint8_t val1, uint8_t val2) {
	return ( (uint16_t)val1 << 8) | val2;
}

WavHeader create_HourHeader(WavHeader *header_in, uint32_t result_bytes) {
	WavHeader hourHeader = *header_in;  // Копируем все поля из входного заголовка

	hourHeader.chunkSize = result_bytes + 44;

	#pragma GCC diagnostic ignored "-Wstringop-truncation"
	#pragma GCC diagnostic push
	strncpy(hourHeader.subchunk2Id, "data", 4);
	#pragma GCC diagnostic pop

	hourHeader.subchunk2Size = result_bytes;

	return hourHeader;
}

int hour_wav_file(FILE *inputFile, FILE *outputFile, char* start_rep, char* end_rep, char* mix_dlit, char* expected_dur, short info){

	WavHeader header_in;
	readWavHeader(inputFile, &header_in);
	if (PRINT_HEADER){
		printf("\nfirst header:\n");
		printWavHeader(&header_in);
	}


#define CALCULATE_BYTES_COUNT(prefix, default_dur) \
	uint32_t prefix##_bytes_count; \
	if (prefix != NULL) { \
		long double seconds = HHMMSS_to_seconds(prefix); \
		prefix##_bytes_count = seconds_to_bytes_count(seconds, 	&header_in); \
	} else { \
		prefix##_bytes_count = seconds_to_bytes_count(default_dur, &header_in); \
	}

	printf("\n");
	CALCULATE_BYTES_COUNT(start_rep, 0);
	CALCULATE_BYTES_COUNT(end_rep, -1);

	const uintptr_t ptr_start = (uintptr_t)calc_header_size(&header_in);
	const uintptr_t ptr_start_rep = (uintptr_t)start_rep_bytes_count + calc_header_size(&header_in);
	const uintptr_t ptr_end_rep = (uintptr_t)end_rep_bytes_count + calc_header_size(&header_in);

	CALCULATE_BYTES_COUNT(mix_dlit, 0);
	CALCULATE_BYTES_COUNT(expected_dur, 3600);
	if (expected_dur_bytes_count < header_in.chunkSize - calc_header_size(&header_in) ){
		perror("Ожидаемая длительность меньше длительности входного файла. Увеличьте параметр -ed.\n");
		expected_dur_bytes_count = header_in.chunkSize - calc_header_size(&header_in);
	}

	const uint32_t rep_dlit_bytes_count = end_rep_bytes_count -  start_rep_bytes_count;
	if (rep_dlit_bytes_count < mix_dlit_bytes_count){
		perror("Буферная зона слишком велика. Уменьшите параметр -md.\n");
		return -1;
	}
	const uint32_t rep_dlit_without_mix_bytes_count = rep_dlit_bytes_count - mix_dlit_bytes_count;

	

	const uint32_t end_bytes_count = (header_in.chunkSize - calc_header_size(&header_in) - end_rep_bytes_count);

	const int counts   =   (expected_dur_bytes_count - start_rep_bytes_count - end_bytes_count ) / rep_dlit_bytes_count;
	const uint32_t result_bytes = start_rep_bytes_count + end_bytes_count + counts * rep_dlit_bytes_count;

	if (info){
		fclose(inputFile);
		
		printf("Byte block:\n");
		printf("start_rep_bytes_count = %d;\n", start_rep_bytes_count);
		printf("end_rep_bytes_count = %d;\n", end_rep_bytes_count);
		printf("rep_dlit_bytes_count = %d;\n", rep_dlit_bytes_count);
		
		printf("\nmix_dlit_bytes_count = %d;\n", mix_dlit_bytes_count);
		printf("rep_dlit_without_mix_bytes_count = %d;\n", rep_dlit_without_mix_bytes_count);
		
		printf("\nend_bytes_count = %d;\n", end_bytes_count);
		printf("result_bytes = %d;\n", result_bytes);
		
		printf("\nTime block:\n");
		printf("counts = %d;\n", counts);
		
		printf("\nУспешная проверка параметров.\n");
		return 0;
	} else {
		// Создание нового заголовка для выходного файла
		WavHeader hourHeader = create_HourHeader(&header_in, result_bytes);
		if (PRINT_HEADER){
			printf("\nhour Header\n");
			printWavHeader(&hourHeader);
		}
	
		
		// Запись нового заголовка в выходной файл
		fwrite(&hourHeader, sizeof(WavHeader), 1, outputFile);


		/* алгоритм */
		uint8_t *buffer;
		// start
		fseek(inputFile,  ptr_start , SEEK_SET);
		buffer = (uint8_t *)malloc(start_rep_bytes_count);
		fread(buffer, start_rep_bytes_count, sizeof(uint8_t), inputFile);
		fwrite(buffer, start_rep_bytes_count, sizeof(uint8_t), outputFile);
		free(buffer);

		// povtor
		buffer = (uint8_t *)malloc(rep_dlit_without_mix_bytes_count);
		fseek(inputFile,  ptr_start_rep, SEEK_SET);
		fread(buffer, rep_dlit_without_mix_bytes_count, sizeof(uint8_t), inputFile);
		
		// формирование буфера
		uint8_t *mix_buffer = (uint8_t *)malloc(mix_dlit_bytes_count);
		
		double first_num, second_num, progress;
	
		fseek(inputFile, ptr_end_rep - (uintptr_t)mix_dlit_bytes_count, SEEK_SET);
		fread(mix_buffer, mix_dlit_bytes_count, sizeof(uint8_t), inputFile);

		
		for (uint32_t i = 0; i < mix_dlit_bytes_count; ++i) {
			progress = (double)i / mix_dlit_bytes_count;
			first_num = (double)mix_buffer[i] * progress;
			second_num = (double)buffer[i] * (1 - progress);
			mix_buffer[i] = (uint8_t)((first_num + second_num) / 14.0f);
		}
		
		// запись повтора
		for (int i=0; i < counts; ++i){
			fwrite(buffer, rep_dlit_without_mix_bytes_count, 1, outputFile); // база
			fwrite(mix_buffer, mix_dlit_bytes_count, 1, outputFile); // склейка
		}
		free(mix_buffer);
		free(buffer);

		// end
		fseek(inputFile,  ptr_end_rep , SEEK_SET);
		buffer = (uint8_t *)malloc(end_bytes_count);
		fread(buffer, end_bytes_count, sizeof(uint8_t), inputFile);
		fwrite(buffer, end_bytes_count, sizeof(uint8_t), outputFile);
		free(buffer);


		// Закрытие файлов
		fclose(inputFile);
		fclose(outputFile);
		printf("\nУспешная запись.\n");
		return 0;
	}	
}


int main(int argc, char *argv[]) {

	char *in=NULL, *start_rep=NULL, *end_rep=NULL, *mix_dlit=NULL, *expected_dur=NULL, *out=NULL;
	short test=0;

	int code = read_params(argc, argv, &in, &out, &start_rep, &end_rep, &mix_dlit, &expected_dur, &test);
	
	if (code == 0){

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
		hour_wav_file(inputFile, outputFile, start_rep, end_rep, mix_dlit, expected_dur, test);
		return 0;
	} else {
		perror("Некорректное считывание параметров");
		return -1;
	}
}

