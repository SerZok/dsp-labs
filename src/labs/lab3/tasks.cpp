#include "tasks.h"
#include "lab3.h"
#include "lab_factory.h"
#include "widgets/fir_response_widget.h"
#include "windows.h"
#include "fir_design.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QSplitter>
#include <QLabel>
#include <QTextEdit>
#include <QGroupBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QTabWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QStringList>

// ============================================
// ЗАДАНИЕ 1: Исследование окон
// ============================================

QString Task1WindowAnalysis::description() const {
    return "Исследовать характеристики оконных функций:\n"
        "• Ширина главного лепестка\n"
        "• Амплитуда первого бокового лепестка (дБ)\n"
        "• Минимальное затухание в полосе непропускания для тестового ФНЧ\n\n"
        "Выберите окно и наблюдайте его форму, спектр и характеристики.";
}

QWidget* Task1WindowAnalysis::createWidget(QWidget* parent) {
    auto* widget = new QWidget(parent);
    auto* layout = new QVBoxLayout(widget);

    auto* responseWidget = new FirResponseWidget;
    auto* infoLabel = new QLabel;
    infoLabel->setStyleSheet("font-weight: bold; color: #0072BD; padding: 5px;");
    infoLabel->setWordWrap(true);

    auto* formWidget = new QWidget;
    auto* windowCombo = new QComboBox;
    for (auto type : FirWindows::allWindows()) {
        windowCombo->addItem(FirWindows::windowName(type), (int)type);
    }

    auto* nSpin = new QSpinBox;
    nSpin->setRange(8, 512);
    nSpin->setValue(64);

    auto* formLayout = new QFormLayout(formWidget);
    formLayout->addRow("Оконная функция:", windowCombo);
    formLayout->addRow("Длина окна (N):", nSpin);

    auto* splitter = new QSplitter(Qt::Vertical);
    splitter->setChildrenCollapsible(false);
    splitter->addWidget(formWidget);
    splitter->addWidget(infoLabel);
    splitter->addWidget(responseWidget);
    splitter->setStretchFactor(2, 1);
    layout->addWidget(splitter);

    auto compute = [=]() {
        auto type = (FirWindows::WindowType)windowCombo->currentData().toInt();
        int N = nSpin->value();

        QVector<double> window = FirWindows::generateWindow(type, N);

        // Ось времени
        QVector<double> x(N);
        for (int i = 0; i < N; ++i) x[i] = i;

        // Показываем окно как импульсную характеристику
        responseWidget->plotImpulseResponse(window, FirWindows::windowName(type));

        // Считаем спектр окна
        int fftSize = 2048;
        QVector<double> padded(fftSize, 0.0);
        int offset = (fftSize - N) / 2;
        for (int i = 0; i < N; ++i) padded[offset + i] = window[i];

        QVector<double> freq(fftSize / 2);
        QVector<double> mag(fftSize / 2);
        for (int k = 0; k < fftSize / 2; ++k) {
            double omega = 2.0 * M_PI * k / fftSize;
            double re = 0.0, im = 0.0;
            for (int n = 0; n < fftSize; ++n) {
                re += padded[n] * std::cos(omega * n);
                im -= padded[n] * std::sin(omega * n);
            }
            freq[k] = k * 500.0 / (fftSize / 2);  // Нормированная частота
            mag[k] = std::sqrt(re * re + im * im);
        }

        // Нормируем
        double maxVal = *std::max_element(mag.begin(), mag.end());
        if (maxVal > 0) for (auto& v : mag) v /= maxVal;

        // ЛАЧХ окна
        QVector<double> logMag(fftSize / 2);
        for (int k = 0; k < fftSize / 2; ++k) {
            logMag[k] = 20.0 * std::log10(std::max(mag[k], 1e-10));
        }

        responseWidget->plotLogMagnitudeResponse(freq, logMag, "Спектр окна");

        // Характеристики
        auto chars = FirDesign::analyzeWindow(type, N);

        QString info = QString("<b>%1</b> (N=%2)<br>"
            "Ширина главного лепестка: <b>%3</b> рад<br>"
            "Амплитуда 1-го бокового лепестка: <b>%4 дБ</b><br>"
            "Мин. затухание в полосе непропускания (тест. ФНЧ): <b>%5 дБ</b>")
            .arg(FirWindows::windowName(type))
            .arg(N)
            .arg(chars.mainLobeWidth, 0, 'f', 4)
            .arg(chars.firstSideLobeDb, 0, 'f', 2)
            .arg(chars.minStopbandDb, 0, 'f', 2);
        infoLabel->setText(info);
        };

    compute();

    QObject::connect(windowCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), widget, compute);
    QObject::connect(nSpin, QOverload<int>::of(&QSpinBox::valueChanged), widget, compute);

    return widget;
}

