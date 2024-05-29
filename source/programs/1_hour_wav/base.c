#include <stdlib.h>

#include "wav_header_struct.h"

/* Переменные алгоритма */
extern FILE *inputFile, *outputFile;
extern WavHeader header_input;
extern uintptr_t ptr_start, ptr_start_rep, ptr_end_rep;
extern uint32_t beginning_bytes_count, end_bytes_count, mix_bytes_count, repetition_bytes_count, repetition_without_mix_bytes_count, result_bytes;
extern int counts;


WavHeader create_HourHeader(WavHeader *header_in, uint32_t result_bytes) {
	WavHeader hourHeader = *header_in;
	hourHeader.chunkSize = result_bytes + 44;

	#pragma GCC diagnostic ignored "-Wstringop-truncation"
	#pragma GCC diagnostic push
	strncpy(hourHeader.subchunk2Id, "data", 4);
	#pragma GCC diagnostic pop
	hourHeader.subchunk2Size = result_bytes;

	return hourHeader;
}


inline static int base(){
	// Создание нового заголовка для выходного файла
	WavHeader hourHeader = create_HourHeader(&header_input, result_bytes);
	// Запись нового заголовка в выходной файл
	fwrite(&hourHeader, sizeof(WavHeader), 1, outputFile);

	/* алгоритм */
	uint8_t *buffer;
	// start
	fseek(inputFile,  ptr_start , SEEK_SET);
	buffer = (uint8_t *)malloc(beginning_bytes_count);
	fread(buffer, beginning_bytes_count, sizeof(uint8_t), inputFile);
	fwrite(buffer, beginning_bytes_count, sizeof(uint8_t), outputFile);
	free(buffer);

	// povtor
	// формирование соединяющего хранилища
	uint8_t *start_buffer, *end_buffer, *mix_buffer;

	start_buffer = (uint8_t *)malloc(mix_bytes_count);
	fseek(inputFile,  ptr_start_rep , SEEK_SET);
	fread(start_buffer, sizeof(uint8_t), mix_bytes_count, inputFile);

	buffer = (uint8_t *)malloc(repetition_without_mix_bytes_count);
	fseek(inputFile,  ptr_start_rep + mix_bytes_count , SEEK_SET);
	fread(buffer, sizeof(uint8_t), repetition_without_mix_bytes_count,  inputFile);
	
	end_buffer = (uint8_t *)malloc(mix_bytes_count);
	fseek(inputFile,  ptr_end_rep - mix_bytes_count , SEEK_SET);
	fread(end_buffer, sizeof(uint8_t), mix_bytes_count, inputFile);
	
	mix_buffer = (uint8_t *)malloc(mix_bytes_count);

	float progress;
	int volume_i;
	
	if (header_input.bitsPerSample == 8){

	int8_t start_num, end_num, result;
	for (unsigned int i = 0; i < mix_bytes_count; ++i) {

		start_num = start_buffer[i];
		end_num   = end_buffer[i];
		
		progress = (float)i / mix_bytes_count;
		volume_i = (progress * 256 + 0.5);
		start_num = ((int64_t)start_num * volume_i + 128) >> 8;

		progress = 1 - progress;
		volume_i = (progress * 256 + 0.5);
		end_num = ((int64_t)end_num * volume_i + 128) >> 8;


		result = (start_num + end_num) / 2;
		mix_buffer[i] = result;
	}
	} else if (header_input.bitsPerSample == 16){

	int16_t start_num=0, end_num=0, result;
	
	for (uint32_t i = 0; i < mix_bytes_count; i+=2) {
		start_num = ((int16_t)start_buffer[i+1] << 8) | start_buffer[i];
		end_num   = ((int16_t)end_buffer[i+1]   << 8) | end_buffer[i];
		
		progress = (float)i / mix_bytes_count;
		volume_i = (progress * 256 + 0.5);
		start_num = ((int64_t)start_num * volume_i + 128) >> 8;

		progress = 1 - progress;
		volume_i = (progress * 256 + 0.5);
		end_num = ((int64_t)end_num * volume_i + 128) >> 8;
		

		result = ((int64_t)start_num + (int64_t)end_num);
		mix_buffer[i+1]   = result >> 8;
		mix_buffer[i] = result;
	}
	} else{
		printf("Не поддерживаемая разрядность семплов: %d (bitsPerSample).\n", header_input.bitsPerSample);
		return -1;
	}


	// запись повтора
	fwrite(start_buffer, 1, mix_bytes_count, outputFile);
	free(start_buffer);

	for (int i=0; i < counts; ++i){
		fwrite(buffer, 1, repetition_without_mix_bytes_count, outputFile);
		fwrite(mix_buffer, 1, mix_bytes_count, outputFile);
	}
	free(mix_buffer);
	fwrite(buffer, 1, repetition_without_mix_bytes_count, outputFile);
	free(buffer);

	
	fwrite(end_buffer, 1, mix_bytes_count, outputFile);
	free(end_buffer);


	// end
	fseek(inputFile,  ptr_end_rep , SEEK_SET);
	buffer = (uint8_t *)malloc(end_bytes_count);
	fread(buffer, end_bytes_count, sizeof(uint8_t), inputFile);
	fwrite(buffer, end_bytes_count, sizeof(uint8_t), outputFile);
	free(buffer);
	
	return 0;
}
