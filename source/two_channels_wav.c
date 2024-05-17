#define BUFF_SIZE 1024
#define PRINT_HEADER 0

#include "read_wav.h"


void concat_wav_files(char* in, char* out){
	FILE *inputFile, *outputFile;

	// Открытие первого входного файла для чтения
	inputFile = fopen(in, "rb");
	if (inputFile == NULL) {
		perror("Ошибка открытия первого входного файла");
	}

	// Открытие выходного файла для записи
	outputFile = fopen(out, "wb");
	if (outputFile == NULL) {
		perror("Ошибка открытия выходного файла");
		fclose(inputFile);
	}

	// Чтение заголовка первого файла
	WavHeader in_header;
	readWavHeader(inputFile, &in_header);
	if (PRINT_HEADER){
		printf("\nfirst header:\n");
		printWavHeader(&in_header);
	}

	// Создание нового заголовка для выходного файла
	WavHeader outHeader;
	create_WavHeader_base(&outHeader, 2);
	uint32_t result_bytes = (in_header.chunkSize - calc_header_size(&in_header)) * 2;
	outHeader.chunkSize = result_bytes + 44;
	outHeader.subchunk2Size = result_bytes;
	fwrite(&outHeader, sizeof(WavHeader), 1, outputFile);    

	// Копирование данных из второго файла в конец первого
	uint8_t buffer[BUFF_SIZE];
	uint8_t buffer2[2* BUFF_SIZE];
	size_t bytesRead;


	while ((bytesRead = fread(buffer, 1, sizeof(buffer), inputFile)) > 0){
		for(size_t i=0; i< bytesRead; ++i){
			buffer2[i*2]     = buffer[i];
			buffer2[i*2 + 1] = buffer[i];
		}
		fwrite(buffer2, 1, 2*bytesRead, outputFile);
	}

	fclose(inputFile);
	fclose(outputFile);
	printf("\nФайлы успешно объединены.\n");
}

int main(int argc, char *argv[]) {
	if (argc != 3) {
		printf("Использование: %s <входной_файл> <выходной_файл>\n", argv[0]);
		return 1;
	}

	// Передача аргументов в функцию
	concat_wav_files(argv[1], argv[2]);
	return 0;
}

