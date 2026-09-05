#include "lab1.h"
#include "tasks.h"
#include "lab_factory.h"
#include "widgets/convolution_plot_widget.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QSplitter>
#include <QStringList>
#include "convolution_algorithms.h"

// ============================================
// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ
// ============================================

static QString toString(const QVector<double>& seq) {
    QStringList parts;
    for (double v : seq) parts.append(QString::number(v, 'f', 2));
    return parts.join(", ");
}

static QVector<double> parseSequence(const QString& text) {
    QVector<double> result;
    QStringList parts = text.split(',', Qt::SkipEmptyParts);
    for (const QString& part : parts) {
        bool ok;
        double val = part.trimmed().toDouble(&ok);
        if (ok) result.append(val);
    }
    return result;
}

// ============================================
// ЗАДАНИЕ 1: Линейная свертка
// ============================================

QString Task1Linear::description() const {
    return "Вычислить линейную свертку последовательностей s1(n) и h1(n).";
}

QWidget* Task1Linear::createWidget(QWidget* parent) {
    auto* widget = new QWidget(parent);
    auto* layout = new QVBoxLayout(widget);

    auto* plotWidget = new ConvolutionPlotWidget;
    auto* resultEdit = new QLineEdit;
    resultEdit->setReadOnly(true);

    auto* formWidget = new QWidget;
    auto* sEdit = new QLineEdit("1, 2, 8, -6, -5, 6");
    auto* hEdit = new QLineEdit("2, 3, 0, -1");

    auto* formLayout = new QFormLayout(formWidget);
    formLayout->addRow("s(n):", sEdit);
    formLayout->addRow("h(n):", hEdit);
    formLayout->addRow("Результат:", resultEdit);

    auto* splitter = new QSplitter(Qt::Vertical);
    splitter->setChildrenCollapsible(false);
    splitter->addWidget(formWidget);
    splitter->addWidget(plotWidget);
    splitter->setStretchFactor(1, 1);
    layout->addWidget(splitter);

    auto compute = [=]() {
        QVector<double> s = parseSequence(sEdit->text());
        QVector<double> h = parseSequence(hEdit->text());

        auto result = ConvolutionAlgorithms::linear(s, h);
        resultEdit->setText(toString(result));
        plotWidget->plotSequences(s, h, result, "s(n)", "h(n)", "Результат");
        };

    compute();

    QObject::connect(sEdit, &QLineEdit::textChanged, widget, compute);
    QObject::connect(hEdit, &QLineEdit::textChanged, widget, compute);

    return widget;
}

// ============================================
// ЗАДАНИЕ 2: Круговая свертка
// ============================================

QString Task2Circular::description() const {
    return "Вычислить круговую (циклическую) свертку последовательностей s2(n) и h2(n).";
}

QWidget* Task2Circular::createWidget(QWidget* parent) {
    auto* widget = new QWidget(parent);
    auto* layout = new QVBoxLayout(widget);

    auto* plotWidget = new ConvolutionPlotWidget;
    auto* resultEdit = new QLineEdit;
    resultEdit->setReadOnly(true);

    auto* formWidget = new QWidget;
    auto* sEdit = new QLineEdit("-9, 8, 6, 2");
    auto* hEdit = new QLineEdit("6, -9, 1, 7");

    auto* formLayout = new QFormLayout(formWidget);
    formLayout->addRow("s(n):", sEdit);
    formLayout->addRow("h(n):", hEdit);
    formLayout->addRow("Результат:", resultEdit);

    auto* splitter = new QSplitter(Qt::Vertical);
    splitter->setChildrenCollapsible(false);
    splitter->addWidget(formWidget);
    splitter->addWidget(plotWidget);
    splitter->setStretchFactor(1, 1);
    layout->addWidget(splitter);

    auto compute = [=]() {
        QVector<double> s = parseSequence(sEdit->text());
        QVector<double> h = parseSequence(hEdit->text());

        auto result = ConvolutionAlgorithms::circular(s, h);
        resultEdit->setText(toString(result));
        plotWidget->plotSequences(s, h, result, "s(n)", "h(n)", "Результат");
        };

    compute();

    QObject::connect(sEdit, &QLineEdit::textChanged, widget, compute);
    QObject::connect(hEdit, &QLineEdit::textChanged, widget, compute);

    return widget;
}

