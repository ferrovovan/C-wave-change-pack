#define PRINT_HEADER 0

// arg parser including
#define REQ_ARG_INPUT
#define REQ_ARG_OUTPUT

#define ARG_START_TIME    0x04
#define ARG_END_TIME      0x05
#define ARG_MD_TIME      0x06
#define ARG_ED_TIME      0x07

#define ARG_TEST_FLAG     0x10

#include "args_parser.h"
// including end


#include "read_wav.h"
#include <string.h>

uint16_t uint8_to_uint16(uint8_t val1, uint8_t val2) {
	return ( (uint16_t)val1 << 8) | val2;
}


int main(int argc, char *argv[]) {
	if (parse_arguments(argc, argv) != 0) {
		return EXIT_FAILURE;
	}


// files opening
	FILE *inputFile = fopen(input_file, "rb");
	if (inputFile == NULL) {
		perror("Ошибка открытия входного файла");
		return -1;
	}
	FILE *outputFile;
	if (test_flag == 0){
		outputFile = fopen(output_file, "wb");
		if (outputFile == NULL) {
			perror("Ошибка открытия выходного файла");
			fclose(inputFile);
			return -1;
		}
	}



// BASE
	WavHeader header_in;
	readWavHeader(inputFile, &header_in);
	if (PRINT_HEADER){
		printf("\nfirst header:\n");
		printWavHeader(&header_in);
	}

// calculations block

#define CALCULATE_BYTES_COUNT(prefix, default_dur) \
	uint32_t prefix##_bytes_count; \
	if (prefix != NULL) { \
		long double seconds = HHMMSS_to_seconds(prefix); \
		prefix##_bytes_count = seconds_to_bytes_count(seconds, 	&header_in); \
	} else { \
		prefix##_bytes_count = seconds_to_bytes_count(default_dur, &header_in); \
	}

	printf("\n");
	CALCULATE_BYTES_COUNT(start_time, 0);
	CALCULATE_BYTES_COUNT(end_time, -1);

	const uintptr_t ptr_start = (uintptr_t)calc_header_size(&header_in);
	const uintptr_t ptr_start_rep = (uintptr_t)start_time_bytes_count + calc_header_size(&header_in);
	const uintptr_t ptr_end_rep = (uintptr_t)end_time_bytes_count + calc_header_size(&header_in);

	CALCULATE_BYTES_COUNT(mix_duration, 0);
	CALCULATE_BYTES_COUNT(expected_duration, 3600);
	if (expected_duration_bytes_count < header_in.chunkSize - calc_header_size(&header_in) ){
		perror("Ожидаемая длительность меньше длительности входного файла. Увеличьте параметр -ed.\n");
		expected_duration_bytes_count = header_in.chunkSize - calc_header_size(&header_in);
	}

	const uint32_t rep_duration_bytes_count = end_time_bytes_count -  start_time_bytes_count;
	if (rep_duration_bytes_count < mix_duration_bytes_count){
		perror("Буферная зона слишком велика. Уменьшите параметр -md.\n");
		return -1;
	}
	const uint32_t rep_duration_without_mix_bytes_count = rep_duration_bytes_count - mix_duration_bytes_count;

	

	const uint32_t end_bytes_count = (header_in.chunkSize - calc_header_size(&header_in) - end_time_bytes_count);

	const int counts   =   (expected_duration_bytes_count - start_time_bytes_count - end_bytes_count ) / rep_duration_bytes_count;
	const uint32_t result_bytes = start_time_bytes_count + end_bytes_count + counts * rep_duration_bytes_count;

	if (test_flag){
		fclose(inputFile);
		
		printf("Byte block:\n");
		printf("start_rep_bytes_count = %d;\n", start_time_bytes_count);
		printf("end_rep_bytes_count = %d;\n", end_time_bytes_count);
		printf("rep_dlit_bytes_count = %d;\n", rep_duration_bytes_count);
		
		printf("\nmix_dlit_bytes_count = %d;\n", mix_duration_bytes_count);
		printf("rep_duration_without_mix_bytes_count = %d;\n", rep_duration_without_mix_bytes_count);
		
		printf("\nend_bytes_count = %d;\n", end_bytes_count);
		printf("result_bytes = %d;\n", result_bytes);
		
		printf("\nTime block:\n");
		printf("counts = %d;\n", counts);
		
		printf("\nУспешная проверка параметров.\n");
		return 0;
	}
	else {
		// Создание нового заголовка для выходного файла
		WavHeader hourHeader;
		copy_WavHeader(&header_in, &hourHeader);
		hourHeader.chunkSize = result_bytes + 44;
		hourHeader.subchunk2Size = result_bytes;
		
		if (PRINT_HEADER){
			printf("\nhour Header\n");
			printWavHeader(&hourHeader);
		}
		fwrite(&hourHeader, sizeof(WavHeader), 1, outputFile);


		/* алгоритм */
		uint8_t *buffer;
		// start
		fseek(inputFile,  ptr_start , SEEK_SET);
		buffer = (uint8_t *)malloc(start_time_bytes_count);
		fread(buffer, start_time_bytes_count, sizeof(uint8_t), inputFile);
		fwrite(buffer, start_time_bytes_count, sizeof(uint8_t), outputFile);
		free(buffer);

		// povtor
		buffer = (uint8_t *)malloc(rep_duration_without_mix_bytes_count);
		fseek(inputFile,  ptr_start_rep, SEEK_SET);
		fread(buffer, rep_duration_without_mix_bytes_count, sizeof(uint8_t), inputFile);
		
		// формирование буфера
		uint8_t *mix_buffer = (uint8_t *)malloc(mix_duration_bytes_count);
		
		double first_num, second_num, progress;
	
		fseek(inputFile, ptr_end_rep - (uintptr_t)mix_duration_bytes_count, SEEK_SET);
		fread(mix_buffer, mix_duration_bytes_count, sizeof(uint8_t), inputFile);

		
		for (uint32_t i = 0; i < mix_duration_bytes_count; ++i) {
			progress = (double)i / mix_duration_bytes_count;
			first_num = (double)mix_buffer[i] * progress;
			second_num = (double)buffer[i] * (1 - progress);
			mix_buffer[i] = (uint8_t)((first_num + second_num) / 14.0f);
		}
		
		// запись повтора
		for (int i=0; i < counts; ++i){
			fwrite(buffer, rep_duration_without_mix_bytes_count, 1, outputFile); // база
			fwrite(mix_buffer, mix_duration_bytes_count, 1, outputFile); // склейка
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
	return EXIT_SUCCESS;
}

