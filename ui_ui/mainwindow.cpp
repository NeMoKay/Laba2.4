#include "mainwindow.h"
#include "AlphabetIndex.hpp"
#include "Stream.hpp"
#include "ArraySequence.hpp"
#include "ListSequence.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QMessageBox>
#include <QHeaderView>
#include <sstream>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent){
    setWindowTitle(tr("Alphabet Index / Streams"));
    resize(900, 700);

    tabs = new QTabWidget(this);
    tabs->setGeometry(0, 0, 900, 700);

    tabIndex = new QWidget();
    tabs->addTab(tabIndex, tr("Alphabet Index"));

    setupIndexTab();
}

MainWindow::~MainWindow() {}

void MainWindow::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    painter.fillRect(rect(), QColor(30, 30, 30));
    QMainWindow::paintEvent(event);
}

void MainWindow::normalizeTable(QTableWidget* table){
    table->horizontalHeader()->setStretchLastSection(true);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    table->verticalHeader()->setVisible(false);
}

void MainWindow::setupIndexTab(){
    QVBoxLayout *lay = new QVBoxLayout(tabIndex);
    lay->setSpacing(12);
    lay->setContentsMargins(16, 16, 16, 16);

    // --- НАСТРОЙКИ ---
    groupSettings = new QGroupBox(tr("Settings"));
    QFormLayout *setLay = new QFormLayout(groupSettings);
    
    comboContainer = new QComboBox();
    comboContainer->addItems({"ArraySequence", "ListSequence"});
    
    setLay->addRow(tr("Container:"), comboContainer);
    lay->addWidget(groupSettings);

    // --- ВВОД ТЕКСТА ---
    groupInput = new QGroupBox(tr("Input Text"));
    QVBoxLayout *inLay = new QVBoxLayout(groupInput);
    
    textInput = new QTextEdit();
    textInput->setPlaceholderText(tr("Enter your text here for stream processing..."));
    inLay->addWidget(textInput);
    
    btnGenerateData = new QPushButton(tr("Generate Test Data"));
    inLay->addWidget(btnGenerateData);
    
    lay->addWidget(groupInput, 2);

    // --- КНОПКА ЗАПУСКА ---
    btnRun = new QPushButton(tr("Build Index"));
    lay->addWidget(btnRun);

    // --- ВЫВОД РЕЗУЛЬТАТА ---
    groupResult = new QGroupBox(tr("Result"));
    QVBoxLayout *resLay = new QVBoxLayout(groupResult);
    
    labelResultTitle = new QLabel(tr("Index Table:"));
    labelResultTitle->setProperty("class", "ResultLabel");
    
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(6);
    shadow->setColor(Qt::black);
    shadow->setOffset(1, 1);
    labelResultTitle->setGraphicsEffect(shadow);
    resLay->addWidget(labelResultTitle);
    
    tableResult = new QTableWidget(0, 3);
    tableResult->setHorizontalHeaderLabels({tr("Word"), tr("Count"), tr("Positions")});
    tableResult->setEditTriggers(QAbstractItemView::NoEditTriggers);
    normalizeTable(tableResult);
    resLay->addWidget(tableResult);
    
    lay->addWidget(groupResult, 3);

    connect(btnGenerateData, &QPushButton::clicked, this, &MainWindow::onGenerateTestData);
    connect(btnRun, &QPushButton::clicked, this, &MainWindow::onRun);
}

void MainWindow::onGenerateTestData() {
    // Генерируем тестовый поток слов для демонстрации
    QString testData = "apple banana apple cherry dog elephant banana cherry cherry lazy stream lazy test stream dog apple";
    textInput->setPlainText(testData);
}

template <template <typename> class Container>
void MainWindow::executeIndexing() {
    try {
        QString qtext = textInput->toPlainText();
        if (qtext.trimmed().isEmpty()) {
            QMessageBox::information(this, tr("Info"), tr("Please enter some text."));
            return;
        }

        std::string text = qtext.toStdString();
        std::stringstream ss(text);
        std::string word;
        
        Container<std::string> inputWords;
        while (ss >> word) {
            inputWords.Append(word);
        }

        // Подключаем ReadOnlyStream к нашей последовательности
        ReadOnlyStream<std::string> stream(&inputWords);

        // Строим алфавитный указатель
        AlphavitIndex<Container> index;
        index.BuildFromStream(stream);

        // Получаем результаты
        auto entries = index.GetAllEntries();

        tableResult->setRowCount(0);
        tableResult->setRowCount(entries.GetLength());

        for (size_t i = 0; i < entries.GetLength(); ++i) {
            auto entry = entries.Get(i);
            
            QTableWidgetItem *itemWord = new QTableWidgetItem(QString::fromStdString(entry.elem1));
            tableResult->setItem(i, 0, itemWord);
            
            size_t count = entry.elem2.GetLength();
            QTableWidgetItem *itemCount = new QTableWidgetItem(QString::number(count));
            tableResult->setItem(i, 1, itemCount);
            
            QString positionsStr = "[ ";
            for (size_t p = 0; p < count; ++p) {
                positionsStr += QString::number(entry.elem2.Get(p));
                if (p < count - 1) positionsStr += ", ";
            }
            positionsStr += " ]";
            
            QTableWidgetItem *itemPos = new QTableWidgetItem(positionsStr);
            tableResult->setItem(i, 2, itemPos);
        }
        
        tableResult->resizeColumnsToContents();

    } catch (const Exception& e) {
        QMessageBox::warning(this, tr("Error"), QString::fromUtf8(e.what()));
    } catch (...) {
        QMessageBox::warning(this, tr("Error"), tr("An unknown error occurred."));
    }
}

void MainWindow::onRun(){
    if (comboContainer->currentIndex() == 0){
        executeIndexing<ArraySequence>();
    } else {
        executeIndexing<ListSequence>();
    }
}