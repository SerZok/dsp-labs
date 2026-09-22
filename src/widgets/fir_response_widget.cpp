#include "fir_response_widget.h"
#include "labs/lab3/fir_design.h"

#include <QSplitter>
#include <algorithm>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

FirResponseWidget::FirResponseWidget(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    auto* splitter = new QSplitter(Qt::Vertical);

    // 1. Импульсная характеристика
    impulsePlot_ = new QCustomPlot;
    impulsePlot_->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    impulsePlot_->xAxis->setLabel("n (отсчёты)");
    impulsePlot_->yAxis->setLabel("h[n]");
    impulsePlot_->setMinimumHeight(120);
    impulsePlot_->legend->setVisible(false);

    // 2. АЧХ (линейная)
    magnitudePlot_ = new QCustomPlot;
    magnitudePlot_->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    magnitudePlot_->xAxis->setLabel("f (Гц)");
    magnitudePlot_->yAxis->setLabel("|H(f)|");
    magnitudePlot_->setMinimumHeight(120);
    magnitudePlot_->legend->setVisible(true);

    // 3. ЛАЧХ (дБ)
    logMagPlot_ = new QCustomPlot;
    logMagPlot_->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    logMagPlot_->xAxis->setLabel("f (Гц)");
    logMagPlot_->yAxis->setLabel("|H(f)|, дБ");
    logMagPlot_->setMinimumHeight(120);
    logMagPlot_->legend->setVisible(true);

    // 4. ФЧХ
    phasePlot_ = new QCustomPlot;
    phasePlot_->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    phasePlot_->xAxis->setLabel("f (Гц)");
    phasePlot_->yAxis->setLabel("arg(H(f)), рад");
    phasePlot_->setMinimumHeight(120);
    phasePlot_->legend->setVisible(false);

    splitter->addWidget(impulsePlot_);
    splitter->addWidget(magnitudePlot_);
    splitter->addWidget(logMagPlot_);
    splitter->addWidget(phasePlot_);

    layout->addWidget(splitter);

    // Двойной клик — авто-масштаб
    connect(impulsePlot_, &QCustomPlot::mouseDoubleClick, this, &FirResponseWidget::autoScaleAll);
    connect(magnitudePlot_, &QCustomPlot::mouseDoubleClick, this, &FirResponseWidget::autoScaleAll);
    connect(logMagPlot_, &QCustomPlot::mouseDoubleClick, this, &FirResponseWidget::autoScaleAll);
    connect(phasePlot_, &QCustomPlot::mouseDoubleClick, this, &FirResponseWidget::autoScaleAll);
}

void FirResponseWidget::plotImpulseResponse(const QVector<double>& h, const QString& title) {
    Q_UNUSED(title);
    impulsePlot_->clearGraphs();

    QVector<double> x(h.size());
    for (int i = 0; i < h.size(); ++i) x[i] = i;

    QCPGraph* graph = impulsePlot_->addGraph();
    graph->setData(x, h);
    graph->setLineStyle(QCPGraph::lsImpulse);
    graph->setPen(QPen(QColor(0, 114, 189), 1.5));
    graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,
        QColor(0, 114, 189), QColor(0, 114, 189), 4));

    impulsePlot_->xAxis->setRange(-1, h.size());
    impulsePlot_->rescaleAxes();

    QSharedPointer<QCPAxisTickerFixed> ticker(new QCPAxisTickerFixed);
    int step = qMax(1, h.size() / 20);
    ticker->setTickStep(step);
    impulsePlot_->xAxis->setTicker(ticker);

    impulsePlot_->replot();
}

void FirResponseWidget::plotMagnitudeResponse(const QVector<double>& freq,
    const QVector<double>& mag,
    const QString& title) {
    magnitudePlot_->clearGraphs();

    QCPGraph* graph = magnitudePlot_->addGraph();
    graph->setData(freq, mag);
    graph->setPen(QPen(QColor(217, 83, 25), 1.5));
    graph->setName(title);

    magnitudePlot_->xAxis->setRange(freq.first(), freq.last());
    magnitudePlot_->yAxis->setRange(0, 1.1);

    magnitudePlot_->replot();
}

void FirResponseWidget::plotLogMagnitudeResponse(const QVector<double>& freq,
    const QVector<double>& logMag,
    const QString& title) {
    logMagPlot_->clearGraphs();

    QCPGraph* graph = logMagPlot_->addGraph();
    graph->setData(freq, logMag);
    graph->setPen(QPen(QColor(217, 83, 25), 1.5));
    graph->setName(title);

    logMagPlot_->xAxis->setRange(freq.first(), freq.last());

    // Находим минимум для установки диапазона
    double minVal = *std::min_element(logMag.begin(), logMag.end());
    logMagPlot_->yAxis->setRange(qMin(minVal - 10.0, -100.0), 10.0);

    logMagPlot_->replot();
}

