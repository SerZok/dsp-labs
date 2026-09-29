#include "tasks.h"
#include "lab2.h"
#include "lab_factory.h"
#include "widgets/spectrum_plot_widget.h"
#include "fft_algorithms.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QSplitter>
#include <QLabel>
#include <QPushButton>
#include <QStringList>
#include <QElapsedTimer>
#include <QTextEdit>       // <-- Добавлено для лога
#include <QFont>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ============================================
// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ
// ============================================

static QString complexToString(const std::complex<double>& c) {
    return QString("%1 %2 %3i")
        .arg(c.real(), 0, 'f', 4)
        .arg(c.imag() >= 0 ? "+" : "-")
        .arg(std::abs(c.imag()), 0, 'f', 4);
}

static QVector<double> generateSignal(int N, double freq1, double freq2) {
    QVector<double> signal(N);
    for (int n = 0; n < N; ++n) {
        double x = 2.0 * M_PI * n / N;
        signal[n] = std::cos(freq1 * x) + std::sin(freq2 * x);
    }
    return signal;
}

// ============================================
// ЗАДАНИЕ 1: Прямое БПФ
// ============================================

QString Task1DirectFft::description() const {
    return "Выполнить прямое БПФ для сигнала Y = cos(2x) + sin(5x).\n\n"
        "В амплитудном спектре должны наблюдаться два пика:\n"
        "• на частотном индексе k = 2 (от косинусной составляющей)\n"
        "• на частотном индексе k = 5 (от синусной составляющей)\n\n"
        "Изменяйте количество отсчётов N и наблюдайте за числовыми значениями в логе.";
}

QWidget* Task1DirectFft::createWidget(QWidget* parent) {
    auto* widget = new QWidget(parent);
    auto* layout = new QGridLayout(widget);

    auto* spectrumWidget = new SpectrumPlotWidget;
    auto* timeLabel = new QLabel("Время расчёта: - мс");
    timeLabel->setStyleSheet("color: blue; font-weight: bold;");

    // === ЛОГ С РЕЗУЛЬТАТАМИ ===
    auto* logEdit = new QTextEdit;
    logEdit->setReadOnly(true);
    logEdit->setFont(QFont("Consolas", 9));

    auto* formWidget = new QWidget;
    auto* nSpin = new QSpinBox;
    nSpin->setRange(8, 8192);
    nSpin->setValue(64);
    nSpin->setSingleStep(1);
    nSpin->setToolTip("Количество отсчётов (должно быть степенью двойки)");

    auto* freq1Spin = new QDoubleSpinBox;
    freq1Spin->setRange(0.0, 50.0);
    freq1Spin->setValue(2.0);
    freq1Spin->setDecimals(1);
    freq1Spin->setSingleStep(1.0);

    auto* freq2Spin = new QDoubleSpinBox;
    freq2Spin->setRange(0.0, 50.0);
    freq2Spin->setValue(5.0);
    freq2Spin->setDecimals(1);
    freq2Spin->setSingleStep(1.0);

    auto* formLayout = new QFormLayout(formWidget);
    formLayout->addRow("Количество отсчётов (N):", nSpin);
    formLayout->addRow("Частота 1 (косинус):", freq1Spin);
    formLayout->addRow("Частота 2 (синус):", freq2Spin);
    formLayout->addRow("Производительность:", timeLabel);

    auto* splitter = new QSplitter(Qt::Vertical);
    splitter->setChildrenCollapsible(false);
    splitter->addWidget(formWidget);
    splitter->addWidget(spectrumWidget);
    splitter->setStretchFactor(2, 1);
    layout->addWidget(splitter, 0, 0);
    layout->addWidget(logEdit, 0, 1);
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 0);

    auto compute = [=]() {
        int N = nSpin->value();

        if (!FftAlgorithms::isPowerOfTwo(N)) {
            int rounded = 1;
            while (rounded < N) rounded <<= 1;
            nSpin->setValue(rounded);
            return;
        }

        double freq1 = freq1Spin->value();
        double freq2 = freq2Spin->value();

        QVector<double> signal = generateSignal(N, freq1, freq2);
        QVector<double> timeAxis(N);
        for (int i = 0; i < N; ++i) timeAxis[i] = i;

        QElapsedTimer timer;
        timer.start();

        FftResult fftResult = FftAlgorithms::directFft(signal);

        qint64 elapsedNs = timer.nsecsElapsed();
        double elapsedMs = elapsedNs / 1000000.0;
        timeLabel->setText(QString("Время расчёта БПФ: <b>%1 мс</b>").arg(elapsedMs, 0, 'f', 4));

        QString html;
        html += "<table style='border-collapse: collapse; width: 100%; font-family: monospace; font-size: 14px; text-align: center;'>";
        html += "<tr style='background-color: #f0f0f0; border-bottom: 2px solid #ccc;'>"
            "<th style='padding: 8px;'>k</th>"
            "<th style='padding: 8px;'>Амплитуда</th>"
            "<th style='padding: 8px;'>Фаза (рад)</th>"
            "<th style='padding: 8px;'>Фаза (град)</th>"
            "</tr>";

        for (int k = 0; k < N; ++k) {
            double amp = std::abs(fftResult.spectrum[k]) / N;
            double phase = (amp < 1e-10) ? 0.0 : std::arg(fftResult.spectrum[k]);
            double phaseDeg = phase * 180.0 / M_PI;
            QString style = (amp > 1e-3) ? "style='background-color: #e6f9e6; font-weight: bold;'" : "style='color: #999;'";

            html += QString("<tr %1>").arg(style);
            html += QString("<td style='padding: 6px; border-bottom: 1px solid #eee;'>%1</td>").arg(k);
            html += QString("<td style='padding: 6px; border-bottom: 1px solid #eee;'>%1</td>").arg(amp, 0, 'f', 4);
            html += QString("<td style='padding: 6px; border-bottom: 1px solid #eee;'>%1</td>").arg(phase, 0, 'f', 4);
            html += QString("<td style='padding: 6px; border-bottom: 1px solid #eee;'>%1</td>").arg(phaseDeg, 0, 'f', 2);
            html += "</tr>";
        }
        html += "</table>";
        logEdit->setHtml(html);

        spectrumWidget->plotTimeDomain(timeAxis, signal,
            QString("Y = cos(%1x) + sin(%2x), N = %3").arg(freq1).arg(freq2).arg(N));

        spectrumWidget->plotSpectrum(fftResult, 1.0, false);
        };

    compute();

    QObject::connect(nSpin, QOverload<int>::of(&QSpinBox::valueChanged), widget, compute);
    QObject::connect(freq1Spin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), widget, compute);
    QObject::connect(freq2Spin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), widget, compute);

    return widget;
}