// ============================================
// ЗАДАНИЕ 2: Метод взвешивания (вариант 3)
// ============================================

QString Task2WindowMethod::description() const {
    return "Спроектировать полосовой фильтр (ПФ) методом взвешивания.\n\n"
        "Вариант 3:\n"
        "• Тип: ПФ\n"
        "• Fs = 1000 Гц\n"
        "• fp1 = 200 Гц, fp2 = 300 Гц\n"
        "• Окна: Бартлетта-Ханна, Бомена, Кайзера β=4\n"
        "• Подавление в полосе непропускания ≥ 60 дБ\n\n"
        "Сравните характеристики фильтров с разными окнами.";
}

QWidget* Task2WindowMethod::createWidget(QWidget* parent) {
    auto* widget = new QWidget(parent);
    auto* layout = new QVBoxLayout(widget);

    auto* responseWidget = new FirResponseWidget;
    auto* infoLabel = new QLabel;
    infoLabel->setStyleSheet("padding: 5px;");
    infoLabel->setWordWrap(true);

    auto* formWidget = new QWidget;

    // Тип фильтра
    auto* typeCombo = new QComboBox;
    typeCombo->addItem("ФНЧ (LowPass)", (int)FilterType::LowPass);
    typeCombo->addItem("ФВЧ (HighPass)", (int)FilterType::HighPass);
    typeCombo->addItem("ПФ (BandPass)", (int)FilterType::BandPass);
    typeCombo->addItem("ПЗФ (BandStop)", (int)FilterType::BandStop);
    typeCombo->setCurrentIndex(2);  // ПФ по умолчанию

    auto* fsSpin = new QDoubleSpinBox;
    fsSpin->setRange(1, 1000000);
    fsSpin->setValue(1000);
    fsSpin->setSuffix(" Гц");

    auto* fc1Spin = new QDoubleSpinBox;
    fc1Spin->setRange(0, 1000000);
    fc1Spin->setValue(200);
    fc1Spin->setSuffix(" Гц");

    auto* fc2Spin = new QDoubleSpinBox;
    fc2Spin->setRange(0, 1000000);
    fc2Spin->setValue(300);
    fc2Spin->setSuffix(" Гц");

    auto* orderSpin = new QSpinBox;
    orderSpin->setRange(1, 1000);
    orderSpin->setValue(65);

    auto* windowCombo = new QComboBox;
    windowCombo->addItem("Бартлетта-Ханна", (int)FirWindows::WindowType::BartlettHann);
    windowCombo->addItem("Бомена", (int)FirWindows::WindowType::Bohman);
    windowCombo->addItem("Кайзера β=4", (int)FirWindows::WindowType::Kaiser4);
    windowCombo->addItem("Все три окна (сравнение)", -1);
    windowCombo->setCurrentIndex(3);  // Сравнение по умолчанию

    auto* formLayout = new QFormLayout(formWidget);
    formLayout->addRow("Тип фильтра:", typeCombo);
    formLayout->addRow("Частота дискретизации Fs:", fsSpin);
    formLayout->addRow("fp1 (для ПФ/ПЗФ):", fc1Spin);
    formLayout->addRow("fp2 (для ПФ/ПЗФ):", fc2Spin);
    formLayout->addRow("Порядок фильтра:", orderSpin);
    formLayout->addRow("Окно:", windowCombo);

    auto* splitter = new QSplitter(Qt::Vertical);
    splitter->setChildrenCollapsible(false);
    splitter->addWidget(formWidget);
    splitter->addWidget(infoLabel);
    splitter->addWidget(responseWidget);
    splitter->setStretchFactor(3, 1);
    layout->addWidget(splitter);

    // Цвета для сравнения
    QVector<QColor> colors = {
        QColor(0, 114, 189),   // синий
        QColor(217, 83, 25),   // оранжевый
        QColor(237, 177, 32),  // жёлтый
    };

    auto compute = [=]() {
        auto type = (FilterType)typeCombo->currentData().toInt();
        double fs = fsSpin->value();
        double fc1 = fc1Spin->value();
        double fc2 = fc2Spin->value();
        int order = orderSpin->value();
        int selectedWindow = windowCombo->currentData().toInt();

        FilterSpec spec;
        spec.type = type;
        spec.sampleRate = fs;
        spec.cutoffFreq = (fc1 + fc2) / 2.0;  // Для ФНЧ/ФВЧ
        spec.cutoffFreq1 = fc1;
        spec.cutoffFreq2 = fc2;
        spec.order = order;

        int numPoints = 2048;
        QVector<double> freq = FirDesign::frequencyAxisHz(numPoints, fs);

        responseWidget->clear();

        QString infoText;

        if (selectedWindow == -1) {
            // Сравнение всех трёх окон
            QList<FirWindows::WindowType> windows = {
                FirWindows::WindowType::BartlettHann,
                FirWindows::WindowType::Bohman,
                FirWindows::WindowType::Kaiser4
            };

            for (int i = 0; i < windows.size(); ++i) {
                spec.window = windows[i];
                QVector<double> h = FirDesign::designFir(spec);

                QVector<double> logMag = FirDesign::logMagnitudeResponse(h, numPoints);

                responseWidget->addLogMagnitudeCurve(freq, logMag,
                    FirWindows::windowName(windows[i]), colors[i]);

                // Находим минимум в полосе непропускания
                double minStop = 1e10;
                for (int k = 0; k < numPoints; ++k) {
                    double f = freq[k];
                    if (f < fc1 * 0.5 || f > fc2 * 1.5) {
                        if (logMag[k] < minStop) minStop = logMag[k];
                    }
                }

                infoText += QString("<b>%1</b> (порядок=%2): затухание ≈ %3 дБ<br>")
                    .arg(FirWindows::windowName(windows[i]))
                    .arg(order)
                    .arg(minStop, 0, 'f', 1);

                // Показываем импульсную только для первого
                if (i == 0) {
                    responseWidget->plotImpulseResponse(h, FirWindows::windowName(windows[i]));
                }
            }

            // Обновляем ЛАЧХ с легендой
            responseWidget->plotLogMagnitudeResponse(freq,
                FirDesign::logMagnitudeResponse(FirDesign::designFir(spec), numPoints),
                "");
        }
        else {
            spec.window = (FirWindows::WindowType)selectedWindow;
            QVector<double> h = FirDesign::designFir(spec);
            responseWidget->plotAll(h, fs, FirWindows::windowName(spec.window));

            double minStop = 1e10;
            QVector<double> logMag = FirDesign::logMagnitudeResponse(h, numPoints);
            for (int k = 0; k < numPoints; ++k) {
                double f = freq[k];
                if (f < fc1 * 0.5 || f > fc2 * 1.5) {
                    if (logMag[k] < minStop) minStop = logMag[k];
                }
            }

            infoText = QString("<b>%1</b> (порядок=%2): затухание ≈ %3 дБ")
                .arg(FirWindows::windowName(spec.window))
                .arg(order)
                .arg(minStop, 0, 'f', 1);
        }

        infoLabel->setText(infoText);
        };

    compute();

    QObject::connect(typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), widget, compute);
    QObject::connect(fsSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), widget, compute);
    QObject::connect(fc1Spin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), widget, compute);
    QObject::connect(fc2Spin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), widget, compute);
    QObject::connect(orderSpin, QOverload<int>::of(&QSpinBox::valueChanged), widget, compute);
    QObject::connect(windowCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), widget, compute);

    return widget;
}