void FirResponseWidget::plotPhaseResponse(const QVector<double>& freq,
    const QVector<double>& phase,
    const QString& title) {
    Q_UNUSED(title);
    phasePlot_->clearGraphs();

    QCPGraph* graph = phasePlot_->addGraph();
    graph->setData(freq, phase);
    graph->setPen(QPen(QColor(126, 47, 142), 1.5));

    phasePlot_->xAxis->setRange(freq.first(), freq.last());
    phasePlot_->yAxis->setRange(-M_PI - 0.3, M_PI + 0.3);

    phasePlot_->replot();
}

void FirResponseWidget::plotAll(const QVector<double>& h, double sampleRate, const QString& title) {
    int numPoints = 2048;

    QVector<double> freq = FirDesign::frequencyAxisHz(numPoints, sampleRate);
    QVector<double> mag = FirDesign::magnitudeResponse(h, numPoints);
    QVector<double> logMag = FirDesign::logMagnitudeResponse(h, numPoints);
    QVector<double> phase = FirDesign::phaseResponse(h, numPoints);

    plotImpulseResponse(h, title);
    plotMagnitudeResponse(freq, mag, title);
    plotLogMagnitudeResponse(freq, logMag, title);
    plotPhaseResponse(freq, phase, title);
}

void FirResponseWidget::addMagnitudeCurve(const QVector<double>& freq,
    const QVector<double>& mag,
    const QString& name, const QColor& color) {
    QCPGraph* graph = magnitudePlot_->addGraph();
    graph->setData(freq, mag);
    graph->setPen(QPen(color, 1.5));
    graph->setName(name);
    magnitudePlot_->replot();
}

void FirResponseWidget::addLogMagnitudeCurve(const QVector<double>& freq,
    const QVector<double>& logMag,
    const QString& name, const QColor& color) {
    QCPGraph* graph = logMagPlot_->addGraph();
    graph->setData(freq, logMag);
    graph->setPen(QPen(color, 1.5));
    graph->setName(name);
    logMagPlot_->replot();
}

void FirResponseWidget::clear() {
    impulsePlot_->clearGraphs();
    impulsePlot_->replot();
    magnitudePlot_->clearGraphs();
    magnitudePlot_->replot();
    logMagPlot_->clearGraphs();
    logMagPlot_->replot();
    phasePlot_->clearGraphs();
    phasePlot_->replot();
}

void FirResponseWidget::autoScaleAll() {
    // === 1. Импульсная характеристика ===
    impulsePlot_->rescaleAxes();
    {
        // Отступы по X: 5% с каждой стороны
        double xRange = impulsePlot_->xAxis->range().upper - impulsePlot_->xAxis->range().lower;
        double xPadding = xRange * 0.05;
        impulsePlot_->xAxis->setRange(
            impulsePlot_->xAxis->range().lower - xPadding,
            impulsePlot_->xAxis->range().upper + xPadding
        );

        // Отступы по Y: 10% сверху и снизу
        double yRange = impulsePlot_->yAxis->range().upper - impulsePlot_->yAxis->range().lower;
        double yPadding = yRange * 0.1;
        impulsePlot_->yAxis->setRange(
            impulsePlot_->yAxis->range().lower - yPadding,
            impulsePlot_->yAxis->range().upper + yPadding
        );
    }
    impulsePlot_->replot();

    // === 2. АЧХ (линейная) ===
    magnitudePlot_->rescaleAxes();
    {
        double xRange = magnitudePlot_->xAxis->range().upper - magnitudePlot_->xAxis->range().lower;
        double xPadding = xRange * 0.05;
        magnitudePlot_->xAxis->setRange(
            magnitudePlot_->xAxis->range().lower - xPadding,
            magnitudePlot_->xAxis->range().upper + xPadding
        );

        // Y: от 0 до максимума + 10%
        magnitudePlot_->yAxis->setRange(0, magnitudePlot_->yAxis->range().upper * 1.1);
    }
    magnitudePlot_->replot();

    // === 3. ЛАЧХ (дБ) ===
    logMagPlot_->rescaleAxes();
    {
        double xRange = logMagPlot_->xAxis->range().upper - logMagPlot_->xAxis->range().lower;
        double xPadding = xRange * 0.05;
        logMagPlot_->xAxis->setRange(
            logMagPlot_->xAxis->range().lower - xPadding,
            logMagPlot_->xAxis->range().upper + xPadding
        );

        // Y: минимум не выше -100 дБ, максимум + 5 дБ
        double yLower = qMin(logMagPlot_->yAxis->range().lower, -100.0);
        double yUpper = logMagPlot_->yAxis->range().upper + 5.0;
        logMagPlot_->yAxis->setRange(yLower, yUpper);
    }
    logMagPlot_->replot();

    // === 4. ФЧХ ===
    phasePlot_->rescaleAxes();
    {
        double xRange = phasePlot_->xAxis->range().upper - phasePlot_->xAxis->range().lower;
        double xPadding = xRange * 0.05;
        phasePlot_->xAxis->setRange(
            phasePlot_->xAxis->range().lower - xPadding,
            phasePlot_->xAxis->range().upper + xPadding
        );

        // Y: от -π до +π с небольшим запасом
        phasePlot_->yAxis->setRange(-M_PI - 0.3, M_PI + 0.3);
    }
    phasePlot_->replot();
}