#include "convolution_algorithms.h"
#include <algorithm>

QVector<double> ConvolutionAlgorithms::linear(const QVector<double>& s, const QVector<double>& h) {
    if (s.isEmpty() || h.isEmpty()) {
        return {};
    }

    int N = s.size();
    int M = h.size();
    int resultSize = N + M - 1;

    if (resultSize <= 0) {
        return {};
    }

    QVector<double> result(resultSize, 0.0);

    for (int n = 0; n < resultSize; ++n) {
        for (int k = 0; k < M; ++k) {
            int idx = n - k;
            if (idx >= 0 && idx < N) {
                result[n] += s[idx] * h[k];
            }
        }
    }
    return result;
}

QVector<double> ConvolutionAlgorithms::circular(const QVector<double>& s, const QVector<double>& h) {
    if (s.isEmpty() || h.isEmpty()) {
        return {};
    }

    if (s.size() != h.size()) {
        return {};
    }

    int N = s.size();
    QVector<double> result(N, 0.0);

    for (int n = 0; n < N; ++n) {
        for (int k = 0; k < N; ++k) {
            int idx = (n - k + N) % N;
            if (idx >= 0 && idx < N && k < h.size()) {
                result[n] += s[idx] * h[k];
            }
        }
    }
    return result;
}

QVector<double> ConvolutionAlgorithms::overlapAdd(const QVector<double>& s, const QVector<double>& h, int blockSize) {
    if (s.isEmpty() || h.isEmpty() || blockSize <= 0) {
        return {};
    }

    int N = s.size();
    int M = h.size();
    int resultSize = N + M - 1;

    if (resultSize <= 0) {
        return {};
    }

    QVector<double> result(resultSize, 0.0);

    int pos = 0;
    while (pos < N) {
        int currentSize = std::min(blockSize, N - pos);
        if (currentSize <= 0) break;

        QVector<double> block(currentSize);
        for (int i = 0; i < currentSize; ++i) {
            if (pos + i < N) {
                block[i] = s[pos + i];
            }
        }

        QVector<double> conv = linear(block, h);

        for (int i = 0; i < conv.size(); ++i) {
            if (pos + i < result.size()) {
                result[pos + i] += conv[i];
            }
        }
        pos += currentSize;
    }
    return result;
}

QVector<double> ConvolutionAlgorithms::overlapSave(const QVector<double>& s, const QVector<double>& h, int blockSize) {
    if (s.isEmpty() || h.isEmpty() || blockSize <= 0) {
        return {};
    }

    const QVector<double>* signal;
    const QVector<double>* filter;

    if (s.size() >= h.size()) {
        signal = &s;
        filter = &h;
    }
    else {
        signal = &h;
        filter = &s;
    }

    int N = signal->size();
    int M = filter->size();
    int L = blockSize;

    int resultSize = N + M - 1;
    if (resultSize <= 0) {
        return {};
    }

    QVector<double> result(resultSize, 0.0);

    QVector<double> padded(M - 1 + N, 0.0);
    for (int i = 0; i < N; ++i) {
        padded[M - 1 + i] = (*signal)[i];
    }

    int pos = 0;
    int outputPos = 0;

    while (pos < padded.size()) {
        int blockLength = L + M - 1;
        QVector<double> block(blockLength, 0.0);

        for (int i = 0; i < blockLength; ++i) {
            if (pos + i < padded.size()) {
                block[i] = padded[pos + i];
            }
        }

        QVector<double> conv = linear(block, *filter);
        int validStart = M - 1;
        int validCount = qMin(L, conv.size() - validStart);

        for (int i = 0; i < validCount; ++i) {
            if (outputPos < result.size()) {
                result[outputPos++] = conv[validStart + i];
            }
        }

        pos += L;

        if (pos >= padded.size() && outputPos >= resultSize) {
            break;
        }
    }

    return result;
}