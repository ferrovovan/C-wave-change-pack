#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "read_wav.h"


int calculate_constans(){
	double seconds;

	readWavHeader(inputFile, &header_input);
	ptr_start = (uintptr_t)calc_header_size(&header_input);

	if (start_rep != NULL) {
		seconds = HHMMSS_to_seconds(start_rep);
		beginning_bytes_count = seconds_to_bytes_count(seconds, 	&header_input);
		
	} else {
		beginning_bytes_count = seconds_to_bytes_count(0, &header_input);
	}
	
	
	ptr_start_rep = ptr_start + (uintptr_t)beginning_bytes_count;
	
	if (end_rep != NULL) {
		seconds = HHMMSS_to_seconds(end_rep);
		ptr_end_rep = (uintptr_t)seconds_to_bytes_count(seconds, 	&header_input);
	} else {
		ptr_end_rep = (uintptr_t)seconds_to_bytes_count(-1, &header_input);
	}
	ptr_end_rep += ptr_start;
	
	if (ptr_end_rep < ptr_start_rep){
		printf("Конец повтора раньше начала. Увеличьте параметр -end или уменьшите параметр -st.\n");
		return EXIT_FAILURE;
	}
	repetition_bytes_count = (uint32_t)(ptr_end_rep - ptr_start_rep);
	

	if (mix_dlit != NULL) {
		seconds = HHMMSS_to_seconds(mix_dlit);
		mix_bytes_count = seconds_to_bytes_count(seconds, 	&header_input);
	} else {
		mix_bytes_count = seconds_to_bytes_count(0, &header_input);
	}

	if (repetition_bytes_count < 2*mix_bytes_count)
		repetition_without_mix_bytes_count = 0;
	else
		repetition_without_mix_bytes_count = repetition_bytes_count - 2*mix_bytes_count;
	

	const uint32_t new_rep_dlit_bytes_count = repetition_without_mix_bytes_count + mix_bytes_count;
	uint32_t expected_dur_bytes_count;

	if (expected_dur != NULL) {
		seconds = HHMMSS_to_seconds(expected_dur);
		expected_dur_bytes_count = seconds_to_bytes_count(seconds, 	&header_input);
	} else {
		expected_dur_bytes_count = seconds_to_bytes_count(3600, &header_input);
	}
	
	if (expected_dur_bytes_count < header_input.chunkSize - ptr_start ){
		printf("Ожидаемая длительность меньше длительности входного файла. Увеличьте параметр -ed.\n");
		return EXIT_FAILURE;
	}
	

	end_bytes_count = (header_input.chunkSize - (uint32_t)ptr_end_rep);
	counts   =   (expected_dur_bytes_count - beginning_bytes_count - end_bytes_count) / new_rep_dlit_bytes_count;

	result_bytes = (beginning_bytes_count + repetition_bytes_count + end_bytes_count) + counts * new_rep_dlit_bytes_count;

	return EXIT_SUCCESS;
}
