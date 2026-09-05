#pragma once
#include <QVector>
#include <algorithm>

class ConvolutionAlgorithms {
public:
    static QVector<double> linear(const QVector<double>& s, const QVector<double>& h);
    static QVector<double> circular(const QVector<double>& s, const QVector<double>& h);
    static QVector<double> overlapAdd(const QVector<double>& s, const QVector<double>& h, int blockSize);
    static QVector<double> overlapSave(const QVector<double>& s, const QVector<double>& h, int blockSize);
};