#define BUFF_SIZE 1024
#define PRINT_HEADER 0

#include "read_wav.h"


WavHeader createCombinedHeader(WavHeader *header1, WavHeader *header2) {
	WavHeader combinedHeader = *header1;  // Копируем все поля из первого заголовка

	// Изменяем Chunk Size на сумму Chunk Size из обоих заголовков
	uint32_t first_samples_size	= header1->chunkSize - calc_header_size(header1);
	uint32_t second_samples_size	= header2->chunkSize - calc_header_size(header2);

	uint32_t samples_size = first_samples_size + second_samples_size;
	
	combinedHeader.chunkSize = samples_size + 44;
	

	strcpy(combinedHeader.subchunk2Id, "data");
	combinedHeader.subchunk2Size = samples_size;

	return combinedHeader;
}

void insert_wav_files(FILE *inputFile1, FILE *inputFile2, FILE *outputFile, \
	double insert_time){
	// Чтение заголовка первого файла
	WavHeader header1;
	readWavHeader(inputFile1, &header1);
	if (PRINT_HEADER){
		printf("\nfirst header:\n");
		printWavHeader(&header1);
	}

	// Чтение  заголовка второго файла
	WavHeader header2;
	readWavHeader(inputFile2, &header2);
	if (PRINT_HEADER){
		printf("\nsecond header:\n");
		printWavHeader(&header2);
	}
    
	// Создание нового заголовка для выходного файла
	WavHeader combinedHeader = createCombinedHeader(&header1, &header2);
	if (PRINT_HEADER){
		printf("\ncombined Header\n");
		printWavHeader(&combinedHeader);
	}
    
	// Запись нового заголовка в выходной файл
	fwrite(&combinedHeader, sizeof(WavHeader), 1, outputFile);    


	/*  Алгоритм  */
	const uintptr_t ptr_start_insert =  (uintptr_t)seconds_to_bytes_count(insert_time, &header1);
	uint8_t buffer[BUFF_SIZE];
	size_t bytesRead;

	for (uintptr_t i=0; i < ptr_start_insert - BUFF_SIZE; i += BUFF_SIZE){
		bytesRead = fread(buffer, 1, sizeof(buffer), inputFile1);
		fwrite(buffer, 1, bytesRead, outputFile);
	}
	bytesRead = fread(buffer, 1, ptr_start_insert % BUFF_SIZE, inputFile1);
	fwrite(buffer, 1, bytesRead, outputFile);

	while ((bytesRead = fread(buffer, 1, sizeof(buffer), inputFile2)) > 0)
		fwrite(buffer, 1, bytesRead, outputFile);

	while ((bytesRead = fread(buffer, 1, sizeof(buffer), inputFile1)) > 0)
		fwrite(buffer, 1, bytesRead, outputFile);


	printf("\nФайлы успешно объединены.\n");
}

int main(int argc, char *argv[]) {
	if (argc != 5) {
		printf("Использование: %s <входной_файл1> <входной_файл2> <время> <выходной_файл>\n", argv[0]);
		return EXIT_FAILURE;
	}
	
	double time = HHMMSS_to_seconds(argv[3]);
	if( time == -1 ){
		printf("Неверный формат %s\n", argv[3]);
		return EXIT_FAILURE;
	}
	
	// Открытие первого входного файла для чтения
	FILE *inputFile1 = fopen(argv[1], "rb");
	if (inputFile1 == NULL) {
		printf("Ошибка открытия первого входного файла.\n");
		return EXIT_FAILURE;
	}

	// Открытие второго входного файла для чтения
	FILE *inputFile2 = fopen(argv[2], "rb");
	if (inputFile2 == NULL) {
		printf("Ошибка открытия второго входного файла.\n");
		fclose(inputFile1);
		return EXIT_FAILURE;
	}

	// Открытие выходного файла для записи
	FILE *outputFile = fopen(argv[4], "wb");
	if (outputFile == NULL) {
		printf("Ошибка открытия выходного файла.\n");
		fclose(inputFile1);	fclose(inputFile2);
		return EXIT_FAILURE;
	}


	insert_wav_files(inputFile1, inputFile2, outputFile, time);
	
	fclose(inputFile1);	fclose(inputFile2);	fclose(outputFile);
	return EXIT_SUCCESS;
}

