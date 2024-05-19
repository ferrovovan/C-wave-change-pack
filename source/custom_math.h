#ifndef CUSTOM_MATH_H
#define CUSTOM_MATH_H

#define MATH_PI 3.14159265358979323846

long long factorial(int n) {
	long long result = 1;
	for (int i = 1; i <= n; i++) {
		result *= i;
	}
	return result;
}

#define power_macro(type) \
	type power_##type(type base, int exp) { \
		    type result = 1; \
		    for (int i = 0; i < exp; i++) { \
			result *= base; \
		    } \
		    return result; \
	}

power_macro(float)
// Реализация функции синуса с использованием ряда Тейлора
float sin_float(float x) {
	const int terms = 7;  // Количество членов ряда для приближения
	float result = 0.0f;

	for (int n = 0; n < terms; n++) {
		int sign = (n % 2 == 0) ? 1 : -1;
		result += sign * power_float(x, 2 * n + 1) / factorial(2 * n + 1);
	}
	return result;
}
#endif // CUSTOM_MATH_H
