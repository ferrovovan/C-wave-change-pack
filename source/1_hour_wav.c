#define PRINT_HEADER 0

#include "read_wav.h"
#include <string.h>

uint16_t uint8_to_uint16(uint8_t val1, uint8_t val2) {
	return ( (uint16_t)val1 << 8) | val2;
}

WavHeader create_HourHeader(WavHeader *header_in, uint32_t result_bytes) {
	WavHeader hourHeader = *header_in;  // Копируем все поля из входного заголовка

	hourHeader.chunkSize = result_bytes + 44;

	strncpy(hourHeader.subchunk2Id, "data", 4);
	hourHeader.subchunk2Size = result_bytes;

	return hourHeader;
}

void hour_wav_file(char* in, char* start_rep, char* end_rep, char* mix_dlit, char* expected_dur, char* out, short info){

	FILE *inputFile = fopen(in, "rb");
	if (inputFile == NULL) {
		perror("Ошибка открытия входного файла");
		return -1;
	}

	WavHeader header_in;
	readWavHeader(inputFile, &header_in);
	if (PRINT_HEADER){
		printf("\nfirst header:\n");
		printWavHeader(&header_in);
	}


#define CALCULATE_BYTES_COUNT(prefix, default_dur) \
	uint32_t prefix##_bytes_count; \
	if (prefix != NULL) { \
		long double seconds = HHMMSS_to_seconds(prefix); \
		prefix##_bytes_count = seconds_to_bytes_count(seconds, 	&header_in); \
	} else { \
		prefix##_bytes_count = seconds_to_bytes_count(default_dur, &header_in); \
	}

	printf("\n");
	CALCULATE_BYTES_COUNT(start_rep, 0);
	CALCULATE_BYTES_COUNT(end_rep, -1);

	const uintptr_t ptr_start = (uintptr_t)calc_header_size(&header_in);
	const uintptr_t ptr_start_rep = (uintptr_t)start_rep_bytes_count + calc_header_size(&header_in);
	const uintptr_t ptr_end_rep = (uintptr_t)end_rep_bytes_count + calc_header_size(&header_in);

	CALCULATE_BYTES_COUNT(mix_dlit, 0);
	CALCULATE_BYTES_COUNT(expected_dur, 3600);
	if (expected_dur_bytes_count < header_in.chunkSize - calc_header_size(&header_in) ){
		perror("Ожидаемая длительность меньше входной длительности. Увеличте параметр -ed.\n");
		expected_dur_bytes_count = header_in.chunkSize - calc_header_size(&header_in);
	}

	const uint32_t rep_dlit_bytes_count = end_rep_bytes_count -  start_rep_bytes_count;
	const uint32_t rep_dlit_without_mix_bytes_count = rep_dlit_bytes_count - mix_dlit_bytes_count;

	if (rep_dlit_without_mix_bytes_count < mix_dlit_bytes_count){
		perror("Буферная зона слишком велика. Уменьшите параметр -md.\n");
		return -1;
	}

	const uint32_t end_bytes_count = (header_in.chunkSize - calc_header_size(&header_in) - end_rep_bytes_count);

	const uint32_t counts   =   (expected_dur_bytes_count - start_rep_bytes_count - end_bytes_count ) / rep_dlit_bytes_count;
	const uint32_t result_bytes = start_rep_bytes_count + end_bytes_count + counts * rep_dlit_bytes_count;

	if (info){
		fclose(inputFile);
		
		printf("Byte block:\n");
		printf("start_rep_bytes_count = %d;\n", start_rep_bytes_count);
		printf("end_rep_bytes_count = %d;\n", end_rep_bytes_count);
		printf("rep_dlit_bytes_count = %d;\n", rep_dlit_bytes_count);
		
		printf("\nmix_dlit_bytes_count = %d;\n", mix_dlit_bytes_count);
		printf("rep_dlit_without_mix_bytes_count = %d;\n", rep_dlit_without_mix_bytes_count);
		
		printf("\nend_bytes_count = %d;\n", end_bytes_count);
		printf("result_bytes = %d;\n", result_bytes);
		
		printf("\nTime block:\n");
		printf("counts = %d;\n", counts);
		
		printf("\nУспешная проверка параметров.\n");
	} else {
		// Создание нового заголовка для выходного файла
		WavHeader hourHeader = create_HourHeader(&header_in, result_bytes);
		if (PRINT_HEADER){
			printf("\nhour Header\n");
			printWavHeader(&hourHeader);
		}
	
		FILE *outputFile = fopen(out, "wb");
		if (outputFile == NULL) {
			perror("Ошибка открытия выходного файла");
			fclose(inputFile);
		}
		// Запись нового заголовка в выходной файл
		fwrite(&hourHeader, sizeof(WavHeader), 1, outputFile);


		/* алгоритм */
		uint8_t *buffer;
		// start
		fseek(inputFile,  ptr_start , SEEK_SET);
		buffer = (uint8_t *)malloc(start_rep_bytes_count);
		fread(buffer, start_rep_bytes_count, sizeof(uint8_t), inputFile);
		fwrite(buffer, start_rep_bytes_count, sizeof(uint8_t), outputFile);
		free(buffer);

		// povtor
		buffer = (uint8_t *)malloc(rep_dlit_without_mix_bytes_count);
		fseek(inputFile,  ptr_start_rep, SEEK_SET);
		fread(buffer, rep_dlit_without_mix_bytes_count, sizeof(uint8_t), inputFile);
		
		// формирование буфера
		uint8_t *mix_buffer = (uint8_t *)malloc(mix_dlit_bytes_count);
		
		float progress;
	
		fseek(inputFile, ptr_end_rep - (uintptr_t)mix_dlit_bytes_count, SEEK_SET);
		fread(mix_buffer, mix_dlit_bytes_count, sizeof(uint8_t), inputFile);

		
		if (header_in.bitsPerSample == 8){
		uint8_t first_num, second_num;
		for (unsigned int i = 0; i < mix_dlit_bytes_count/2; ++i) {
			progress = (float)i / mix_dlit_bytes_count;
			first_num = mix_buffer[i] * progress;
			//first_num = mix_buffer[i];

			second_num = buffer[i] * (1 - progress);

			//first_num = 0;
			second_num = 0;
			mix_buffer[i] = (first_num + second_num) / 2;
			//mix_buffer[i] = first_num + second_num;
		}
		else if (header_in.bitsPerSample == 16){
		uint16_t first_num, second_num, result;
		for (unsigned int i = 0; i < mix_dlit_bytes_count/2; i+=2) {
			progress = (float)(i*2) / mix_dlit_bytes_count;
			first_num = uint8_to_uint16(mix_buffer[i+1], mix_buffer[i]) / 2/** progress*/;
			printf("before=%d, after=%d\n", first_num, first_num - 1024 );
			
			second_num = uint8_to_uint16(buffer[i+1], buffer[i]) * (1 - progress);

			//first_num = 0;
			second_num = 0;
			result = (first_num + second_num);

			mix_buffer[i+1] = (uint8_t)(result >> 8);
			mix_buffer[i] = (uint8_t)result;
		}
		} else{
			printf("Не поддерживаемая разрядность семплов (bitsPerSample).\n"
		}
		
		// запись повтора
		for (int i=0; i < counts; ++i){
			fwrite(buffer, rep_dlit_without_mix_bytes_count, 1, outputFile); // база
			fwrite(mix_buffer, mix_dlit_bytes_count, 1, outputFile); // склейка
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
	}

	
}


void print_usage(char *program_name){
	/* input, start repetition, end repetition, mix duration, output
	*/
	printf("Использование: %s -i <входной_файл> -sr <начало повтора> -er <конец повтора> -md <длительность наложения> -ed <ожидаемая длительность> -o <выходной_файл>  [опциональные параметры]\n", program_name);
	printf("Опциональные параметры:\n--info\t - предполагаемый итоговый хедер\n");
}

int main(int argc, char *argv[]) {

	char *in=NULL, *start_rep=NULL, *end_rep=NULL, *mix_dlit=NULL, *expected_dur=NULL, *out=NULL;
	short info=0;

	#define CASE_ARG(ptr, flag, flag_len) \
	if (strncmp(argv[arg], flag, flag_len) == 0) { \
		arg++; \
		if (arg < argc) \
			ptr = argv[arg]; \
		else \
			printf("Неверный параметр flag"); \
	}

	for (int arg=1; arg < argc;++arg){
		CASE_ARG(in, "-i", 2)
		else CASE_ARG(start_rep, "-sr", 3)
		else CASE_ARG(end_rep, "-er", 3)
		else CASE_ARG(mix_dlit, "-md", 3)
		else CASE_ARG(expected_dur, "-ed", 3)
		else CASE_ARG(out, "-o", 2)
		else if (strncmp(argv[arg], "--info", 6) == 0 ){
			info = 1;
		} else {
			print_usage(argv[0]);
			return -1;
		}
	}
	// Валидация считанных данных
	if (in == NULL){
		printf("Не указан входной файл.\n-i <входной_файл>\n");
		return -2;
	} else if (info == 0 && out == NULL){
		printf("Не указан выходной файл.\n-o <выходной_файл>\n");
		return -2;
	}

	// Передача аргументов в функцию
	hour_wav_file(in, start_rep, end_rep, mix_dlit, expected_dur, out, info);
	return 0;
}

