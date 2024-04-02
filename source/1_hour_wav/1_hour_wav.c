#define PRINT_HEADER 0

#include "../read_wav.h"
#include "read_args.c"
#include <string.h>



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
	printf("\n");


#define CALCULATE_BYTES_COUNT(prefix, default_dur) \
	uint32_t prefix##_bytes_count; \
	if (prefix != NULL) { \
		long double seconds = HHMMSS_to_seconds(prefix); \
		prefix##_bytes_count = seconds_to_bytes_count(seconds, 	&header_in); \
	} else { \
		prefix##_bytes_count = seconds_to_bytes_count(default_dur, &header_in); \
	}


	CALCULATE_BYTES_COUNT(start_rep, 0);
	CALCULATE_BYTES_COUNT(end_rep, -1);

	const uintptr_t ptr_start = (uintptr_t)calc_header_size(&header_in);
	const uintptr_t ptr_start_rep = (uintptr_t)start_rep_bytes_count + calc_header_size(&header_in);
	const uintptr_t ptr_end_rep = (uintptr_t)end_rep_bytes_count + calc_header_size(&header_in);
	if (ptr_end_rep < ptr_start_rep){
		printf("Конец повтора раньше начала. Увеличьте параметр -end или уменьшите параметр -st.\n");
		return -1;
	}
	const uint32_t rep_dlit_bytes_count = end_rep_bytes_count -  start_rep_bytes_count;


	CALCULATE_BYTES_COUNT(mix_dlit, 0);

	if (rep_dlit_bytes_count < mix_dlit_bytes_count){
		perror("Буферная зона слишком велика. Уменьшите параметр -md.\n");
		return -1;
	}
	uint32_t rep_dlit_without_mix_bytes_count;
	if (rep_dlit_bytes_count < 2*mix_dlit_bytes_count)
		rep_dlit_without_mix_bytes_count = 0;
	else
		rep_dlit_without_mix_bytes_count = rep_dlit_bytes_count - 2*mix_dlit_bytes_count;
	
	
	CALCULATE_BYTES_COUNT(expected_dur, 3600);
	if (expected_dur_bytes_count < header_in.chunkSize - calc_header_size(&header_in) ){
		perror("Ожидаемая длительность меньше длительности входного файла. Увеличьте параметр -ed.\n");
		return -1;
	}




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
		float time = bytes_count_to_seconds(result_bytes, &header_in);
		printf("duration = %f seconds;\n", time);
		
		
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
		uint8_t *start_buffer, *end_buffer, *mix_buffer;

		start_buffer = (uint8_t *)malloc(mix_dlit_bytes_count);
		fseek(inputFile,  ptr_start_rep , SEEK_SET);
		fread(start_buffer, sizeof(uint8_t), mix_dlit_bytes_count, inputFile);

		buffer = (uint8_t *)malloc(rep_dlit_without_mix_bytes_count);
		fseek(inputFile,  ptr_start_rep + mix_dlit_bytes_count , SEEK_SET);
		fread(buffer, sizeof(uint8_t), rep_dlit_without_mix_bytes_count,  inputFile);
		
		end_buffer = (uint8_t *)malloc(mix_dlit_bytes_count);
		fseek(inputFile,  ptr_end_rep - mix_dlit_bytes_count , SEEK_SET);
		fread(end_buffer, sizeof(uint8_t), mix_dlit_bytes_count, inputFile);
		
		mix_buffer = (uint8_t *)malloc(mix_dlit_bytes_count);


		float progress;
		int volume_i;
		
		if (header_in.bitsPerSample == 8){

		int8_t start_num, end_num, result;
		for (unsigned int i = 0; i < mix_dlit_bytes_count; ++i) {

			start_num = start_buffer[i];
			end_num   = end_buffer[i];
			
			
			progress = (float)i / mix_dlit_bytes_count;
			volume_i = (progress * 256 + 0.5);
			start_num = ((int64_t)start_num * volume_i + 128) >> 8;

			progress = 1 - (float)i / mix_dlit_bytes_count;
			volume_i = (progress * 256 + 0.5);
			end_num = ((int64_t)end_num * volume_i + 128) >> 8;


			result = (start_num + end_num) / 2;
			mix_buffer[i] = result;
		}
		} else if (header_in.bitsPerSample == 16){

		int16_t start_num=0, end_num=0, result;
		
		for (uint32_t i = 0; i < mix_dlit_bytes_count; i+=2) {
			start_num = ((int16_t)start_buffer[i+1] << 8) | start_buffer[i];
			end_num   = ((int16_t)end_buffer[i+1]   << 8) | end_buffer[i];
			
			
			progress = (float)i / mix_dlit_bytes_count;
			volume_i = (progress * 256 + 0.5);
			start_num = ((int64_t)start_num * volume_i + 128) >> 8;

			progress = 1.1 - (float)i / mix_dlit_bytes_count;
			volume_i = (progress * 256 + 0.5);
			end_num = ((int64_t)end_num * volume_i + 128) >> 8;
			

			result = ((int64_t)start_num + (int64_t)end_num);
			mix_buffer[i+1]   = result >> 8;
			mix_buffer[i] = result;

		}
		} else{
			printf("Не поддерживаемая разрядность семплов: %d (bitsPerSample).\n", header_in.bitsPerSample);
			return -1;
		}


		// запись повтора
		fwrite(start_buffer, 1, mix_dlit_bytes_count, outputFile);
		free(start_buffer);

		for (int i=0; i < counts; ++i){
			fwrite(buffer, 1, rep_dlit_without_mix_bytes_count, outputFile);
			fwrite(mix_buffer, 1, mix_dlit_bytes_count, outputFile);
		}
		free(buffer);
		free(mix_buffer);
		
		fwrite(end_buffer, 1, mix_dlit_bytes_count, outputFile);
		free(end_buffer);


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

	char *in = NULL, *out = NULL, *start_rep = NULL, *end_rep = NULL, *mix_dlit = NULL, *expected_dur = NULL;
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

