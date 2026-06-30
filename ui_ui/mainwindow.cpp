#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPainter>
#include <QMessageBox>
#include <QHeaderView>
#include <QFile>
#include <QTextStream>
#include <QStringConverter> 
#include <QRegularExpression>
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
    delete liveIndexArr; 
    liveIndexArr = nullptr;
    delete liveIndexList; 
    liveIndexList = nullptr;
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

    groupSettings = new QGroupBox(tr("Settings"));
    QFormLayout *setLay = new QFormLayout(groupSettings);
    comboContainer = new QComboBox();
    comboContainer->addItems({"ArraySequence", "ListSequence"});
    comboInputMode = new QComboBox();
    comboInputMode->addItems({tr("Manual Input"), tr("Live Test Stream")});
    setLay->addRow(tr("Container:"), comboContainer);
    setLay->addRow(tr("Input Mode:"), comboInputMode);
    lay->addWidget(groupSettings);

    groupManual = new QGroupBox(tr("Input Text"));
    QVBoxLayout *manLay = new QVBoxLayout(groupManual);
    textInput = new QTextEdit();
    textInput->setPlaceholderText(tr("Enter your text here..."));
    btnGenerateData = new QPushButton(tr("Generate Test Data"));
    manLay->addWidget(textInput);
    manLay->addWidget(btnGenerateData);
    lay->addWidget(groupManual, 2);

    groupLive = new QGroupBox(tr("Live Data Stream"));
    QVBoxLayout *livLay = new QVBoxLayout(groupLive);
    QHBoxLayout *genRow = new QHBoxLayout();
    comboGen = new QComboBox();
    comboGen->addItems({tr("Random Words"), tr("Repeating Pattern")});
    genRow->addWidget(new QLabel(tr("Generator:")));
    genRow->addWidget(comboGen);
    genRow->addStretch();
    livLay->addLayout(genRow);
    labelLiveInfo = new QLabel(tr("Processed words: 0"));
    labelLiveInfo->setStyleSheet("color: #a0c4ff; font-style: italic;");
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
    } else{
        groupManual->setVisible(true);
        groupLive->setVisible(false);
    }
    onStop();
    tableResult->setRowCount(0);
}

void MainWindow::onGenerateTestData(){
    QFile file(":/poem.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        textInput->setPlainText(QString::fromUtf8(file.readAll()));
    } else{
        textInput->setPlainText("Ошибка: файл poem.txt не найден.");
    }
}

template <template <typename> class Container>
void MainWindow::populateTable(AlphavitIndex<Container>* index){
    if (index == nullptr){
        return;
    }
    auto entries = index->GetAllEntries();
    tableResult->setRowCount(entries.GetLength());

    for (size_t i = 0; i < entries.GetLength(); ++i){
        auto entry = entries.Get(i);
        tableResult->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(entry.elem1)));
        tableResult->setItem(i, 1, new QTableWidgetItem(QString::number(entry.elem2.GetLength())));
        
        QString pos;
        for (size_t p = 0; p < entry.elem2.GetLength(); ++p){
            pos += QString::number(entry.elem2.Get(p));
            if (p < entry.elem2.GetLength() - 1){
                pos += ", ";
            }
        }
        tableResult->setItem(i, 2, new QTableWidgetItem("[ " + pos + " ]"));
    }
    tableResult->resizeColumnsToContents();
}

template <template <typename> class Container>
void MainWindow::executeManualIndexing(){
    try{
        QString qtext = textInput->toPlainText();
        if (qtext.trimmed().isEmpty()){
            QMessageBox::information(this, tr("Info"), tr("Please enter some text."));
            return;
        }

        std::stringstream ss(qtext.toStdString());
        std::string word;
        Container<std::string> inputWords;
        while (ss >> word){
            inputWords.Append(word);
        }

        ReadOnlyStream<std::string> stream(&inputWords);
        AlphavitIndex<Container> index;
        index.BuildFromStream(stream);
        populateTable(&index);
    } 
    catch (const Exception& e){
        QMessageBox::warning(this, tr("Error"), QString::fromUtf8(e.what()));
    }
}

void MainWindow::onRun(){
    onStop();
    if (comboInputMode->currentIndex() == 0){
        if (comboContainer->currentIndex() == 0){
            executeManualIndexing<ArraySequence>();
        } else{
            executeManualIndexing<ListSequence>();
        }
    } else{
        clearLiveResources();
        processedCount = 0;
        tableResult->setRowCount(0);
        labelLiveInfo->setText(tr("Processed words: 0"));

        if (comboContainer->currentIndex() == 0){
            liveIndexArr = new AlphavitIndex<ArraySequence>();
        } 
        else{
            liveIndexList = new AlphavitIndex<ListSequence>();
        }

        ArraySequence<std::string> seed;
        seed.Append("start");

        if (comboGen->currentIndex() == 0){
            std::vector<std::string> poemWords;
            QFile file(":/poem.txt");
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
                QStringList list = QString::fromUtf8(file.readAll()).split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
                for (const QString& w : list){
                    poemWords.push_back(w.toLower().toStdString());
                }
            }
            if (poemWords.empty()){
                poemWords.push_back("ошибка");
            }

            liveSequence = new LazySequence<std::string>([poemWords](Sequence<std::string>*){
                return poemWords[rand() % poemWords.size()];
            }, &seed);
        } 
        else{
            liveSequence = new LazySequence<std::string>([](Sequence<std::string>* c){
                size_t len = c->GetLength();
                if (len % 3 == 0){
                    return std::string("любовь");
                } 
                else if (len % 3 == 1){
                    return std::string("надежда");
                } 
                else{
                    return std::string("слава");
                }
            }, &seed);
        }
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
    
    if (comboContainer->currentIndex() == 0){
        if (liveIndexArr != nullptr){
            liveIndexArr->AddWord(word, processedCount);
            populateTable(liveIndexArr);
        }
    } 
    else{
        if (liveIndexList != nullptr){
            liveIndexList->AddWord(word, processedCount);
            populateTable(liveIndexList);
        }
    }
    
    processedCount++;
    labelLiveInfo->setText(tr("Processed words: ") + QString::number(processedCount));
}

void MainWindow::onStop(){
    timer->stop();
    btnRun->setEnabled(true);
    btnStop->setEnabled(false);
}