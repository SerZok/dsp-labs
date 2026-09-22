#pragma once
#include "lab_base.h"

class Lab3Fir : public LabBase {
public:
    QString id() const override { return "lab3"; }
    QString name() const override { return "Лабораторная работа №3: КИХ-фильтры"; }
    QString description() const override;
    QList<TaskBase*> tasks() const override;
};