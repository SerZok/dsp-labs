#pragma once
#include <QMainWindow>
#include <QTreeWidget>
#include <QStackedWidget>
#include <QLabel>
#include "lab_base.h"
#include "task_base.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onTreeItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);

private:
    void populateNavTree();
    void clearAll();

    QTreeWidget* navTree_;
    QStackedWidget* taskStack_;

    QWidget* infoWidget_;
    QLabel* infoLabel_;

    QList<LabBase*> allLabs_;
    QList<TaskBase*> allTasks_;
};