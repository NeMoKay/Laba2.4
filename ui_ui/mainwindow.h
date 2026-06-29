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

#include "AlphabetIndex.hpp"
#include "ArraySequence.hpp"
#include "ListSequence.hpp"
#include "LazySequence.hpp"
#include "Stream.hpp"

class MainWindow : public QMainWindow{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void paintEvent(QPaintEvent *event) override;

    void onRun();
    void onStop();
    void onInputModeChanged(int index);
    void onTimerTick();
    void onGenerateTestData();

private:
    void setupIndexTab();
    void normalizeTable(QTableWidget* table);

    template <template <typename> class Container>
    void executeManualIndexing();

    template <template <typename> class Container>
    void populateTable(AlphavitIndex<Container>* index);

    QTabWidget *tabs;
    QWidget *tabIndex;

    QGroupBox *groupSettings;
    QComboBox *comboContainer;
    QComboBox *comboInputMode;

    QGroupBox *groupManual;
    QTextEdit *textInput;
    QPushButton *btnGenerateData;

    QGroupBox *groupLive;
    QComboBox *comboGen;
    QLabel *labelLiveInfo;

    QPushButton *btnRun;
    QPushButton *btnStop;

    QGroupBox *groupResult;
    QLabel *labelResultTitle;
    QTableWidget *tableResult;




    QTimer *timer;
    size_t processedCount = 0;
    
    LazySequence<std::string>* liveSequence = nullptr;
    ReadOnlyStream<std::string>* liveStream = nullptr;

    AlphavitIndex<ArraySequence>* liveIndexArr = nullptr;
    AlphavitIndex<ListSequence>* liveIndexList = nullptr;
};

#endif