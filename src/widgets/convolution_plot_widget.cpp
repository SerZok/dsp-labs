#include "convolution_plot_widget.h"

ConvolutionPlotWidget::ConvolutionPlotWidget(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    plot_ = new QCustomPlot(this);
    layout->addWidget(plot_);

    plot_->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    plot_->xAxis->setLabel("n (отсчеты)");
    plot_->yAxis->setLabel("Амплитуда");
    plot_->legend->setVisible(true);
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

    // s(n)
    plot_->addGraph();
    plot_->graph(0)->setData(makeXAxis(seq1), seq1);
    plot_->graph(0)->setPen(QPen(Qt::blue));
    plot_->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));
    plot_->graph(0)->setName(label1);

    // h(n)
    plot_->addGraph();
    plot_->graph(1)->setData(makeXAxis(seq2), seq2);
    plot_->graph(1)->setPen(QPen(Qt::red));
    plot_->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, 5));
    plot_->graph(1)->setName(label2);

    // Результат
    plot_->addGraph();
    plot_->graph(2)->setData(makeXAxis(result), result);
    plot_->graph(2)->setPen(QPen(Qt::darkGreen, 2));
    plot_->graph(2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDiamond, 6));
    plot_->graph(2)->setName(labelResult);

    plot_->rescaleAxes();
    plot_->replot();
}

void ConvolutionPlotWidget::clear() {
    plot_->clearGraphs();
    plot_->replot();
}