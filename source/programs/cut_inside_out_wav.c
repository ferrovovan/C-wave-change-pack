// args parser including
#define REQ_ARG_INPUT
#define REQ_ARG_OUTPUT

#define ARG_START_TIME
#define ARG_END_TIME
#define ARG_MD_TIME

#define ARG_TEST_FLAG

#include "args_parser.h"
// including end


#include "read_wav.h"


#define CHS calc_header_size(&in_header)

int cut_wav_file(FILE *inputFile, FILE *outputFile, double start_time, double end_time, double mix_duration, short info){

	WavHeader in_header;
	readWavHeader(inputFile, &in_header);


	const uintptr_t ptr_start     = CHS;
	const uintptr_t ptr_end       = in_header.chunkSize;
	const uintptr_t ptr_start_cut =  (uintptr_t)seconds_to_bytes_count(start_time, &in_header) + CHS;
	const uintptr_t   ptr_end_cut   = (uintptr_t)seconds_to_bytes_count(end_time, &in_header) + CHS;


	if (ptr_end_cut <= ptr_start_cut){
		perror("Конец меньше начала.\n");
		return -1;
	}

	const uint32_t start_t_bytes_count = ptr_start_cut - CHS;
	const uint32_t end_t_bytes_count   = ptr_end - ptr_end_cut;
	const uint32_t md_bytes_count = seconds_to_bytes_count(mix_duration, &in_header);
	const uint32_t res_duration_bytes = start_t_bytes_count + end_t_bytes_count - md_bytes_count;
	

	if (info){
		printf("\n");
		printf("\nfirst header:\n");
		printWavHeader(&in_header);

		printf("\n-----------\n");
		printf("Byte block:\n");
		printf("start_time_bytes_count = %d;\n", start_t_bytes_count);
		printf("end_time_bytes_count = %d;\n", end_t_bytes_count);


		printf("\nTime block:\n");
		float time = bytes_count_to_seconds(res_duration_bytes, &in_header);
		printf("duration = %f seconds;\n", time);
		
		return 0;
	} else {
		// Создание нового заголовка для выходного файла
		WavHeader cutHeader;
		copy_WavHeader(&in_header, &cutHeader);
		cutHeader.chunkSize = res_duration_bytes + 44;
		cutHeader.subchunk2Size = res_duration_bytes;
		fwrite(&cutHeader, sizeof(cutHeader), 1, outputFile);


		/* алгоритм */
		uint8_t *buffer;
		uint32_t temp = start_t_bytes_count - md_bytes_count;
		buffer = (uint8_t *)malloc(temp);

		fseek(inputFile,  ptr_start , SEEK_SET);
		fread(buffer, temp, sizeof(uint8_t), inputFile);
		fwrite(buffer, temp, sizeof(uint8_t), outputFile);
		free(buffer);


	// запись смешивания
		uint8_t *start_buffer = (uint8_t *)malloc(md_bytes_count);
		fread(start_buffer, sizeof(uint8_t), md_bytes_count, inputFile);
	
		uint8_t *end_buffer = (uint8_t *)malloc(md_bytes_count);
	fseek(inputFile,  ptr_end_cut , SEEK_SET);
	fread(end_buffer, sizeof(uint8_t), md_bytes_count, inputFile);
		

		uint8_t *mix_buffer = (uint8_t *)malloc(md_bytes_count);		
		if (in_header.bitsPerSample == 16)
	{
	int16_t start_num=0, end_num=0, result, volume_i;
	float progress;
	
	for (uint32_t i = 0; i < md_bytes_count; i+=2) {
		start_num = ((int16_t)start_buffer[i+1] << 8) | start_buffer[i];
		end_num   = ((int16_t)end_buffer[i+1]   << 8) | end_buffer[i];
		
		progress = (float)i / md_bytes_count;
		volume_i = (progress * 256 + 0.5);
		start_num = ((int64_t)start_num * volume_i + 128) >> 8;

		progress = 1 - progress;
		volume_i = (progress * 256 + 0.5);
		end_num = ((int64_t)end_num * volume_i + 128) >> 8;
		

		result = ((int64_t)start_num + (int64_t)end_num);
		mix_buffer[i+1]   = result >> 8;
		mix_buffer[i] = result;
	}
		}else{
			printf("Не поддерживаемая разрядность семплов: %d (bitsPerSample).\n", in_header.bitsPerSample);
			return -1;
		}
		
		fwrite(mix_buffer, md_bytes_count, sizeof(uint8_t), outputFile);
		free(mix_buffer);


		temp = end_t_bytes_count - md_bytes_count;
		buffer = (uint8_t *)malloc(temp);
		fread(buffer, temp, sizeof(uint8_t), inputFile);
		fwrite(buffer, temp, sizeof(uint8_t), outputFile);


		fwrite(buffer, res_duration_bytes, sizeof(uint8_t), outputFile);
		free(buffer);

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

	double start_time_double, end_time_double, mix_duration_double;
	READ_VALUE(start_time, 0)
	READ_VALUE(end_time, -1)
	READ_VALUE(mix_duration, 0)

// Передача аргументов в функцию
	cut_wav_file(inputFile, outputFile, start_time_double, end_time_double, mix_duration_double, (short )test_flag);

	fclose(inputFile);
	fclose(outputFile);

	return EXIT_SUCCESS;
}

