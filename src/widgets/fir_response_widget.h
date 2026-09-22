#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include "qcustomplot.h"

class FirResponseWidget : public QWidget {
    Q_OBJECT
public:
    explicit FirResponseWidget(QWidget* parent = nullptr);

    // Показать импульсную характеристику
    void plotImpulseResponse(const QVector<double>& h, const QString& title = "");

    // Показать АЧХ (линейная)
    void plotMagnitudeResponse(const QVector<double>& freq, const QVector<double>& mag,
        const QString& title = "");

    // Показать ЛАЧХ (дБ)
    void plotLogMagnitudeResponse(const QVector<double>& freq, const QVector<double>& logMag,
        const QString& title = "");

    // Показать ФЧХ
    void plotPhaseResponse(const QVector<double>& freq, const QVector<double>& phase,
        const QString& title = "");

    // Показать все характеристики сразу
    void plotAll(const QVector<double>& h, double sampleRate, const QString& title = "");

    // Добавить кривую на существующий график (для сравнения)
    void addMagnitudeCurve(const QVector<double>& freq, const QVector<double>& mag,
        const QString& name, const QColor& color);
    void addLogMagnitudeCurve(const QVector<double>& freq, const QVector<double>& logMag,
        const QString& name, const QColor& color);

    // Очистить все графики
    void clear();

public slots:
    void autoScaleAll();

private:
    QCustomPlot* impulsePlot_;
    QCustomPlot* magnitudePlot_;
    QCustomPlot* logMagPlot_;
    QCustomPlot* phasePlot_;
};