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

class MainWindow : public QMainWindow{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onRun();
    void onGenerateTestData();

private:
    void setupIndexTab();
    void normalizeTable(QTableWidget* table);

    template <template <typename> class Container>
    void executeIndexing();

    QTabWidget *tabs;
    QWidget *tabIndex;

    QGroupBox *groupSettings;
    QComboBox *comboContainer;

    QGroupBox *groupInput;
    QTextEdit *textInput;
    QPushButton *btnGenerateData;

    QPushButton *btnRun;

    QGroupBox *groupResult;
    QLabel *labelResultTitle;
    QTableWidget *tableResult;
};

#endif