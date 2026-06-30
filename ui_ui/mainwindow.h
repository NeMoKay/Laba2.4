#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QGroupBox>
#include <QTableWidget>
#include <QTimer>
#include <functional>

#include "AlphabetIndex.hpp"
#include "ArraySequence.hpp"
#include "LazySequence.hpp"
#include "Stream.hpp"

class MainWindow : public QMainWindow{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onRun();
    void onStop();
    void onInputModeChanged(int index);
    void onTimerTick();
    void onGenerateTestData();

private:
    void setupIndexTab();
    void normalizeTable(QTableWidget* table);
    void clearLiveResources();

    void executeManualIndexing();
    void populateTable(AlphavitIndex<ArraySequence>* index);

    QTabWidget *tabs;
    QWidget *tabIndex;
    QGroupBox *groupManual, *groupLive, *groupResult;
    QComboBox *comboInputMode;
    QTextEdit *textInput;
    QPushButton *btnRun, *btnStop, *btnGenerateData;
    QLabel *labelLiveInfo;
    QTableWidget *tableResult;

    QTimer *timer;
    size_t processedCount = 0;
    
    LazySequence<std::string>* liveSequence = nullptr;
    ReadOnlyStream<std::string>* liveStream = nullptr;
    AlphavitIndex<ArraySequence>* liveIndex = nullptr;
};

#endif