#include "data_input_dialog.h"
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QLabel>

DataInputDialog::DataInputDialog(const QString& title, QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(title);
    auto* layout = new QFormLayout(this);

    layout->addRow(new QLabel("Введите последовательности через запятую:"));

    seq1Edit_ = new QLineEdit(this);
    seq1Edit_->setPlaceholderText("Например: 1, 2, 3, 4");
    layout->addRow("s(n):", seq1Edit_);

    seq2Edit_ = new QLineEdit(this);
    seq2Edit_->setPlaceholderText("Например: 2, 3, 0, -1");
    layout->addRow("h(n):", seq2Edit_);

    blockSizeSpin_ = new QSpinBox(this);
    blockSizeSpin_->setRange(2, 100);
    blockSizeSpin_->setValue(4);
    layout->addRow("Размер блока:", blockSizeSpin_);

    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addRow(buttons);
}

QVector<double> DataInputDialog::getSeq1() const { return parseSequence(seq1Edit_->text()); }
QVector<double> DataInputDialog::getSeq2() const { return parseSequence(seq2Edit_->text()); }
int DataInputDialog::getBlockSize() const { return blockSizeSpin_->value(); }

void DataInputDialog::setSeq1(const QVector<double>& seq) { seq1Edit_->setText(toString(seq)); }
void DataInputDialog::setSeq2(const QVector<double>& seq) { seq2Edit_->setText(toString(seq)); }
void DataInputDialog::setBlockSize(int size) { blockSizeSpin_->setValue(size); }

QVector<double> DataInputDialog::parseSequence(const QString& text) const {
    QVector<double> result;
    QStringList parts = text.split(',', Qt::SkipEmptyParts);
    for (const QString& part : parts) {
        bool ok;
        double val = part.trimmed().toDouble(&ok);
        if (ok) result.append(val);
    }
    return result;
}

QString DataInputDialog::toString(const QVector<double>& seq) const {
    QStringList parts;
    \
    return parts.join(", ");
}