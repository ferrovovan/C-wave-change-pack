#include "read_wav.h"


void wav_info(char* filename){
	FILE *inputFile;

	// Открытие первого входного файла для чтения
	inputFile = fopen(filename, "rb");
	if (inputFile == NULL) {
		perror("Ошибка открытия входного файла");
	}

	// Чтение заголовка первого файла
	WavHeader header;
	readWavHeader(inputFile, &header);
	printf("\nheader\n");
	printWavHeader(&header);

	fclose(inputFile);
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Использование: %s <входной_файл>\n", argv[0]);
		return 1;
	}

	// Передача аргументов в функцию
	wav_info(argv[1]);
	return 0;
}

