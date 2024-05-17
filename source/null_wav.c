#include "read_wav.h"
#include <stdint.h>

#define BUFF_SIZE 1024


void zero_wav_file(char* out, double time){
	// Открытие выходного файла для записи
	FILE *outputFile = fopen(out, "wb");
	if (outputFile == NULL)
		perror("Ошибка открытия выходного файла");
    
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

	fclose(outputFile);
	printf("\nФайл успешно занулён.\n");
}


int main(int argc, char *argv[]) {
	if (argc != 3) {
		printf("Использование: %s <выходной_файл> <время в секундах>\n", argv[0]);
		return 1;
	}
	double time = HHMMSS_to_seconds(argv[2]);
	zero_wav_file(argv[1], time);
	return 0;
}
