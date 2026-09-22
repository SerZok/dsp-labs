#pragma once
#include <QVector>
#include "windows.h"

enum class FilterType {
    LowPass,    // ФНЧ
    HighPass,   // ФВЧ
    BandPass,   // ПФ
    BandStop    // ПЗФ
};

struct FilterSpec {
    FilterType type;
    double sampleRate;      // Fs (Гц)
    double cutoffFreq;      // fc для ФНЧ/ФВЧ (Гц)
    double cutoffFreq1;     // fp1 для ПФ/ПЗФ (Гц)
    double cutoffFreq2;     // fp2 для ПФ/ПЗФ (Гц)
    FirWindows::WindowType window;
    int order;              // N-1 (размер окна = order + 1)
};

class FirDesign {
public:
    // Проектирование КИХ-фильтра методом взвешивания
    static QVector<double> designFir(const FilterSpec& spec);

    // Расчёт АЧХ (в линейных единицах)
    static QVector<double> magnitudeResponse(const QVector<double>& h, int numPoints = 1024);

    // Расчёт ЛАЧХ (в дБ)
    static QVector<double> logMagnitudeResponse(const QVector<double>& h, int numPoints = 1024);

    // Расчёт ФЧХ (в радианах)
    static QVector<double> phaseResponse(const QVector<double>& h, int numPoints = 1024);

    // Ось частот (Гц) от 0 до Fs/2
    static QVector<double> frequencyAxisHz(int numPoints, double sampleRate);

    // Ось нормированных частот (от 0 до π)
    static QVector<double> normalizedFrequencyAxis(int numPoints);

    // Проектирование ФНЧ с косинусоидальным сглаживанием (raised cosine)
    // alpha — roll-off factor (0..1)
    static QVector<double> designRaisedCosine(double sampleRate, double cutoffFreq,
        double rollOff, int order);

    // Характеристики окон (для задания 1)
    struct WindowCharacteristics {
        double mainLobeWidth;       // Ширина главного лепестка (в норм. частоте)
        double firstSideLobeDb;     // Амплитуда 1-го бокового лепестка (дБ)
        double minStopbandDb;       // Мин. затухание для тестового ФНЧ (дБ)
    };
    static WindowCharacteristics analyzeWindow(FirWindows::WindowType type, int N = 64);
};