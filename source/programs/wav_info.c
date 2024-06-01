#include "read_wav.h"


int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Использование: %s <входной_файл>\n", argv[0]);
		return 1;
	}
	
	FILE *inputFile = fopen(argv[1], "rb");
	if (inputFile == NULL) {
		perror("Ошибка открытия входного файла");
	}
	
	WavHeader header;
	readWavHeader(inputFile, &header);
	printf("\nheader\n");
	printWavHeader(&header);

	fclose(inputFile);

	return 0;
}

