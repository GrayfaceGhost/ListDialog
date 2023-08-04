#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    auto s = QStyleFactory::create("Fusion");
    a.setStyle(s);
    MainWindow w;
    w.show();
    return a.exec();
}