// ============================================
// ЗАДАНИЕ 3: Оптимальный фильтр (raised cosine)
// ============================================

QString Task3OptimalFilter::description() const {
    return "Оптимальный нерекурсивный фильтр методом косинусоидального сглаживания.\n\n"
        "Вариант 3:\n"
        "• ФНЧ\n"
        "• Fs = 20 КГц\n"
        "• fp = 10 КГц, fs = 10.5 КГц\n"
        "• Подавление в полосе непропускания ≥ 60 дБ\n"
        "• Выбрать оптимальную оконную функцию\n\n"
        "Коэффициент сглаживания α (roll-off) определяет крутизну перехода.";
}

QWidget* Task3OptimalFilter::createWidget(QWidget* parent) {
    auto* widget = new QWidget(parent);
    auto* layout = new QVBoxLayout(widget);

    auto* responseWidget = new FirResponseWidget;
    auto* infoLabel = new QLabel;
    infoLabel->setStyleSheet("padding: 5px;");
    infoLabel->setWordWrap(true);

    auto* formWidget = new QWidget;

    auto* fsSpin = new QDoubleSpinBox;
    fsSpin->setRange(1, 1000000);
    fsSpin->setValue(20000);
    fsSpin->setSuffix(" Гц");

    auto* fpSpin = new QDoubleSpinBox;
    fpSpin->setRange(0, 1000000);
    fpSpin->setValue(10000);
    fpSpin->setSuffix(" Гц");

    auto* fsCutoffSpin = new QDoubleSpinBox;
    fsCutoffSpin->setRange(0, 1000000);
    fsCutoffSpin->setValue(10500);
    fsCutoffSpin->setSuffix(" Гц");

    auto* rollOffSpin = new QDoubleSpinBox;
    rollOffSpin->setRange(0.01, 1.0);
    rollOffSpin->setValue(0.5);
    rollOffSpin->setSingleStep(0.05);
    rollOffSpin->setDecimals(2);

    auto* orderSpin = new QSpinBox;
    orderSpin->setRange(1, 1000);
    orderSpin->setValue(101);

    auto* formLayout = new QFormLayout(formWidget);
    formLayout->addRow("Fs:", fsSpin);
    formLayout->addRow("fp (полоса пропускания):", fpSpin);
    formLayout->addRow("fs (полоса непропускания):", fsCutoffSpin);
    formLayout->addRow("Roll-off α:", rollOffSpin);
    formLayout->addRow("Порядок:", orderSpin);

    auto* splitter = new QSplitter(Qt::Vertical);
    splitter->setChildrenCollapsible(false);
    splitter->addWidget(formWidget);
    splitter->addWidget(infoLabel);
    splitter->addWidget(responseWidget);
    splitter->setStretchFactor(3, 1);
    layout->addWidget(splitter);

    auto compute = [=]() {
        double fs = fsSpin->value();
        double fp = fpSpin->value();
        double fstop = fsCutoffSpin->value();
        double alpha = rollOffSpin->value();
        int order = orderSpin->value();

        QVector<double> h = FirDesign::designRaisedCosine(fs, fp, alpha, order);
        responseWidget->plotAll(h, fs, "Raised Cosine");

        // Оценка затухания
        int numPoints = 2048;
        QVector<double> freq = FirDesign::frequencyAxisHz(numPoints, fs);
        QVector<double> logMag = FirDesign::logMagnitudeResponse(h, numPoints);

        double minStop = 1e10;
        for (int k = 0; k < numPoints; ++k) {
            if (freq[k] > fstop) {
                if (logMag[k] < minStop) minStop = logMag[k];
            }
        }

        infoLabel->setText(QString("<b>Raised Cosine</b> (α=%1, N=%2): затухание ≈ %3 дБ")
            .arg(alpha, 0, 'f', 2)
            .arg(order)
            .arg(minStop, 0, 'f', 1));
        };

    compute();

    QObject::connect(fsSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), widget, compute);
    QObject::connect(fpSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), widget, compute);
    QObject::connect(fsCutoffSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), widget, compute);
    QObject::connect(rollOffSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), widget, compute);
    QObject::connect(orderSpin, QOverload<int>::of(&QSpinBox::valueChanged), widget, compute);

    return widget;
}

