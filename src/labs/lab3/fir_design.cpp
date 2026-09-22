#include "fir_design.h"
#include <cmath>
#include <algorithm>
#include <complex>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

QVector<double> FirDesign::designFir(const FilterSpec& spec) {
    int N = spec.order + 1;
    if (N <= 0) return {};

    double M = (N - 1) / 2.0;
    QVector<double> hIdeal(N);

    // Нормированные частоты (рад/отсчёт)
    double wc = 2.0 * M_PI * spec.cutoffFreq / spec.sampleRate;
    double wc1 = 2.0 * M_PI * spec.cutoffFreq1 / spec.sampleRate;
    double wc2 = 2.0 * M_PI * spec.cutoffFreq2 / spec.sampleRate;

    // Идеальная импульсная характеристика
    for (int n = 0; n < N; ++n) {
        double nm = n - M;

        if (std::abs(nm) < 1e-10) {
            // Центр (n = M)
            switch (spec.type) {
            case FilterType::LowPass:
                hIdeal[n] = wc / M_PI;
                break;
            case FilterType::HighPass:
                hIdeal[n] = 1.0 - wc / M_PI;
                break;
            case FilterType::BandPass:
                hIdeal[n] = (wc2 - wc1) / M_PI;
                break;
            case FilterType::BandStop:
                hIdeal[n] = 1.0 - (wc2 - wc1) / M_PI;
                break;
            }
        }
        else {
            switch (spec.type) {
            case FilterType::LowPass:
                hIdeal[n] = std::sin(wc * nm) / (M_PI * nm);
                break;
            case FilterType::HighPass:
                hIdeal[n] = -std::sin(wc * nm) / (M_PI * nm);
                break;
            case FilterType::BandPass:
                hIdeal[n] = (std::sin(wc2 * nm) - std::sin(wc1 * nm)) / (M_PI * nm);
                break;
            case FilterType::BandStop:
                hIdeal[n] = -(std::sin(wc2 * nm) - std::sin(wc1 * nm)) / (M_PI * nm);
                break;
            }
        }
    }

    // Для ФВЧ и ПЗФ добавляем δ[n-M]
    if (spec.type == FilterType::HighPass || spec.type == FilterType::BandStop) {
        int center = (int)std::round(M);
        if (center >= 0 && center < N) {
            hIdeal[center] += 1.0;
        }
    }

    // Умножаем на окно
    QVector<double> window = FirWindows::generateWindow(spec.window, N);
    QVector<double> h(N);
    for (int n = 0; n < N; ++n) {
        h[n] = hIdeal[n] * window[n];
    }

    return h;
}

QVector<double> FirDesign::magnitudeResponse(const QVector<double>& h, int numPoints) {
    QVector<double> mag(numPoints);
    int N = h.size();

    for (int k = 0; k < numPoints; ++k) {
        double omega = M_PI * k / (numPoints - 1);
        double re = 0.0, im = 0.0;
        for (int n = 0; n < N; ++n) {
            re += h[n] * std::cos(omega * n);
            im -= h[n] * std::sin(omega * n);
        }
        mag[k] = std::sqrt(re * re + im * im);
    }
    return mag;
}

QVector<double> FirDesign::logMagnitudeResponse(const QVector<double>& h, int numPoints) {
    QVector<double> mag = magnitudeResponse(h, numPoints);
    QVector<double> logMag(numPoints);
    for (int k = 0; k < numPoints; ++k) {
        // Защита от log(0)
        double val = std::max(mag[k], 1e-10);
        logMag[k] = 20.0 * std::log10(val);
    }
    return logMag;
}

QVector<double> FirDesign::phaseResponse(const QVector<double>& h, int numPoints) {
    QVector<double> phase(numPoints);
    int N = h.size();

    for (int k = 0; k < numPoints; ++k) {
        double omega = M_PI * k / (numPoints - 1);
        double re = 0.0, im = 0.0;
        for (int n = 0; n < N; ++n) {
            re += h[n] * std::cos(omega * n);
            im -= h[n] * std::sin(omega * n);
        }
        phase[k] = std::atan2(im, re);
    }
    return phase;
}

QVector<double> FirDesign::frequencyAxisHz(int numPoints, double sampleRate) {
    QVector<double> freq(numPoints);
    for (int k = 0; k < numPoints; ++k) {
        freq[k] = (sampleRate / 2.0) * k / (numPoints - 1);
    }
    return freq;
}

QVector<double> FirDesign::normalizedFrequencyAxis(int numPoints) {
    QVector<double> freq(numPoints);
    for (int k = 0; k < numPoints; ++k) {
        freq[k] = M_PI * k / (numPoints - 1);
    }
    return freq;
}

