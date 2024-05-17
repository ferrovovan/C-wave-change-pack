#ifndef READ_WAV_H
#define READ_WAV_H
/*стандартный шаблон для защиты заголовочных файлов от многократного включения.*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "wav_header_struct.h"


uint32_t calc_notData_size(WavHeader *header){
	if (strncmp(header->subchunk2Id, "data", 4) == 0)
		return 0;
	else
		return header->subchunk2Size + 8;
}

uint32_t calc_header_size(WavHeader *header){
	uint32_t res = 44;
	res += calc_notData_size(header);
	return res;
}

int readWavHeader(FILE *file, WavHeader *header) {
	// Чтение заголовка WAV-файла
	fread(header, sizeof( WavHeader ), 1, file);

	// Проверка, что заголовок начинается с "RIFF" и формата "WAVE"
	if (strncmp(header->chunkId, "RIFF", 4) != 0 || strncmp(header->format, "WAVE", 4) != 0) {
		perror("Некорректный формат файла");
		return 1;
	}
	//printf("header->subchunk2Id = %s", header->subchunk2Id);
	fseek(file, calc_notData_size(header), SEEK_CUR);

	return 0;  // Успешное чтение заголовка
}

void create_WavHeader_base(WavHeader *header, uint16_t numChannels) {
	#pragma GCC diagnostic ignored "-Wstringop-truncation"
	#pragma GCC diagnostic push
	strncpy(header->chunkId, "RIFF", 4); // const
	strncpy(header->format, "WAVE", 4); // const

	strncpy(header->subchunk1Id, "fmt ", 4); // const
	header->subchunk1Size = 16; // const
	header->audioFormat = 1; // 16-bit in sample
	header->numChannels = numChannels;
	header->sampleRate = 44100;
	
	if( header->audioFormat == 1)
		header->blockAlign = 2 * numChannels;
	else
		header->blockAlign = 4; // ?
	
	header->byteRate = header->sampleRate * header->blockAlign;
	header->bitsPerSample = numChannels * 8;
	strncpy(header->subchunk2Id, "data", 4);
	#pragma GCC diagnostic pop
}

void printWavHeader(WavHeader *header) {
	// header
	printf("Chunk ID: %.4s\n", header->chunkId);
	printf("Chunk Size: %u\n", header->chunkSize);
	printf("Format: %.4s\n", header->format);
	// Format Subchunk
	printf("Subchunk1 ID: %.4s\n", header->subchunk1Id);
	printf("Subchunk1 Size: %u\n", header->subchunk1Size);
	printf("Audio Format: %u\n", header->audioFormat);
	printf("Number of Channels: %u\n", header->numChannels);
	printf("Sample Rate: %u\n", header->sampleRate);
	printf("Byte Rate: %u\n", header->byteRate);
	printf("Block Align: %u\n", header->blockAlign);
	printf("Bits Per Sample: %u\n", header->bitsPerSample);

	// Subchunks Data
	printf("\nSubchunk2 ID: %.4s\n", header->subchunk2Id);
	printf("Subchunk2 Size: %u\n", header->subchunk2Size);
}

int char2digit(char in_char){
	return in_char - '0';  
}

double HHMMSS_to_seconds(const char* time){

	char *last_colon_ptr = strrchr(time, ':');
	if (last_colon_ptr == NULL) {
		return atof(time);
	}

	double result_seconds = atof(last_colon_ptr+1);

	last_colon_ptr--;
	result_seconds += char2digit(*last_colon_ptr) * 60;
	last_colon_ptr--;
	if (last_colon_ptr - time == -1)
		return result_seconds;
	
	if (*last_colon_ptr != ':'){
		result_seconds += char2digit(*last_colon_ptr) * 10 * 60;
		last_colon_ptr--;
	}
	
	if (last_colon_ptr - time == -1)
		return result_seconds;

	last_colon_ptr--;
	result_seconds += char2digit(*last_colon_ptr) * 3600;
	last_colon_ptr--;
	if (last_colon_ptr - time == -1)
		return result_seconds;

	result_seconds += char2digit(*last_colon_ptr) * 10 * 3600;
	return result_seconds;
		
}

uint32_t seconds_to_bytes_count(double time, WavHeader *header){
	uint32_t bytes_count;
	
	if (time == -1)
		bytes_count = header->chunkSize - calc_header_size(header);
	else{
		bytes_count = time * (double)header->byteRate;
	}
	return  bytes_count - (bytes_count % header->blockAlign);
}

double bytes_count_to_seconds(uint32_t bytes_count, WavHeader *header){
	double time = bytes_count / header->byteRate;
	return  time;
}
#endif /* READ_WAV_H */

