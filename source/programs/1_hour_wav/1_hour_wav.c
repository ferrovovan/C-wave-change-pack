#include "1_hour_wav.h"

int main(int argc, char *argv[]) {

	int code;
	short info=0, test_rep=0;

	code = read_params(argc, argv, &info, &test_rep);
	if (code != 0){
		perror("Некорректное считывание параметров");
		return -1;
	}

	inputFile = fopen(in, "rb");
	if (inputFile == NULL) {
		perror("Ошибка открытия входного файла");
		return -1;
	}

	code = calculate_constans();
	if (code != 0)
		return -1;

	if (info){
		fclose(inputFile);

		print_info();
		printf("\nУспешная проверка параметров.\n");
		return 0;
	}
	

	// TODO:
	if (test_rep != 0){
		test_rep_func(); // записывает только фрагмент повторения
		return 0;
	}

	outputFile = fopen(out, "wb");
	if (outputFile == NULL) {
		perror("Ошибка открытия выходного файла");
		fclose(inputFile);
		return -1;
	}

	base();

	// Закрытие файлов
	fclose(inputFile);	fclose(outputFile);
	printf("\nУспешная запись.\n");
	return 0;
}