// ============================================
// ЗАДАНИЕ 6: Реакция на тестовые сигналы
// ============================================

QString Task6FilterResponse::description() const {
    return "Найти реакцию фильтров на тестовые гармонические последовательности.\n\n"
        "Для каждого фильтра взять три синусоиды:\n"
        "• из полосы пропускания\n"
        "• из полосы непропускания\n"
        "• из переходной полосы";
}

QWidget* Task6FilterResponse::createWidget(QWidget* parent) {
    auto* widget = new QWidget(parent);
    auto* layout = new QVBoxLayout(widget);

    auto* formWidget = new QWidget;

    auto* fsSpin = new QDoubleSpinBox;
    fsSpin->setRange(1, 1000000);
    fsSpin->setValue(1000);
    fsSpin->setSuffix(" Гц");

    auto* fc1Spin = new QDoubleSpinBox;
    fc1Spin->setRange(0, 1000000);
    fc1Spin->setValue(200);
    fc1Spin->setSuffix(" Гц");

    auto* fc2Spin = new QDoubleSpinBox;
    fc2Spin->setRange(0, 1000000);
    fc2Spin->setValue(300);
    fc2Spin->setSuffix(" Гц");

    auto* orderSpin = new QSpinBox;
    orderSpin->setRange(1, 1000);
    orderSpin->setValue(65);

    auto* windowCombo = new QComboBox;
    windowCombo->addItem("Бартлетта-Ханна", (int)FirWindows::WindowType::BartlettHann);
    windowCombo->addItem("Бомена", (int)FirWindows::WindowType::Bohman);
    windowCombo->addItem("Кайзера β=4", (int)FirWindows::WindowType::Kaiser4);

    auto* f1Spin = new QDoubleSpinBox;  // Полоса пропускания
    f1Spin->setRange(0, 1000000);
    f1Spin->setValue(250);
    f1Spin->setSuffix(" Гц");
    f1Spin->setToolTip("Частота из полосы пропускания");

    auto* f2Spin = new QDoubleSpinBox;  // Полоса непропускания
    f2Spin->setRange(0, 1000000);
    f2Spin->setValue(50);
    f2Spin->setSuffix(" Гц");
    f2Spin->setToolTip("Частота из полосы непропускания");

    auto* f3Spin = new QDoubleSpinBox;  // Переходная полоса
    f3Spin->setRange(0, 1000000);
    f3Spin->setValue(150);
    f3Spin->setSuffix(" Гц");
    f3Spin->setToolTip("Частота из переходной полосы");

    auto* nSamplesSpin = new QSpinBox;
    nSamplesSpin->setRange(100, 10000);
    nSamplesSpin->setValue(500);

    auto* formLayout = new QFormLayout(formWidget);
    formLayout->addRow("Fs:", fsSpin);
    formLayout->addRow("fp1:", fc1Spin);
    formLayout->addRow("fp2:", fc2Spin);
    formLayout->addRow("Порядок:", orderSpin);
    formLayout->addRow("Окно:", windowCombo);
    formLayout->addRow("f (полоса пропускания):", f1Spin);
    formLayout->addRow("f (полоса непропускания):", f2Spin);
    formLayout->addRow("f (переходная полоса):", f3Spin);
    formLayout->addRow("Число отсчётов:", nSamplesSpin);

    // Три графика: вход-выход для каждой частоты
    auto* plot1 = new QCustomPlot;
    auto* plot2 = new QCustomPlot;
    auto* plot3 = new QCustomPlot;

    for (auto* plot : { plot1, plot2, plot3 }) {
        plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
        plot->setMinimumHeight(120);
        plot->legend->setVisible(true);
        plot->xAxis->setLabel("n (отсчёты)");
        plot->yAxis->setLabel("Амплитуда");
    }

    auto* infoLabel = new QLabel;
    infoLabel->setStyleSheet("padding: 5px;");
    infoLabel->setWordWrap(true);

    auto* splitter = new QSplitter(Qt::Vertical);
    splitter->addWidget(formWidget);
    splitter->addWidget(infoLabel);
    splitter->addWidget(plot1);
    splitter->addWidget(plot2);
    splitter->addWidget(plot3);
    splitter->setStretchFactor(3, 1);
    layout->addWidget(splitter);

    auto compute = [=]() {
        double fs = fsSpin->value();
        int order = orderSpin->value();
        int N = nSamplesSpin->value();

        FilterSpec spec;
        spec.type = FilterType::BandPass;
        spec.sampleRate = fs;
        spec.cutoffFreq1 = fc1Spin->value();
        spec.cutoffFreq2 = fc2Spin->value();
        spec.window = (FirWindows::WindowType)windowCombo->currentData().toInt();
        spec.order = order;

        QVector<double> h = FirDesign::designFir(spec);

        // Три тестовые частоты
        double freqs[3] = { f1Spin->value(), f2Spin->value(), f3Spin->value() };
        QCustomPlot* plots[3] = { plot1, plot2, plot3 };
        QString labels[3] = { "Полоса пропускания", "Полоса непропускания", "Переходная полоса" };

        QString info;

        for (int i = 0; i < 3; ++i) {
            plots[i]->clearGraphs();

            double f = freqs[i];
            double omega = 2.0 * M_PI * f / fs;

            // Входной сигнал
            QVector<double> x(N), y(N);
            for (int n = 0; n < N; ++n) {
                x[n] = std::sin(omega * n);
            }

            // Свёртка x с h
            int hLen = h.size();
            for (int n = 0; n < N; ++n) {
                y[n] = 0;
                for (int k = 0; k < hLen; ++k) {
                    if (n - k >= 0) {
                        y[n] += h[k] * x[n - k];
                    }
                }
            }

            // Ось времени
            QVector<double> t(N);
            for (int n = 0; n < N; ++n) t[n] = n;

            // Вход
            QCPGraph* inGraph = plots[i]->addGraph();
            inGraph->setData(t, x);
            inGraph->setPen(QPen(QColor(0, 114, 189, 150), 1.0));
            inGraph->setName("Вход");

            // Выход
            QCPGraph* outGraph = plots[i]->addGraph();
            outGraph->setData(t, y);
            outGraph->setPen(QPen(QColor(217, 83, 25), 1.5));
            outGraph->setName("Выход");

            plots[i]->xAxis->setRange(0, N);
            plots[i]->rescaleAxes();
            plots[i]->replot();

            // Измеряем амплитуду выхода (после установления)
            double maxOut = 0;
            for (int n = hLen; n < N; ++n) {
                if (std::abs(y[n]) > maxOut) maxOut = std::abs(y[n]);
            }

            double attenDb = 20.0 * std::log10(std::max(maxOut, 1e-10));
            info += QString("<b>%1</b> (f=%2 Гц): амплитуда выхода = %3 (%4 дБ)<br>")
                .arg(labels[i])
                .arg(f, 0, 'f', 1)
                .arg(maxOut, 0, 'f', 4)
                .arg(attenDb, 0, 'f', 2);
        }

        infoLabel->setText(info);
        };

    compute();

    auto connectSpin = [&](auto* spin) {
        QObject::connect(spin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), widget, compute);
        };
    connectSpin(fsSpin);
    connectSpin(fc1Spin);
    connectSpin(fc2Spin);
    connectSpin(f1Spin);
    connectSpin(f2Spin);
    connectSpin(f3Spin);
    QObject::connect(orderSpin, QOverload<int>::of(&QSpinBox::valueChanged), widget, compute);
    QObject::connect(nSamplesSpin, QOverload<int>::of(&QSpinBox::valueChanged), widget, compute);
    QObject::connect(windowCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), widget, compute);

    return widget;
}