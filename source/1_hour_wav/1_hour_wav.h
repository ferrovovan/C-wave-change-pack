#include <stdlib.h>
#include <stdio.h>
#include "../read_wav.h"

/* Переменные считывания */
char *in = NULL, *out = NULL, *start_rep = NULL, *end_rep = NULL, *mix_dlit = NULL, *expected_dur = NULL;

/* Переменные алгоритма */
FILE *inputFile, *outputFile;
WavHeader header_input;
uintptr_t ptr_start, ptr_start_rep, ptr_end_rep;
uint32_t beginning_bytes_count, end_bytes_count, mix_bytes_count, repetition_bytes_count, repetition_without_mix_bytes_count, result_bytes;
int counts;


#include "read_args.c"
#include "print_info.c"
#include "test_rep.c"
#include "calculate_constans.c"
#include "base.c"

