#pragma once
#include "task_base.h"

// Задание 1: Исследование характеристик окон
class Task1WindowAnalysis : public TaskBase {
public:
    int id() const override { return 1; }
    QString title() const override { return "Исследование оконных функций"; }
    QString description() const override;
    QWidget* createWidget(QWidget* parent = nullptr) override;
};

// Задание 2: Метод взвешивания (вариант 3)
class Task2WindowMethod : public TaskBase {
public:
    int id() const override { return 2; }
    QString title() const override { return "Метод взвешивания (вариант 3)"; }
    QString description() const override;
    QWidget* createWidget(QWidget* parent = nullptr) override;
};

// Задание 3: Оптимальный фильтр (raised cosine)
class Task3OptimalFilter : public TaskBase {
public:
    int id() const override { return 3; }
    QString title() const override { return "Оптимальный фильтр (raised cosine)"; }
    QString description() const override;
    QWidget* createWidget(QWidget* parent = nullptr) override;
};

// Задание 6: Реакция фильтров на тестовые сигналы
class Task6FilterResponse : public TaskBase {
public:
    int id() const override { return 6; }
    QString title() const override { return "Реакция на тестовые сигналы"; }
    QString description() const override;
    QWidget* createWidget(QWidget* parent = nullptr) override;
};