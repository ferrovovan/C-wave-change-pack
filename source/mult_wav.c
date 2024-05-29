// args parser including
#define REQ_ARG_INPUT
#define REQ_ARG_OUTPUT
#define REQ_ARG_COUNT

#include "args_parser.h"
// including end


#include "read_wav.h"

#define BUFF_SIZE 1024
#define PRINT_HEADER 1


void mult_wave_file(FILE *inputFile, FILE *outputFile, const int cycles){
	// Считывание заголовка первого файла
	WavHeader in_header;
	readWavHeader(inputFile, &in_header);
	if (PRINT_HEADER){
		printf("\nIn header:\n");
		printWavHeader(&in_header);
	}

    
	// Создание нового заголовка для выходного файла
	WavHeader out_header;
	copy_WavHeader(&in_header, &out_header);
	uint32_t result_bytes = calc_main_data_size(&in_header) * cycles;
	out_header.chunkSize = result_bytes + 44;
	out_header.subchunk2Size = result_bytes;

	if (PRINT_HEADER){
		printf("\nOut header:\n");
		printWavHeader(&out_header);
	}
	fwrite(&out_header, sizeof(out_header), 1, outputFile);    


	uint8_t buffer[BUFF_SIZE];
	size_t bytesRead;
	size_t samples_start = calc_header_size(&in_header);
	
	for (int i=0; i<cycles; i++){
		fseek(inputFile, samples_start, SEEK_SET);
		while ((bytesRead = fread(buffer, 1, sizeof(buffer), inputFile)) > 0)
			fwrite(buffer, sizeof(uint8_t), bytesRead, outputFile);
	}

	printf("\nФайл успешно умножен.\n");
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
	int cycles = atoi(count);
	if( cycles == 0 ){
		printf("Некорректное значение циклов.\n");
		return EXIT_FAILURE;
	}

// Передача аргументов в функцию
	mult_wave_file(inputFile, outputFile, cycles);

	fclose(inputFile);	fclose(outputFile);
	return EXIT_SUCCESS;
}

