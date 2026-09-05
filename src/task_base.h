#pragma once
#include <QString>
#include <QWidget>

class TaskBase {
public:
    virtual ~TaskBase() = default;

    virtual int id() const = 0;
    virtual QString title() const = 0;
    virtual QString description() const = 0;

    virtual QWidget* createWidget(QWidget* parent = nullptr) = 0;
};