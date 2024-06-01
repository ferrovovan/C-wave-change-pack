// args parser including
#define REQ_ARG_INPUT
#define REQ_ARG_OUTPUT

#define ARG_START_TIME
#define ARG_END_TIME

#define ARG_TEST_FLAG

#include "args_parser.h"
// including end


#include "read_wav.h"


int cut_wav_file(FILE *inputFile, FILE *outputFile, double start_time, double end_time, short info){

	WavHeader in_header;
	readWavHeader(inputFile, &in_header);


	uint32_t start_time_bytes_count = seconds_to_bytes_count(start_time, &in_header);
	const uintptr_t ptr_start_cut = (uintptr_t)start_time_bytes_count + calc_header_size(&in_header);
	uint32_t end_time_bytes_count = seconds_to_bytes_count(end_time, &in_header);
	const uintptr_t ptr_end_cut = (uintptr_t)end_time_bytes_count + calc_header_size(&in_header);


	if (ptr_end_cut <= ptr_start_cut){
		perror("Конец меньше начала.\n");
		return -1;
	}

	const uint32_t cut_dlit = ptr_end_cut - ptr_start_cut;
	

	if (info){
		printf("\n");
		printf("\nfirst header:\n");
		printWavHeader(&in_header);

		printf("\n-----------\n");
		printf("Byte block:\n");
		printf("start_time_bytes_count = %d;\n", start_time_bytes_count);
		printf("end_time_bytes_count = %d;\n", end_time_bytes_count);


		printf("\nTime block:\n");
		float time = bytes_count_to_seconds(cut_dlit, &in_header);
		printf("duration = %f seconds;\n", time);
		
		return 0;
	} else {
		// Создание нового заголовка для выходного файла
		WavHeader cutHeader;
		copy_WavHeader(&in_header, &cutHeader);
		cutHeader.chunkSize = cut_dlit + 44;
		cutHeader.subchunk2Size = cut_dlit;
		fwrite(&cutHeader, sizeof(cutHeader), 1, outputFile);


		/* алгоритм */
		uint8_t *buffer = (uint8_t *)malloc(cut_dlit);

		fseek(inputFile,  ptr_start_cut , SEEK_SET);
		fread(buffer, cut_dlit, sizeof(uint8_t), inputFile);
		fwrite(buffer, cut_dlit, sizeof(uint8_t), outputFile);

		free(buffer);

		// Закрытие файлов
		
		printf("\nУспешная запись.\n");
		return 0;
	}
}



int main(int argc, char *argv[]) {
// Считывание
	if (parse_arguments(argc, argv) != 0) {
		return EXIT_FAILURE;
	}
	
// Открытие файлов
	FILE *inputFile = fopen(input_file, "rb");
	if (inputFile == NULL) {
		printf("Ошибка открытия входного файла.\n");
		return EXIT_FAILURE;
	}

	FILE *outputFile = fopen(output_file, "wb");
	if (outputFile == NULL) {
		printf("Ошибка открытия выходного файла.\n");
		fclose(inputFile);
		return EXIT_FAILURE;
	}

// Валидация
#define READ_VALUE(prefix, default_dur) \
	if (prefix != NULL){ \
		prefix##_double = HHMMSS_to_seconds(prefix); \
		if (prefix##_double == -1){ \
			printf("Неверный формат %s\n", prefix); \
			return EXIT_FAILURE; \
		} \
	} else { \
		prefix##_double = default_dur; \
	}

	double start_time_double, end_time_double;
	READ_VALUE(start_time, 0)
	READ_VALUE(end_time, -1)

// Передача аргументов в функцию
	cut_wav_file(inputFile, outputFile, start_time_double, end_time_double, (short )test_flag);

	fclose(inputFile);
	fclose(outputFile);

	return EXIT_SUCCESS;
}

