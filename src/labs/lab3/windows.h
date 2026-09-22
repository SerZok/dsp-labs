#pragma once
#include <QVector>
#include <QString>
#include <QList>

namespace FirWindows {

    enum class WindowType {
        Rectangular,
        Triangular,
        Bartlett,
        Hann,
        Hamming,
        Blackman,
        Kaiser4,
        Kaiser9,
        Chebyshev40,
        Chebyshev90,
        BartlettHann,
        BlackmanHarris,
        Bohman,
        Gauss,
        Nuttall,
        Tukey
    };

    QString windowName(WindowType type);

    QVector<double> generateWindow(WindowType type, int N);

    QList<WindowType> allWindows();

} // namespace FirWindows