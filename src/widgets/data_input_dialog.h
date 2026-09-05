#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>

class DataInputDialog : public QDialog {
    Q_OBJECT
public:
    explicit DataInputDialog(const QString& title, QWidget* parent = nullptr);

    QVector<double> getSeq1() const;
    QVector<double> getSeq2() const;
    int getBlockSize() const;

    void setSeq1(const QVector<double>& seq);
    void setSeq2(const QVector<double>& seq);
    void setBlockSize(int size);

    QSpinBox* blockSizeSpinBox() const { return blockSizeSpin_; }

private:
    QLineEdit* seq1Edit_;
    QLineEdit* seq2Edit_;
    QSpinBox* blockSizeSpin_;

    QVector<double> parseSequence(const QString& text) const;
    QString toString(const QVector<double>& seq) const;
};