#pragma once
#include "task_base.h"

class Task1DirectFft : public TaskBase {
public:
    int id() const override { return 1; }
    QString title() const override { return "Прямое БПФ"; }
    QString description() const override;
    QWidget* createWidget(QWidget* parent = nullptr) override;
};