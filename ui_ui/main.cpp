#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QStyleFactory>
#include <QFile>

int main(int argc, char *argv[]){
    QApplication a(argc, argv);
    a.setStyle(QStyleFactory::create("Fusion"));

    QTranslator translator;
    QString locale = QLocale::system().name(); 
    
    if (translator.load(":/i18n/" + locale + ".qm")){
        a.installTranslator(&translator);
    } 

    else if (translator.load(":/i18n/ru_RU.qm")){
        a.installTranslator(&translator);
    }

    QFile styleFile(":/style.qss");
    if (styleFile.open(QFile::ReadOnly)){
        a.setStyleSheet(styleFile.readAll());
    }

    MainWindow w;
    w.show();

    return a.exec();
}