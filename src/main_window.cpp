#include "main_window.h"
#include "lab_factory.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QGroupBox>
#include <QVariant>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("ЦОС — Лабораторные работы");
    resize(1100, 700);

    auto* central = new QWidget(this);
    auto* mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    auto* splitter = new QSplitter(Qt::Horizontal);

    auto* navGroup = new QGroupBox("Навигация");
    auto* navLayout = new QVBoxLayout(navGroup);

    infoWidget_ = new QWidget;
    auto* infoLayout = new QVBoxLayout(infoWidget_);
    infoLabel_ = new QLabel("Выберите лабораторную работу в дереве слева.");
    infoLabel_->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    infoLabel_->setWordWrap(true);
    infoLabel_->setStyleSheet("font-size: 14pt; padding: 20px;");
    infoLayout->addWidget(infoLabel_);

    navTree_ = new QTreeWidget;
    navTree_->setHeaderHidden(true);
    navTree_->setIndentation(20);
    navLayout->addWidget(navTree_);
    splitter->addWidget(navGroup);

    taskStack_ = new QStackedWidget;
    taskStack_->addWidget(infoWidget_);
    splitter->addWidget(taskStack_);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 2);
    splitter->setChildrenCollapsible(false);

    mainLayout->addWidget(splitter);
    setCentralWidget(central);

    populateNavTree();
    connect(navTree_, &QTreeWidget::currentItemChanged, this, &MainWindow::onTreeItemChanged);
}

MainWindow::~MainWindow() {
    clearAll();
}

void MainWindow::clearAll() {
    for (int i = taskStack_->count() - 1; i >= 0; --i) {
        QWidget* w = taskStack_->widget(i);
        if (w != infoWidget_) {
            taskStack_->removeWidget(w);
            delete w;
        }
    }

    qDeleteAll(allTasks_);
    allTasks_.clear();

    qDeleteAll(allLabs_);
    allLabs_.clear();
}

void MainWindow::populateNavTree() {
    navTree_->clear();
    clearAll();

    const auto labs = LabFactory::instance().availableLabs();
    for (const QString& labId : labs) {
        LabBase* lab = LabFactory::instance().createLab(labId);
        if (!lab) continue;

        allLabs_.append(lab);

        auto* labItem = new QTreeWidgetItem(navTree_);
        labItem->setText(0, lab->name());
        labItem->setData(0, Qt::UserRole, "lab");
        labItem->setData(0, Qt::UserRole + 1, QVariant::fromValue(lab));
        labItem->setExpanded(true);

        QList<TaskBase*> tasks = lab->tasks();
        for (TaskBase* task : tasks) {
            allTasks_.append(task);

            auto* taskItem = new QTreeWidgetItem(labItem);
            taskItem->setText(0, QString("Задание %1: %2").arg(task->id()).arg(task->title()));
            taskItem->setData(0, Qt::UserRole, "task");

            QWidget* taskWidget = task->createWidget();
            taskStack_->addWidget(taskWidget);
            taskItem->setData(0, Qt::UserRole + 1, QVariant::fromValue(taskWidget));
        }
    }
}

void MainWindow::onTreeItemChanged(QTreeWidgetItem* current, QTreeWidgetItem*) {
    if (!current) return;

    QString type = current->data(0, Qt::UserRole).toString();

    if (type == "lab") {
        LabBase* lab = current->data(0, Qt::UserRole + 1).value<LabBase*>();
        if (lab) {
            infoLabel_->setText(QString("%1\n%2")
                .arg(lab->name())
                .arg(lab->description()));
            taskStack_->setCurrentWidget(infoWidget_);
        }
    }
    else if (type == "task") {
        QWidget* taskWidget = current->data(0, Qt::UserRole + 1).value<QWidget*>();
        if (taskWidget) {
            taskStack_->setCurrentWidget(taskWidget);
        }
    }
}