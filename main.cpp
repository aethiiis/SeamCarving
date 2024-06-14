#include "mainwindow.h"
#include <opencv4/opencv2/opencv.hpp>
//#include <opencv4/opencv2/core.hpp>
//#include <QLabel>

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
