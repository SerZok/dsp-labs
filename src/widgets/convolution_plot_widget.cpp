#include "convolution_plot_widget.h"
#include <algorithm>

ConvolutionPlotWidget::ConvolutionPlotWidget(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    plot_ = new QCustomPlot(this);
    layout->addWidget(plot_);

    plot_->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    plot_->xAxis->setLabel("n (отсчёты)");
    plot_->yAxis->setLabel("Амплитуда");
    plot_->legend->setVisible(true);
    plot_->legend->setFont(QFont(plot_->font().family(), 9));
    plot_->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft | Qt::AlignTop);

    plot_->yAxis->setRangeLower(-1);


    connect(plot_, &QCustomPlot::mouseDoubleClick, this, &ConvolutionPlotWidget::autoScale);
}

QCPGraph* ConvolutionPlotWidget::addStemGraph(const QVector<double>& x, const QVector<double>& y,
    const QColor& color, const QString& name) {
    QCPGraph* graph = plot_->addGraph();

    graph->setLineStyle(QCPGraph::lsImpulse);
    graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, color, color, 6));

    QColor lineColor = color;
    lineColor.setAlpha(180);
    graph->setPen(QPen(lineColor, 1.5));

    graph->setName(name);
    graph->setData(x, y);

    return graph;
}

void ConvolutionPlotWidget::plotSequences(
    const QVector<double>& seq1, const QVector<double>& seq2,
    const QVector<double>& result,
    const QString& label1, const QString& label2, const QString& labelResult)
{
    plot_->clearGraphs();

    auto makeXAxis = [](const QVector<double>& seq) {
        QVector<double> x(seq.size());
        for (int i = 0; i < seq.size(); ++i) x[i] = i;
        return x;
        };

    addStemGraph(makeXAxis(seq1), seq1, QColor(0, 114, 189), label1);
    addStemGraph(makeXAxis(seq2), seq2, QColor(217, 83, 25), label2);

    QCPGraph* resultGraph = addStemGraph(makeXAxis(result), result, QColor(237, 177, 32), labelResult);
    resultGraph->setPen(QPen(QColor(237, 177, 32, 200), 2.0));
    resultGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QColor(237, 177, 32), QColor(237, 177, 32), 8));

    plot_->rescaleAxes();
    double yLower = plot_->yAxis->range().lower;
    double yUpper = plot_->yAxis->range().upper;
    plot_->yAxis->setRangeLower(qMin(yLower, -1.0));
    plot_->yAxis->setRangeUpper(qMax(yUpper, 1.0) * 1.1);

    qsizetype maxSize = std::max({ seq1.size(), seq2.size(), result.size() });
    plot_->xAxis->setRange(-1.0, static_cast<double>(maxSize));
    QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
    fixedTicker->setTickStep(1.0);
    plot_->xAxis->setTicker(fixedTicker);

    plot_->replot();
}

void ConvolutionPlotWidget::clear() {
    plot_->clearGraphs();
    plot_->replot();
}

void ConvolutionPlotWidget::autoScale() {
    plot_->rescaleAxes();

    double xLower = plot_->xAxis->range().lower;
    double xUpper = plot_->xAxis->range().upper;
    double xPadding = (xUpper - xLower) * 0.05;
    plot_->xAxis->setRange(xLower - xPadding, xUpper + xPadding);

    double yLower = plot_->yAxis->range().lower;
    double yUpper = plot_->yAxis->range().upper;
    double yPadding = (yUpper - yLower) * 0.1;
    plot_->yAxis->setRange(qMin(yLower - yPadding, -1.0), yUpper + yPadding);

    plot_->replot();
}