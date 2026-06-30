#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QMessageBox>
#include <QHeaderView>
#include <QFile>
#include <QTextStream>
#include <QStringConverter>
#include <sstream>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent){
    setWindowTitle(tr("Alphabet Index / Streams"));
    resize(900, 750);

    timer = new QTimer(this);
    timer->setInterval(150); 
    connect(timer, &QTimer::timeout, this, &MainWindow::onTimerTick);

    tabs = new QTabWidget(this);
    tabs->setGeometry(0, 0, 900, 750);
    tabIndex = new QWidget();
    tabs->addTab(tabIndex, tr("Alphabet Index"));
    setupIndexTab();
}

MainWindow::~MainWindow(){
    clearLiveResources();
}

void MainWindow::clearLiveResources(){
    delete liveSequence; 
    liveSequence = nullptr;
    delete liveStream;   
    liveStream = nullptr;
    delete liveIndex; 
    liveIndex = nullptr;
}

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

    comboInputMode = new QComboBox();
    comboInputMode->addItem(tr("Manual Input"));
    comboInputMode->addItem(tr("Stream Input"));
    lay->addWidget(new QLabel(tr("Mode:")));
    lay->addWidget(comboInputMode);

    groupManual = new QGroupBox(tr("Manual Input"));
    QVBoxLayout *manLay = new QVBoxLayout(groupManual);
    textInput = new QTextEdit();
    btnGenerateData = new QPushButton(tr("Generate Test Data"));
    manLay->addWidget(textInput);
    manLay->addWidget(btnGenerateData);
    lay->addWidget(groupManual);

    groupLive = new QGroupBox(tr("Stream Status"));
    QVBoxLayout *livLay = new QVBoxLayout(groupLive);
    labelLiveInfo = new QLabel(tr("Processed words: 0"));
    livLay->addWidget(labelLiveInfo);
    groupLive->setVisible(false);
    lay->addWidget(groupLive);

    QHBoxLayout *btnRow = new QHBoxLayout();
    btnRun = new QPushButton(tr("Run"));
    btnStop = new QPushButton(tr("Stop"));
    btnStop->setEnabled(false);
    btnRow->addWidget(btnRun);
    btnRow->addWidget(btnStop);
    lay->addLayout(btnRow);

    groupResult = new QGroupBox(tr("Result"));
    QVBoxLayout *resLay = new QVBoxLayout(groupResult);
    tableResult = new QTableWidget(0, 3);
    tableResult->setHorizontalHeaderLabels({tr("Word"), tr("Count"), tr("Positions")});
    tableResult->setEditTriggers(QAbstractItemView::NoEditTriggers);
    normalizeTable(tableResult);
    resLay->addWidget(tableResult);
    lay->addWidget(groupResult, 3);

    connect(comboInputMode, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onInputModeChanged);
    connect(btnGenerateData, &QPushButton::clicked, this, &MainWindow::onGenerateTestData);
    connect(btnRun, &QPushButton::clicked, this, &MainWindow::onRun);
    connect(btnStop, &QPushButton::clicked, this, &MainWindow::onStop);
}

void MainWindow::onInputModeChanged(int index){
    if (index == 1){
        groupManual->setVisible(false);
        groupLive->setVisible(true);
    }
    else{
        groupManual->setVisible(true);
        groupLive->setVisible(false);
    }
    onStop();
    tableResult->setRowCount(0);
}

void MainWindow::onGenerateTestData(){
    QFile file(":/poem.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream in(&file);
        in.setEncoding(QStringConverter::Utf8);
        textInput->setPlainText(in.readAll());
    }
    else{
        textInput->setPlainText("Ошибка: файл poem.txt не найден.");
    }
}

void MainWindow::populateTable(AlphavitIndex<ArraySequence>* index){
    if (index == nullptr){
        return;
    }
    
    // Используем явный тип для entries, чтобы избежать 'auto'
    auto entries = index->GetAllEntries();
    tableResult->setRowCount(entries.GetLength());

    for (size_t i = 0; i < entries.GetLength(); i++){
        auto entry = entries.Get(i);
        tableResult->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(entry.elem1)));
        tableResult->setItem(i, 1, new QTableWidgetItem(QString::number(entry.elem2.GetLength())));
        
        QString pos;
        for (size_t p = 0; p < entry.elem2.GetLength(); p++){
            pos += QString::number(entry.elem2.Get(p));
            if (p < entry.elem2.GetLength() - 1){
                pos += ", ";
            }
        }
        tableResult->setItem(i, 2, new QTableWidgetItem("[ " + pos + " ]"));
    }
    tableResult->resizeColumnsToContents();
}

void MainWindow::executeManualIndexing(){
    QString qtext = textInput->toPlainText();
    if (qtext.trimmed().isEmpty()){
        QMessageBox::information(this, tr("Info"), tr("Enter text."));
        return;
    }

    std::stringstream ss(qtext.toStdString());
    std::string word;
    ArraySequence<std::string> inputWords;
    while (ss >> word){
        inputWords.Append(word);
    }

    ReadOnlyStream<std::string> stream(&inputWords);
    AlphavitIndex<ArraySequence> index;
    index.BuildFromStream(stream);
    populateTable(&index);
}

void MainWindow::onRun(){
    onStop();
    
    if (comboInputMode->currentIndex() == 0){
        executeManualIndexing();
    }
    else{
        clearLiveResources();
        processedCount = 0;
        tableResult->setRowCount(0);
        labelLiveInfo->setText(tr("Processed words: 0"));
        
        liveIndex = new AlphavitIndex<ArraySequence>();
        
        ArraySequence<std::string> seed;
        seed.Append("start");

        std::vector<std::string> streamData;
        streamData.push_back("apple");
        streamData.push_back("banana");
        streamData.push_back("cherry");
        
        liveSequence = new LazySequence<std::string>([streamData](Sequence<std::string>* c){
            return streamData[c->GetLength() % streamData.size()];
        }, &seed);

        liveStream = new ReadOnlyStream<std::string>(liveSequence);
        btnRun->setEnabled(false);
        btnStop->setEnabled(true);
        timer->start();
    }
}

void MainWindow::onTimerTick(){
    if (liveStream == nullptr){
        onStop();
        return;
    }
    
    std::string word = liveStream->Read();
    
    if (liveIndex != nullptr){
        liveIndex->AddWord(word, processedCount);
        populateTable(liveIndex);
    }
    
    processedCount++;
    labelLiveInfo->setText(tr("Processed words: ") + QString::number(processedCount));
}

void MainWindow::onStop(){
    timer->stop();
    btnRun->setEnabled(true);
    btnStop->setEnabled(false);
}