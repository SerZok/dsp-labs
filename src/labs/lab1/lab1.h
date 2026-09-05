#pragma once
#include "lab_base.h"

class Lab1Convolution : public LabBase {
public:
    QString id() const override { return "lab1"; }
    QString name() const override { return "Лабораторная работа 1"; }
    QString description() const override;
    QList<TaskBase*> tasks() const override;
};