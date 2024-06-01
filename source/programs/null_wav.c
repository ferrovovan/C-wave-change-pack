// args parser including
#define REQ_ARG_OUTPUT
#define REQ_ARG_ED_TIME

#include "args_parser.h"
//


#include "read_wav.h"
#include <stdint.h>

#define BUFF_SIZE 1024


void zero_wav_file(FILE *outputFile, double time){
    
	// Создание нового заголовка для выходного файла
	WavHeader outHeader;
	create_WavHeader_base(&outHeader, 2);
	uint32_t result_bytes = seconds_to_bytes_count(time, &outHeader);
	outHeader.chunkSize = result_bytes + 44;
	outHeader.subchunk2Size = result_bytes;

	// Запись нового заголовка в выходной файл
	fwrite(&outHeader, sizeof(WavHeader), 1, outputFile);    


	const uint8_t *buffer = (uint8_t *) calloc(BUFF_SIZE, sizeof(uint8_t) );
	const int counts = result_bytes / BUFF_SIZE;
	const int remainder = result_bytes % BUFF_SIZE;
	for (int i = 0; i < counts; ++i)
		fwrite(buffer, sizeof(uint8_t), BUFF_SIZE, outputFile);
	fwrite(buffer, sizeof(uint8_t), remainder, outputFile);

	printf("\nФайл успешно занулён.\n");
}



int main(int argc, char *argv[]) {
// Считывание
	if (parse_arguments(argc, argv) != 0) {
		return EXIT_FAILURE;
	}
	
// Открытие файлов
	FILE *outputFile = fopen(output_file, "wb");
	if (outputFile == NULL) {
		printf("Ошибка открытия выходного файла.\n");
		return EXIT_FAILURE;
	}

// Валидация
	double time = HHMMSS_to_seconds(expected_duration);
	if( time == -1 ){
		printf("Неверный формат %s\n", expected_duration);
		return EXIT_FAILURE;
	}

// Передача аргументов в функцию
	zero_wav_file(outputFile, time);

	fclose(outputFile);
	return EXIT_SUCCESS;
}
