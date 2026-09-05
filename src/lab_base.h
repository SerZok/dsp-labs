#pragma once
#include <QString>
#include <QList>
#include <QWidget>

class TaskBase;

class LabBase {
public:
    virtual ~LabBase() = default;

    // Метаданные лабораторной
    virtual QString id() const = 0;
    virtual QString name() const = 0;
    virtual QString description() const = 0;

    virtual QList<TaskBase*> tasks() const = 0;
    static LabBase* create(const QString& labId);
};