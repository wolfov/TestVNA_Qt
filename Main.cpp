#include "MainWindow.h"
#include "Handler.h"
#include <QApplication>
#include <QString>
#include <QObject>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
