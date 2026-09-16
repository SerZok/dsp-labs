#include "spectrum_plot_widget.h"
#include <QSplitter>
#include <algorithm>
#include <cmath>

SpectrumPlotWidget::SpectrumPlotWidget(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    auto* splitter = new QSplitter(Qt::Vertical);

    // 1. График во временной области
    timePlot_ = new QCustomPlot;
    timePlot_->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    timePlot_->xAxis->setLabel("x (время/аргумент)");
    timePlot_->yAxis->setLabel("Амплитуда");
    timePlot_->setMinimumHeight(200);
    timePlot_->legend->setVisible(true);
    connect(timePlot_, &QCustomPlot::mouseDoubleClick, this, &SpectrumPlotWidget::autoScaleTimeDomain);

    // 2. Амплитудный спектр
    magnitudePlot_ = new QCustomPlot;
    magnitudePlot_->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    magnitudePlot_->xAxis->setLabel("k (частотный индекс)");
    magnitudePlot_->yAxis->setLabel("|X[k]| (амплитуда)");
    magnitudePlot_->setMinimumHeight(150);
    connect(magnitudePlot_, &QCustomPlot::mouseDoubleClick, this, &SpectrumPlotWidget::autoScaleSpectrum);

    // 3. Фазовый спектр
    phasePlot_ = new QCustomPlot;
    phasePlot_->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    phasePlot_->xAxis->setLabel("k (частотный индекс)");
    phasePlot_->yAxis->setLabel("arg(X[k]) (фаза, рад)");
    phasePlot_->setMinimumHeight(150);
    connect(phasePlot_, &QCustomPlot::mouseDoubleClick, this, &SpectrumPlotWidget::autoScaleSpectrum);

    splitter->addWidget(timePlot_);
    splitter->addWidget(magnitudePlot_);
    splitter->addWidget(phasePlot_);

    layout->addWidget(splitter);
}

QVector<double> SpectrumPlotWidget::generateAnalogSignal(
    const QVector<double>& x, const QVector<double>& y, int resolution)
{
    if (x.size() < 2 || y.size() < 2) return y;

    // Простая линейная интерполяция для плавной кривой
    // (для синусоид можно использовать сплайны, но линейной достаточно)
    QVector<double> analogY(resolution);

    double xMin = x.first();
    double xMax = x.last();
    double step = (xMax - xMin) / (resolution - 1);

    for (int i = 0; i < resolution; ++i) {
        double xInterp = xMin + i * step;

        // Находим интервал [x[j], x[j+1]], в который попадает xInterp
        int j = 0;
        while (j < x.size() - 2 && x[j + 1] < xInterp) {
            ++j;
        }

        // Линейная интерполяция
        double t = (xInterp - x[j]) / (x[j + 1] - x[j]);
        analogY[i] = y[j] + t * (y[j + 1] - y[j]);
    }

    return analogY;
}

void SpectrumPlotWidget::plotTimeDomain(const QVector<double>& x, const QVector<double>& y,
    const QString& title, int analogResolution) {
    timePlot_->clearGraphs();

    if (x.isEmpty() || y.isEmpty()) {
        timePlot_->replot();
        return;
    }

    // === 1. Рисуем "аналоговую" плавную кривую ===
    QVector<double> analogX(analogResolution);
    double xMin = x.first();
    double xMax = x.last();
    for (int i = 0; i < analogResolution; ++i) {
        analogX[i] = xMin + i * (xMax - xMin) / (analogResolution - 1);
    }

    QVector<double> analogY = generateAnalogSignal(x, y, analogResolution);

    QCPGraph* analogGraph = timePlot_->addGraph();
    analogGraph->setData(analogX, analogY);
    analogGraph->setLineStyle(QCPGraph::lsLine);  // Сплошная линия
    analogGraph->setPen(QPen(QColor(0, 114, 189, 150), 1.5));  // Полупрозрачный синий
    analogGraph->setName("Аналоговый сигнал");

    // === 2. Рисуем дискретные отсчёты (поверх аналогового) ===
    QCPGraph* discreteGraph = timePlot_->addGraph();
    discreteGraph->setData(x, y);
    discreteGraph->setLineStyle(QCPGraph::lsNone);  // Без линий
    discreteGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,
        QColor(217, 83, 25),
        QColor(255, 255, 255),  // Белая обводка
        6));  // Размер маркера
    discreteGraph->setName("Дискретные отсчёты");

    // === 3. Вертикальные линии от оси X до точек (опционально) ===
    // Можно добавить для наглядности, как в stem plot
    QCPGraph* stemGraph = timePlot_->addGraph();
    stemGraph->setData(x, y);
    stemGraph->setLineStyle(QCPGraph::lsImpulse);
    stemGraph->setPen(QPen(QColor(217, 83, 25, 100), 1.0));  // Очень прозрачный
    stemGraph->setName("");  // Не показывать в легенде

    // Настройка осей
    timePlot_->xAxis->setRange(xMin - 0.5, xMax + 0.5);
    timePlot_->rescaleAxes();
    timePlot_->yAxis->setRangeLower(qMin(timePlot_->yAxis->range().lower, -1.5));
    timePlot_->yAxis->setRangeUpper(timePlot_->yAxis->range().upper * 1.1);

    // Тикер для целых чисел на оси X (если нужно)
    if (x.size() <= 50) {
        QSharedPointer<QCPAxisTickerFixed> ticker(new QCPAxisTickerFixed);
        ticker->setTickStep(1.0);
        timePlot_->xAxis->setTicker(ticker);
    }

    timePlot_->replot();
}

