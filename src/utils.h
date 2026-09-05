#pragma once
#include <QVector>
#include <QString>
#include <QStringList>

namespace utils {
    inline QString toString(const QVector<double>& seq, int precision = 2) {
        QStringList parts;
        for (double v : seq) parts.append(QString::number(v, 'f', precision));
        return parts.join(", ");
    }

    inline QVector<double> fromString(const QString& text) {
        QVector<double> result;
        QStringList parts = text.split(',', Qt::SkipEmptyParts);
        for (const QString& part : parts) {
            bool ok;
            double val = part.trimmed().toDouble(&ok);
            if (ok) result.append(val);
        }
        return result;
    }
}