QVector<double> FirDesign::designRaisedCosine(double sampleRate, double cutoffFreq,
    double rollOff, int order) {
    int N = order + 1;
    double M = (N - 1) / 2.0;
    QVector<double> h(N);

    double fcFs = cutoffFreq / sampleRate;

    for (int n = 0; n < N; ++n) {
        double nm = n - M;
        double t = nm / sampleRate;

        if (std::abs(nm) < 1e-10) {
            // Центр
            h[n] = 2.0 * fcFs * (1.0 - rollOff + 4.0 * rollOff / M_PI);
        }
        else {
            double sincArg = M_PI * 2.0 * fcFs * t;
            double cosArg = M_PI * rollOff * 2.0 * fcFs * t;
            double denomArg = 4.0 * rollOff * 2.0 * fcFs * t;

            double sincVal = std::sin(sincArg) / sincArg;
            double cosVal = std::cos(cosArg);
            double denom = 1.0 - denomArg * denomArg;

            if (std::abs(denom) < 1e-10) {
                // Особый случай — используем предел
                h[n] = fcFs * sincVal * (M_PI / 4.0);
            }
            else {
                h[n] = 2.0 * fcFs * sincVal * cosVal / denom;
            }
        }
    }

    return h;
}

FirDesign::WindowCharacteristics FirDesign::analyzeWindow(FirWindows::WindowType type, int N) {
    WindowCharacteristics result{ 0, 0, 0 };

    // 1. Считаем ДПФ окна большой длины для анализа спектра
    int fftSize = 4096;
    QVector<double> window = FirWindows::generateWindow(type, N);

    // Дополняем окно нулями до fftSize
    QVector<double> padded(fftSize, 0.0);
    int offset = (fftSize - N) / 2;
    for (int i = 0; i < N; ++i) {
        padded[offset + i] = window[i];
    }

    // Считаем ДПФ
    QVector<double> mag(fftSize / 2);
    for (int k = 0; k < fftSize / 2; ++k) {
        double omega = 2.0 * M_PI * k / fftSize;
        double re = 0.0, im = 0.0;
        for (int n = 0; n < fftSize; ++n) {
            re += padded[n] * std::cos(omega * n);
            im -= padded[n] * std::sin(omega * n);
        }
        mag[k] = std::sqrt(re * re + im * im);
    }

    // Нормируем на максимум
    double maxVal = *std::max_element(mag.begin(), mag.end());
    if (maxVal > 0) {
        for (auto& v : mag) v /= maxVal;
    }

    // 2. Ширина главного лепестка: расстояние между первыми нулями
    // Ищем первый минимум после максимума
    int peakIdx = 0;
    for (int k = 1; k < fftSize / 2; ++k) {
        if (mag[k] > mag[peakIdx]) peakIdx = k;
    }

    // Ищем первый нуль (минимум) справа от пика
    int firstZero = peakIdx;
    for (int k = peakIdx + 1; k < fftSize / 2 - 1; ++k) {
        if (mag[k] < mag[k - 1] && mag[k] < mag[k + 1] && mag[k] < 0.1) {
            firstZero = k;
            break;
        }
    }

    // Ширина главного лепестка (в нормированной частоте)
    result.mainLobeWidth = 2.0 * 2.0 * M_PI * (firstZero - peakIdx) / fftSize;

    // 3. Амплитуда первого бокового лепестка
    // Ищем максимум между первым и вторым нулями
    int secondZero = firstZero;
    for (int k = firstZero + 1; k < fftSize / 2 - 1; ++k) {
        if (mag[k] < mag[k - 1] && mag[k] < mag[k + 1] && mag[k] < 0.1) {
            secondZero = k;
            break;
        }
    }

    double maxSideLobe = 0.0;
    for (int k = firstZero; k <= secondZero && k < fftSize / 2; ++k) {
        if (mag[k] > maxSideLobe) maxSideLobe = mag[k];
    }

    result.firstSideLobeDb = (maxSideLobe > 0) ? 20.0 * std::log10(maxSideLobe) : -200.0;

    // 4. Минимальное затухание в полосе непропускания тестового ФНЧ
    FilterSpec testSpec;
    testSpec.type = FilterType::LowPass;
    testSpec.sampleRate = 1000.0;
    testSpec.cutoffFreq = 100.0;
    testSpec.cutoffFreq1 = 0;
    testSpec.cutoffFreq2 = 0;
    testSpec.window = type;
    testSpec.order = N - 1;

    QVector<double> testH = designFir(testSpec);
    QVector<double> testMag = magnitudeResponse(testH, 2048);

    // Полоса непропускания — от 150 Гц до 500 Гц (примерно)
    // Индексы: 150/500 * 2048 = 614, 500/500 * 2048 = 2048
    int stopStart = (int)(150.0 / 500.0 * 2047);
    int stopEnd = 2047;

    double minStop = 1e10;
    for (int k = stopStart; k < stopEnd; ++k) {
        if (testMag[k] < minStop) minStop = testMag[k];
    }

    result.minStopbandDb = (minStop > 0) ? 20.0 * std::log10(minStop) : -200.0;

    return result;
}