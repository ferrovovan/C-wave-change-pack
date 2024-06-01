#ifndef wav_header_struct_H
#define wav_header_struct_H

typedef struct {
	char chunkId[4];	// 4, "RIFF" = 0x4646 4952
	uint32_t chunkSize;	// 4
	char format[4];		// 4, "WAVE" = 0x5741 5645

	char subchunk1Id[4];	// 4, "fmt " = 0x2074 6D66
	uint32_t subchunk1Size;	// 4, "16" = 0x1000 0000
	uint16_t audioFormat;	// 2, "1"  = 0x0100 PCM
	uint16_t numChannels;	// 2
	uint32_t sampleRate;	// 4, "44100" = 0x0200 44ac	Hz
	uint32_t byteRate;	// 4, "176400" = 0x000010b1
	uint16_t blockAlign;	// 2, "4"
	uint16_t bitsPerSample;	// 2, "16"
	
	char subchunk2Id[4];	// 4, in simple "data", maybe others for example "LIST"
	uint32_t subchunk2Size;	// 4
	// in sum - 44
} WavHeader;

#endif /* wav_header_struct_H */

