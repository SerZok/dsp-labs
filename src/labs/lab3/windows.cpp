#include "windows.h"
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace FirWindows {

    // Модифицированная функция Бесселя I0(x) — нужна для окна Кайзера и Бомена
    static double besselI0(double x) {
        double sum = 1.0;
        double term = 1.0;
        double xHalfSq = (x / 2.0) * (x / 2.0);
        for (int k = 1; k <= 25; ++k) {
            term *= xHalfSq / (double)(k * k);
            sum += term;
            if (std::abs(term) < 1e-15) break;
        }
        return sum;
    }

    QString windowName(WindowType type) {
        switch (type) {
        case WindowType::Rectangular:    return "Прямоугольное";
        case WindowType::Triangular:     return "Треугольное";
        case WindowType::Bartlett:       return "Бартлетта";
        case WindowType::Hann:           return "Ханна";
        case WindowType::Hamming:        return "Хемминга";
        case WindowType::Blackman:       return "Блекмена";
        case WindowType::Kaiser4:        return "Кайзера (β=4)";
        case WindowType::Kaiser9:        return "Кайзера (β=9)";
        case WindowType::Chebyshev40:    return "Чебышева (β=40дБ)";
        case WindowType::Chebyshev90:    return "Чебышева (β=90дБ)";
        case WindowType::BartlettHann:   return "Бартлетта-Ханна";
        case WindowType::BlackmanHarris: return "Блекмена-Харриса";
        case WindowType::Bohman:         return "Бомена";
        case WindowType::Gauss:          return "Гаусса";
        case WindowType::Nuttall:        return "Nuttall (мод. Блекмена-Харриса)";
        case WindowType::Tukey:          return "Тьюки";
        }
        return "Unknown";
    }

    QList<WindowType> allWindows() {
        return {
            WindowType::Rectangular, WindowType::Triangular, WindowType::Bartlett,
            WindowType::Hann, WindowType::Hamming, WindowType::Blackman,
            WindowType::Kaiser4, WindowType::Kaiser9,
            WindowType::Chebyshev40, WindowType::Chebyshev90,
            WindowType::BartlettHann, WindowType::BlackmanHarris,
            WindowType::Bohman, WindowType::Gauss,
            WindowType::Nuttall, WindowType::Tukey
        };
    }

    QVector<double> generateWindow(WindowType type, int N) {
        if (N <= 0) return {};
        if (N == 1) return { 1.0 };

        QVector<double> w(N);
        double M = (N - 1) / 2.0;

        switch (type) {
        case WindowType::Rectangular: {
            for (int n = 0; n < N; ++n) w[n] = 1.0;
            break;
        }
        case WindowType::Triangular: {
            // Не даёт нулей на концах
            for (int n = 0; n < N; ++n) {
                w[n] = 1.0 - std::abs((n - M) / (N / 2.0));
            }
            break;
        }
        case WindowType::Bartlett: {
            // Даёт нули на концах
            for (int n = 0; n < N; ++n) {
                w[n] = 1.0 - std::abs((n - M) / M);
            }
            break;
        }
        case WindowType::Hann: {
            for (int n = 0; n < N; ++n) {
                w[n] = 0.5 - 0.5 * std::cos(2.0 * M_PI * n / (N - 1));
            }
            break;
        }
        case WindowType::Hamming: {
            for (int n = 0; n < N; ++n) {
                w[n] = 0.54 - 0.46 * std::cos(2.0 * M_PI * n / (N - 1));
            }
            break;
        }
        case WindowType::Blackman: {
            for (int n = 0; n < N; ++n) {
                w[n] = 0.42 - 0.5 * std::cos(2.0 * M_PI * n / (N - 1))
                    + 0.08 * std::cos(4.0 * M_PI * n / (N - 1));
            }
            break;
        }
        case WindowType::Kaiser4:
        case WindowType::Kaiser9: {
            double beta = (type == WindowType::Kaiser4) ? 4.0 : 9.0;
            double i0beta = besselI0(beta);
            for (int n = 0; n < N; ++n) {
                double x = (n - M) / M;
                double arg = beta * std::sqrt(std::max(0.0, 1.0 - x * x));
                w[n] = besselI0(arg) / i0beta;
            }
            break;
        }
        case WindowType::Chebyshev40:
        case WindowType::Chebyshev90: {
            // Упрощённая реализация через Кайзера с эквивалентным β
            // Chebyshev β=40дБ ≈ Kaiser β≈3.8
            // Chebyshev β=90дБ ≈ Kaiser β≈9.0
            double beta = (type == WindowType::Chebyshev40) ? 3.8 : 9.0;
            double i0beta = besselI0(beta);
            for (int n = 0; n < N; ++n) {
                double x = (n - M) / M;
                double arg = beta * std::sqrt(std::max(0.0, 1.0 - x * x));
                w[n] = besselI0(arg) / i0beta;
            }
            break;
        }
        case WindowType::BartlettHann: {
            for (int n = 0; n < N; ++n) {
                double x = (double)n / (N - 1) - 0.5;
                w[n] = 0.62 - 0.48 * std::abs(x) + 0.38 * std::cos(2.0 * M_PI * x);
            }
            break;
        }
        case WindowType::BlackmanHarris: {
            const double a0 = 0.35875, a1 = 0.48829, a2 = 0.14128, a3 = 0.01168;
            for (int n = 0; n < N; ++n) {
                w[n] = a0
                    - a1 * std::cos(2.0 * M_PI * n / (N - 1))
                    + a2 * std::cos(4.0 * M_PI * n / (N - 1))
                    - a3 * std::cos(6.0 * M_PI * n / (N - 1));
            }
            break;
        }
        case WindowType::Bohman: {
            for (int n = 0; n < N; ++n) {
                double x = std::abs(2.0 * n / (N - 1) - 1.0);
                if (x >= 1.0) {
                    w[n] = 0.0;
                }
                else {
                    w[n] = (1.0 - x) * std::cos(M_PI * x) + (1.0 / M_PI) * std::sin(M_PI * x);
                }
            }
            break;
        }
        case WindowType::Gauss: {
            const double alpha = 2.5;  // Стандартное значение
            double sigma = alpha / M;
            for (int n = 0; n < N; ++n) {
                double x = (n - M) * sigma;
                w[n] = std::exp(-0.5 * x * x);
            }
            break;
        }
        case WindowType::Nuttall: {
            const double a0 = 0.355768, a1 = 0.487396, a2 = 0.144232, a3 = 0.012604;
            for (int n = 0; n < N; ++n) {
                w[n] = a0
                    - a1 * std::cos(2.0 * M_PI * n / (N - 1))
                    + a2 * std::cos(4.0 * M_PI * n / (N - 1))
                    - a3 * std::cos(6.0 * M_PI * n / (N - 1));
            }
            break;
        }
        case WindowType::Tukey: {
            const double r = 0.5;  // Доля косинусной части
            for (int n = 0; n < N; ++n) {
                double x = (double)n / (N - 1);
                if (x < r / 2.0) {
                    w[n] = 0.5 * (1.0 + std::cos(2.0 * M_PI / r * (x - r / 2.0)));
                }
                else if (x > 1.0 - r / 2.0) {
                    w[n] = 0.5 * (1.0 + std::cos(2.0 * M_PI / r * (x - 1.0 + r / 2.0)));
                }
                else {
                    w[n] = 1.0;
                }
            }
            break;
        }
        }

        return w;
    }

} // namespace FirWindows