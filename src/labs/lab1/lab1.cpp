#include "lab1.h"
#include "tasks.h"
#include "lab_factory.h"

QString Lab1Convolution::description() const {
    return "Цель работы: научиться вычислять линейную и круговую (циклическую) "
        "дискретную свертку последовательностей.\n\n"
        "Задания:\n"
        "1. Линейная свертка\n"
        "2. Круговая свертка\n"
        "3. Линейная свертка методом перекрытия с суммированием\n"
        "4. Линейная свертка методом перекрытия с накоплением";
}

QList<TaskBase*> Lab1Convolution::tasks() const {
    return {
        new Task1Linear(),
        new Task2Circular(),
        new Task3OverlapAdd(),
        new Task4OverlapSave()
    };
}

// Автоматическая регистрация в фабрике
REGISTER_LAB(Lab1Convolution)