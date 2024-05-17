#define BUFF_SIZE 1024
#define PRINT_HEADER 1

#include "read_wav.h"


WavHeader create_multiplicatedHeader(WavHeader *in_header, const int num) {
	WavHeader multiplicatedHeader = *in_header;  // Копируем все поля из первого заголовка

	// Изменяем Chunk Size на сумму Chunk Size из обоих заголовков
	uint32_t samples_size	= in_header->chunkSize - calc_header_size(in_header);
	samples_size = samples_size * num;

	multiplicatedHeader.chunkSize = samples_size + 44;

	strcpy(multiplicatedHeader.subchunk2Id, "data");
	multiplicatedHeader.subchunk2Size = samples_size;

	return multiplicatedHeader;
}

void mult_wave_file(const char* in, const char* out, int cycles){
	FILE *inputFile, *outputFile;

	// Открытие первого входного файла для чтения
	inputFile = fopen(in, "rb");
	if (inputFile == NULL) {
		perror("Ошибка открытия входного файла");
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
		printf("\nIn header:\n");
		printWavHeader(&in_header);
	}

    
	// Создание нового заголовка для выходного файла
	WavHeader out_header = create_multiplicatedHeader(&in_header, cycles);
	if (PRINT_HEADER){
		printf("\nOut header:\n");
		printWavHeader(&out_header);
	}
    
	// Запись нового заголовка в выходной файл
	fwrite(&out_header, sizeof(out_header), 1, outputFile);    

	uint8_t buffer[BUFF_SIZE];
	size_t bytesRead;
	size_t samples_start = calc_header_size(&in_header);
	
	for (int i=0; i<cycles; i++){
		fseek(inputFile, samples_start, SEEK_SET);
		while ((bytesRead = fread(buffer, 1, sizeof(buffer), inputFile)) > 0)
			fwrite(buffer, sizeof(uint8_t), bytesRead, outputFile);
	}

	// Закрытие файлов
	fclose(inputFile);
	fclose(outputFile);
	printf("\nФайл успешно умножен.\n");
}

int main(int argc, char *argv[]) {
	if (argc != 4) {
		printf("Использование: %s <входной_файл> <выходной_файл> <кол-во повторений>\n", argv[0]);
		return 1;
	}

	int number = atoi(argv[3]);	// Преобразование строки в целое число

	// Передача аргументов в функцию
	mult_wave_file(argv[1], argv[2], number);
	return 0;
}

