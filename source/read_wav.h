#ifndef READ_WAV_H
#define READ_WAV_H
/*стандартный шаблон для защиты заголовочных файлов от многократного включения.*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


typedef struct {
	char chunkId[4];	// 4, "RIFF" = 0x4646 4952
	uint32_t chunkSize;	// 4
	char format[4];		// 4, "WAVE" = 0x5741 5645

	char subchunk1Id[4];	// 4, "fmt " = 0x2074 6D66
	uint32_t subchunk1Size;	// 4, "16" = 0x1000 0000
	uint16_t audioFormat;	// 2, "1"  = 0x0100
	uint16_t numChannels;	// 2
	uint32_t sampleRate;	// 4, "44100" = 0x0200 44ac
	uint32_t byteRate;	// 4, "176400" = 0x000010b1
	uint16_t blockAlign;	// 2
	uint16_t bitsPerSample;	// 2
	
	char subchunk2Id[4];	// 4, in simple "data", maybe others for example "LIST"
	uint32_t subchunk2Size;	// 4
	// in sum - 44
} WavHeader;


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
#endif /* READ_WAV_H */

