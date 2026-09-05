#include "lab_factory.h"

LabFactory& LabFactory::instance() {
    static LabFactory factory;
    return factory;
}

void LabFactory::registerLab(const QString& id, Creator creator) {
    creators_[id] = creator;
}

LabBase* LabFactory::createLab(const QString& id) const {
    auto it = creators_.find(id);
    if (it != creators_.end()) {
        return it.value()();
    }
    return nullptr;
}

QStringList LabFactory::availableLabs() const {
    return creators_.keys();
}