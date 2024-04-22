#include <stdio.h>

#include "../wav_header_struct.h"

extern uint32_t beginning_bytes_count, end_bytes_count, mix_bytes_count, repetition_bytes_count, repetition_without_mix_bytes_count, result_bytes;
extern int counts;
extern WavHeader header_input;


void print_info(){
	printf("Byte block:\n");
	printf("beginning_bytes_count = %d;\n", beginning_bytes_count);
	printf("repetition_bytes_count = %d;\n", repetition_bytes_count);
	printf("end_bytes_count = %d;\n", end_bytes_count);

	printf("\nmix_bytes_count = %d;\n", mix_bytes_count);
	printf("repetition_without_mix_bytes_count = %d;\n", repetition_without_mix_bytes_count);

	printf("result_bytes = %d;\n", result_bytes);

	printf("\nTime block:\n");
	printf("counts = %d;\n", counts);
	float time = bytes_count_to_seconds(result_bytes, &header_input);
	printf("duration = %f seconds;\n", time);
}
