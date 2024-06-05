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

void write_pitch(FILE *outputFile, float duration){

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

	printf("\nПроцедура завершена.\n");
}


int main(int argc, char *argv[]) {
	if (argc != 5) {
		printf("Использование: %s <выходной_файл> <frequency> <amplitude (volume)> <time in seconds>\n", argv[0]);
		return EXIT_FAILURE;
	}

	FILE *outputFile = fopen(argv[1], "wb");
	if (outputFile == NULL) {
		printf("Ошибка открытия выходного файла.\n");
		return EXIT_FAILURE;
	}
	
	freq = atof(argv[2]);
	if( freq == 0 ){
		printf("Некорректное значение частоты.\n");
		return EXIT_FAILURE;
	}

	amp  = atof(argv[3]);
	if( amp == 0 ){
		printf("Некорректное значение амплитуды.\n");
		return EXIT_FAILURE;
	}

	float duration = atof(argv[4]);
	if( duration == 0 ){
		printf("Некорректное значение длительности.\n");
		return EXIT_FAILURE;
	}

	offset = 2 * MATH_PI * freq / sampleRate;

	write_pitch(outputFile, duration);
	
	fclose(outputFile);
	return EXIT_SUCCESS;
}
