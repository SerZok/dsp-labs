#include "fft_algorithms.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

bool FftAlgorithms::isPowerOfTwo(int n) {
    return n > 0 && (n & (n - 1)) == 0;
}

QVector<double> FftAlgorithms::padToPowerOfTwo(const QVector<double>& signal) {
    int n = signal.size();
    if (isPowerOfTwo(n)) return signal;

    int paddedSize = 1;
    while (paddedSize < n) paddedSize <<= 1;

    QVector<double> padded(paddedSize, 0.0);
    for (int i = 0; i < n; ++i) {
        padded[i] = signal[i];
    }
    return padded;
}

// Вспомогательная функция: рекурсивное БПФ (алгоритм Кули-Тьюки)
static void fftRecursive(QVector<std::complex<double>>& x) {
    int N = x.size();
    if (N <= 1) return;

    // Разделяем на чётные и нечётные
    QVector<std::complex<double>> even(N / 2), odd(N / 2);
    for (int i = 0; i < N / 2; ++i) {
        even[i] = x[2 * i];
        odd[i] = x[2 * i + 1];
    }

    // Рекурсивно вызываем для каждой половины
    fftRecursive(even);
    fftRecursive(odd);

    // Объединяем (операция "бабочка")
    for (int k = 0; k < N / 2; ++k) {
        // Поворотный множитель: exp(-2πi * k / N)
        double angle = -2.0 * M_PI * k / N;
        std::complex<double> twiddle = std::polar(1.0, angle) * odd[k];

        x[k] = even[k] + twiddle;
        x[k + N / 2] = even[k] - twiddle;
    }
}

FftResult FftAlgorithms::directFft(const QVector<double>& signal) {
    FftResult result;

    if (signal.isEmpty()) {
        result.N = 0;
        return result;
    }

    // Дополняем до степени двойки, если нужно
    QVector<double> padded = padToPowerOfTwo(signal);
    int N = padded.size();
    result.N = N;

    // Преобразуем в комплексный вектор
    QVector<std::complex<double>> complexSignal(N);
    for (int i = 0; i < N; ++i) {
        complexSignal[i] = std::complex<double>(padded[i], 0.0);
    }

    // Выполняем БПФ
    fftRecursive(complexSignal);
    result.spectrum = complexSignal;

    // Формируем ось частот (индексы k от 0 до N-1)
    result.frequencyAxis.resize(N);
    for (int i = 0; i < N; ++i) {
        result.frequencyAxis[i] = i;
    }

    return result;
}