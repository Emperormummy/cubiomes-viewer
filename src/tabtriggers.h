#ifndef TABTRIGGERS_H
#define TABTRIGGERS_H

#include <QWidget>
#include <QThread>
#include <QTreeWidgetItem>

#include "mainwindow.h"
#include "search.h"
#include "world.h"

namespace Ui {
class TabTriggers;
}

class AnalysisTriggers : public QThread
{
    Q_OBJECT
public:
    explicit AnalysisTriggers(QObject *parent = nullptr)
        : QThread(parent) {}

    virtual void run() override;

signals:
    void itemDone(QTreeWidgetItem *item);

public:
    QVector<Condition> conds;
    QVector<uint64_t> seeds;
    WorldInfo wi;
    std::atomic_bool stop;
};


class TabTriggers : public QWidget, public ISaveTab
{
    Q_OBJECT

public:
    explicit TabTriggers(MainWindow *parent);
    ~TabTriggers();

    virtual void save(QSettings& settings) override;
    virtual void load(QSettings& settings) override;

private slots:
    void onAnalysisItemDone(QTreeWidgetItem *item);
    void onAnalysisFinished();

    void on_pushStart_clicked();
    void on_pushExpand_clicked();
    void on_pushExport_clicked();

    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

private:
    Ui::TabTriggers *ui;
    MainWindow *parent;
    AnalysisTriggers thread;
};

#endif // TABTRIGGERS_H
