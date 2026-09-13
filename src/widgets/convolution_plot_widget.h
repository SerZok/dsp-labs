#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include "qcustomplot.h"

class ConvolutionPlotWidget : public QWidget {
    Q_OBJECT
public:
    explicit ConvolutionPlotWidget(QWidget* parent = nullptr);

    void plotSequences(const QVector<double>& seq1, const QVector<double>& seq2,
        const QVector<double>& result,
        const QString& label1, const QString& label2,
        const QString& labelResult);
    void clear();

public slots:
    void autoScale();

private:
    QCustomPlot* plot_;

    QCPGraph* addStemGraph(const QVector<double>& x, const QVector<double>& y,
        const QColor& color, const QString& name);
};