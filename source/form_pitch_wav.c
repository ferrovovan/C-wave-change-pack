#include "read_wav.h"
#include <stdint.h>
#include "custom_math.h"


power_macro(int)


const int sampleRate = 44100;
const int bitDepth = 16;
float offset;
float freq, amp, angle = 0.0f;


double process(){
	double sample = amp * sin_float(angle);
	angle += offset;
	if (angle > 1)
		angle -= 1;
	
	return sample;
}

void write_pitch(char* out, float duration){
	FILE *outputFile = fopen(out, "wb");
	if (outputFile == NULL)
		perror("Ошибка открытия выходного файла");
	
	// Создание нового заголовка для выходного файла
	WavHeader outHeader;
	create_WavHeader_base(&outHeader, 2);
	uint32_t result_bytes = seconds_to_bytes_count(duration, &outHeader);
	outHeader.chunkSize = result_bytes + 44;
	outHeader.subchunk2Size = result_bytes;

	// Запись нового заголовка в выходной файл
	fwrite(&outHeader, sizeof(WavHeader), 1, outputFile);    

	
	const int maxAmplitude = power_int(2, bitDepth - 1) - 1;
	const int frames = sampleRate * duration;
	
	long long sample;
	for(int i = 0; i < frames; ++i) {
		
		sample = process() * maxAmplitude;
		uint16_t intSample = sample;
		fwrite(&intSample, sizeof(uint16_t), 1, outputFile);
		fwrite(&intSample, sizeof(uint16_t), 1, outputFile);
	}

	fclose(outputFile);
	printf("\nПроцедура завершена.\n");
}


int main(int argc, char *argv[]) {
	if (argc != 5) {
		printf("Использование: %s <выходной_файл> <frequency> <amplitude (volume)> <time in seconds>\n", argv[0]);
		return 1;
	}
	
	freq = atof(argv[2]);
	amp  = atof(argv[3]);
	float duration = atof(argv[4]);
	offset = 2 * MATH_PI * freq / sampleRate;
	write_pitch(argv[1], duration);
	return 0;
}