void SpectrumPlotWidget::plotSpectrum(const FftResult& fftResult, double sampleRate) {
    Q_UNUSED(sampleRate);

    if (fftResult.N == 0) return;

    // Берём только первую половину спектра (0 до N/2)
    int halfN = fftResult.N / 2 + 1;

    QVector<double> freqAxis(halfN);
    QVector<double> magnitude(halfN);
    QVector<double> phase(halfN);

    QVector<double> normMag = fftResult.normalizedMagnitude();
    QVector<double> fullPhase = fftResult.phase();

    for (int i = 0; i < halfN; ++i) {
        freqAxis[i] = fftResult.frequencyAxis[i];
        magnitude[i] = normMag[i];
        phase[i] = fullPhase[i];
    }

    // === Амплитудный спектр ===
    magnitudePlot_->clearGraphs();
    QCPGraph* magGraph = magnitudePlot_->addGraph();
    magGraph->setData(freqAxis, magnitude);
    magGraph->setLineStyle(QCPGraph::lsImpulse);
    magGraph->setPen(QPen(QColor(217, 83, 25), 1.5));
    magGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,
        QColor(217, 83, 25), QColor(217, 83, 25), 5));
    magGraph->setName("Амплитудный спектр");
    magnitudePlot_->legend->setVisible(true);

    magnitudePlot_->xAxis->setRange(-0.5, halfN - 0.5);
    magnitudePlot_->rescaleAxes();
    magnitudePlot_->yAxis->setRangeLower(0);

    QSharedPointer<QCPAxisTickerFixed> magTicker(new QCPAxisTickerFixed);
    magTicker->setTickStep(1.0);
    magnitudePlot_->xAxis->setTicker(magTicker);

    magnitudePlot_->replot();

    // === Фазовый спектр ===
    phasePlot_->clearGraphs();
    QCPGraph* phGraph = phasePlot_->addGraph();
    phGraph->setData(freqAxis, phase);
    phGraph->setLineStyle(QCPGraph::lsImpulse);
    phGraph->setPen(QPen(QColor(126, 47, 142), 1.5));
    phGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,
        QColor(126, 47, 142), QColor(126, 47, 142), 5));
    phGraph->setName("Фазовый спектр");
    phasePlot_->legend->setVisible(true);

    phasePlot_->xAxis->setRange(-0.5, halfN - 0.5);
    phasePlot_->yAxis->setRange(-M_PI - 0.3, M_PI + 0.3);

    QSharedPointer<QCPAxisTickerFixed> phTicker(new QCPAxisTickerFixed);
    phTicker->setTickStep(1.0);
    phasePlot_->xAxis->setTicker(phTicker);

    phasePlot_->replot();
}

void SpectrumPlotWidget::clear() {
    timePlot_->clearGraphs();
    timePlot_->replot();
    magnitudePlot_->clearGraphs();
    magnitudePlot_->replot();
    phasePlot_->clearGraphs();
    phasePlot_->replot();
}

void SpectrumPlotWidget::autoScaleTimeDomain() {
    timePlot_->rescaleAxes();
    timePlot_->replot();
}

void SpectrumPlotWidget::autoScaleSpectrum() {
    magnitudePlot_->rescaleAxes();
    magnitudePlot_->yAxis->setRangeLower(0);
    magnitudePlot_->replot();

    phasePlot_->rescaleAxes();
    phasePlot_->replot();
}