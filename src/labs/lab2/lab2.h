#pragma once
#include "lab_base.h"

class Lab2Fft : public LabBase {
public:
    QString id() const override { return "lab2"; }
    QString name() const override { return "Лабораторная работа №2: БПФ"; }
    QString description() const override;
    QList<TaskBase*> tasks() const override;
};

