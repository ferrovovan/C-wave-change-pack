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

uint32_t calc_main_data_size(WavHeader *header){
	return ( header->chunkSize - calc_header_size(header) );
}

int readWavHeader(FILE *file, WavHeader *header) {
	fseek(file, 0, SEEK_CUR);
	if( fread(header, sizeof(WavHeader), 1, file) != 1 ){
		perror("Not enough bytes");
		return -2;
	}
	


#define compare_macro(str, ptr) \
	if (strncmp(ptr, str, 4) != 0){ \
		goto incorrect_format; \
	}

//  chunkId
	compare_macro("RIFF", header->chunkId)
//  chunkSize
//  format
	compare_macro("WAVE", header->format)
//  subchunk1Id
	compare_macro("fmt ", header->subchunk1Id)
//  subchunk1Size
	if( header->subchunk1Size != 16 )
		goto incorrect_format;
//  audioFormat
	if( header->audioFormat != 1 )
		goto incorrect_format;
//  numChannels
//  sampleRate
	/*
	if( header->sampleRate != 44100 )
		goto incorrect_format;
	*/
//  byteRate
	/*
	if( header->byteRate != 176400 )
		goto incorrect_format;
	*/
//  blockAlign
//  bitsPerSample

// subchunk2Id
// subchunk2Size
	fseek(file, calc_notData_size(header), SEEK_CUR);
	return 0;  // Успешное чтение заголовка
	
	incorrect_format:
		perror("Incorrect file format");
		return -1;
}

void create_WavHeader_base(WavHeader *header, uint16_t numChannels) {
	#pragma GCC diagnostic ignored "-Wstringop-truncation"
	#pragma GCC diagnostic push
	strncpy(header->chunkId, "RIFF", 4); // const
	strncpy(header->format, "WAVE", 4); // const

	strncpy(header->subchunk1Id, "fmt ", 4); // const
	header->subchunk1Size = 16; // const
	header->audioFormat = 1; // PCM Format,
/* also read rfc2361.txt for more Format names */
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

void copy_WavHeader(WavHeader *in_header, WavHeader *out_header){
	*out_header = *in_header;
	strncpy(out_header->subchunk2Id, "data", 4);
}

// parse functions
int char2digit(char in_char){
	return in_char - '0';  
}

double HHMMSS_to_seconds(const char* time_str){
	int len = strlen(time_str);
	int colons = 0, dots = 0, val;

	// Валидация разделителей
	for (int i = 0; i < len; ++i) {
		if (time_str[i] == ':') {
			if( dots )
				return -1;
			colons++;
		} else if (time_str[i] == '.') {
			dots++;
		}
		else {
			val = char2digit(time_str[i]);
			if ( val < 0 || val > 9 )
				return -1;
		}
	}

	if (colons > 2 || dots > 1) {
		return -1;
	}


	double result_seconds;
	char *last_colon_ptr = strrchr(time_str, ':');
	if (last_colon_ptr == NULL) {
		return atof(time_str);
	}

	val = char2digit( *(last_colon_ptr+1) );
	if ( val < 0 || val > 9 )
		return -1;
	result_seconds = atof(last_colon_ptr+1);

	last_colon_ptr--;
	result_seconds += char2digit(*last_colon_ptr) * 60;
	last_colon_ptr--;
	if (last_colon_ptr - time_str == -1)
		return result_seconds;
	
	if (*last_colon_ptr != ':'){
		result_seconds += char2digit(*last_colon_ptr) * 10 * 60;
		last_colon_ptr--;
	}
	
	if (last_colon_ptr - time_str == -1)
		return result_seconds;

	last_colon_ptr--;
	result_seconds += char2digit(*last_colon_ptr) * 3600;
	last_colon_ptr--;
	if (last_colon_ptr - time_str == -1)
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
