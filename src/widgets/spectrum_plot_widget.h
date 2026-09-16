#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include "qcustomplot.h"
#include "labs/lab2/fft_algorithms.h"

class SpectrumPlotWidget : public QWidget {
    Q_OBJECT
public:
    explicit SpectrumPlotWidget(QWidget* parent = nullptr);

    // Показать сигнал: аналоговая кривая + дискретные отсчёты
    void plotTimeDomain(const QVector<double>& x, const QVector<double>& y,
        const QString& title,
        int analogResolution = 1000);  // Точки для плавной кривой

    // Показать спектр (амплитудный и фазовый)
    void plotSpectrum(const FftResult& fftResult, double sampleRate = 1.0);

    // Очистить оба графика
    void clear();

public slots:
    void autoScaleTimeDomain();
    void autoScaleSpectrum();

private:
    QCustomPlot* timePlot_;
    QCustomPlot* magnitudePlot_;
    QCustomPlot* phasePlot_;

    // Генерация "аналоговой" версии сигнала (интерполяция)
    QVector<double> generateAnalogSignal(const QVector<double>& x,
        const QVector<double>& y,
        int resolution);
};