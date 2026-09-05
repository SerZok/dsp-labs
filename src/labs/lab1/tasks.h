#pragma once
#include "task_base.h"

class Task1Linear : public TaskBase {
public:
    int id() const override { return 1; }
    QString title() const override { return "Линейная свертка"; }
    QString description() const override;
    QWidget* createWidget(QWidget* parent = nullptr) override;
};

class Task2Circular : public TaskBase {
public:
    int id() const override { return 2; }
    QString title() const override { return "Круговая свертка"; }
    QString description() const override;
    QWidget* createWidget(QWidget* parent = nullptr) override;
};

class Task3OverlapAdd : public TaskBase {
public:
    int id() const override { return 3; }
    QString title() const override { return "Метод перекрытия с суммированием"; }
    QString description() const override;
    QWidget* createWidget(QWidget* parent = nullptr) override;
};

class Task4OverlapSave : public TaskBase {
public:
    int id() const override { return 4; }
    QString title() const override { return "Метод перекрытия с накоплением"; }
    QString description() const override;
    QWidget* createWidget(QWidget* parent = nullptr) override;
};

