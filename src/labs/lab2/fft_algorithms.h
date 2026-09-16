#pragma once
#include <QVector>
#include <complex>

struct FftResult {
    QVector<std::complex<double>> spectrum;
    QVector<double> frequencyAxis;
    int N;

    QVector<double> magnitude() const {
        QVector<double> mag(N);
        for (int i = 0; i < N; ++i) {
            mag[i] = std::abs(spectrum[i]);
        }
        return mag;
    }

    QVector<double> phase() const {
        QVector<double> ph(N);
        for (int i = 0; i < N; ++i) {
            ph[i] = std::arg(spectrum[i]);
        }
        return ph;
    }

    QVector<double> normalizedMagnitude() const {
        QVector<double> mag = magnitude();
        for (int i = 0; i < N; ++i) {
            mag[i] /= N;
        }
        for (int i = 1; i < N / 2; ++i) {
            mag[i] *= 2.0;
        }
        return mag;
    }
};

class FftAlgorithms {
public:
    static FftResult directFft(const QVector<double>& signal);
    static bool isPowerOfTwo(int n);
    static QVector<double> padToPowerOfTwo(const QVector<double>& signal);
};