// ============================================
// ЗАДАНИЕ 3: Overlap-Add
// ============================================

QString Task3OverlapAdd::description() const {
    return "Линейная свертка методом перекрытия с суммированием.";
}

QWidget* Task3OverlapAdd::createWidget(QWidget* parent) {
    auto* widget = new QWidget(parent);
    auto* layout = new QVBoxLayout(widget);

    auto* plotWidget = new ConvolutionPlotWidget;
    auto* resultEdit = new QLineEdit;
    resultEdit->setReadOnly(true);

    auto* formWidget = new QWidget;
    auto* sEdit = new QLineEdit("5, 2, 1, 2");
    auto* hEdit = new QLineEdit("5, 6, 1, 8, -5, 2, -4, 1, 2, 8, 6, 7, 1, 3, -3, 5, 2, 3");

    auto* formLayout = new QFormLayout(formWidget);
    formLayout->addRow("s(n):", sEdit);
    formLayout->addRow("h(n):", hEdit);
    formLayout->addRow("Результат:", resultEdit);

    auto* splitter = new QSplitter(Qt::Vertical);
    splitter->setChildrenCollapsible(false);
    splitter->addWidget(formWidget);
    splitter->addWidget(plotWidget);
    splitter->setStretchFactor(1, 1);
    layout->addWidget(splitter);

    auto compute = [=]() {
        QVector<double> s = parseSequence(sEdit->text());
        QVector<double> h = parseSequence(hEdit->text());

        auto result = ConvolutionAlgorithms::overlapAdd(s, h, 4);
        resultEdit->setText(toString(result));
        plotWidget->plotSequences(s, h, result, "s(n)", "h(n)", "Результат");
        };

    compute();

    QObject::connect(sEdit, &QLineEdit::textChanged, widget, compute);
    QObject::connect(hEdit, &QLineEdit::textChanged, widget, compute);

    return widget;
}

// ============================================
// ЗАДАНИЕ 4: Overlap-Save
// ============================================

QString Task4OverlapSave::description() const {
    return "Линейная свертка методом перекрытия с накоплением.";
}

QWidget* Task4OverlapSave::createWidget(QWidget* parent) {
    auto* widget = new QWidget(parent);
    auto* layout = new QVBoxLayout(widget);

    auto* plotWidget = new ConvolutionPlotWidget;
    auto* resultEdit = new QLineEdit;
    resultEdit->setReadOnly(true);

    auto* formWidget = new QWidget;
    auto* sEdit = new QLineEdit("5, 2, 1, 2");
    auto* hEdit = new QLineEdit("5, 6, 1, 8, -5, 2, -4, 1, 2, 8, 6, 7, 1, 3, -3, 5, 2, 3");

    auto* formLayout = new QFormLayout(formWidget);
    formLayout->addRow("s(n):", sEdit);
    formLayout->addRow("h(n):", hEdit);
    formLayout->addRow("Результат:", resultEdit);

    auto* splitter = new QSplitter(Qt::Vertical);
    splitter->setChildrenCollapsible(false);
    splitter->addWidget(formWidget);
    splitter->addWidget(plotWidget);
    splitter->setStretchFactor(1, 1);
    layout->addWidget(splitter);

    auto compute = [=]() {
        QVector<double> s = parseSequence(sEdit->text());
        QVector<double> h = parseSequence(hEdit->text());

        auto result = ConvolutionAlgorithms::overlapSave(s, h, 4);
        resultEdit->setText(toString(result));
        plotWidget->plotSequences(s, h, result, "s(n)", "h(n)", "Результат");
        };

    compute();

    QObject::connect(sEdit, &QLineEdit::textChanged, widget, compute);
    QObject::connect(hEdit, &QLineEdit::textChanged, widget, compute);

    return widget;
}

REGISTER_LAB(Lab1Convolution)