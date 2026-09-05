#pragma once
#include <QString>
#include <QMap>
#include <functional>
#include "lab_base.h"

class LabFactory {
public:
    using Creator = std::function<LabBase* ()>;

    static LabFactory& instance();

    void registerLab(const QString& id, Creator creator);
    LabBase* createLab(const QString& id) const;
    QStringList availableLabs() const;

private:
    LabFactory() = default;
    QMap<QString, Creator> creators_;
};

// Макрос для автоматической регистрации лабораторной
#define REGISTER_LAB(LabClass) \
    namespace { \
        struct LabClass##Registrar { \
            LabClass##Registrar() { \
                LabFactory::instance().registerLab( \
                    LabClass().id(), \
                    []() -> LabBase* { return new LabClass(); } \
                ); \
            } \
        } labClass##RegistrarInstance; \
    }