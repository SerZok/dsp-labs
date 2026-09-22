#include "lab3.h"
#include "tasks.h"
#include "lab_factory.h"

QString Lab3Fir::description() const {
    return "Цель работы:\n"
        "1. Научиться проектировать нерекурсивные (КИХ) фильтры\n"
        "2. Провести анализ различных фильтров при одинаковых требованиях\n\n"
        "Вариант 3:\n"
        "• Задание 2: ПФ, Fs=1 КГц, fp1=200 Гц, fp2=300 Гц\n"
        "  Окна: Бартлетта-Ханна, Бомена, Кайзера β=4\n"
        "• Задание 3: ФНЧ с raised cosine, Fs=20 КГц, fp=10 КГц, fs=10.5 КГц\n"
        "• Задание 6: Реакция фильтров на тестовые сигналы\n\n"
        "Примечание: Задания 4 и 5 (многополосовой фильтр и FDA Tool) "
        "требуют специализированных алгоритмов и MATLAB.";
}

QList<TaskBase*> Lab3Fir::tasks() const {
    return {
        new Task1WindowAnalysis(),
        new Task2WindowMethod(),
        new Task3OptimalFilter(),
        new Task6FilterResponse()
    };
}

REGISTER_LAB(Lab3Fir)