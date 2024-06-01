#include "1_hour_wav.h"

int main(int argc, char *argv[]) {

	int code;
	short info=0, test_rep=0;

	code = read_params(argc, argv, &info, &test_rep);
	if (code != 0){
		printf("Некорректное считывание параметров.\n");
		return EXIT_FAILURE;
	}

	inputFile = fopen(in, "rb");
	if (inputFile == NULL) {
		printf("Ошибка открытия входного файла.\n");
		return EXIT_FAILURE;
	}

	code = calculate_constans();
	if (code != EXIT_SUCCESS)
		return EXIT_FAILURE;

	if (info){
		fclose(inputFile);
		print_info();
		printf("\nУспешная проверка параметров.\n");
		return EXIT_SUCCESS;
	}
	// TODO:
	else if (test_rep){
		test_rep_func(); // записывает только фрагмент повторения
		fclose(inputFile);
		return EXIT_SUCCESS;
	}
	else{
		outputFile = fopen(out, "wb");
		if (outputFile == NULL) {
			printf("Ошибка открытия выходного файла.\n");
			fclose(inputFile);
			return EXIT_FAILURE;
		}

		base();

		// Закрытие файлов
		fclose(inputFile);	fclose(outputFile);
		printf("\nУспешная запись.\n");
	}
	return EXIT_SUCCESS;
}

