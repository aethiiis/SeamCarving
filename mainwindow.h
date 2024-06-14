#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QImage>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    int objectif;
    bool vertical;

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    Mat QImageToMat(const QImage& image);
    QImage MatToQImage(const Mat& mat);

    Mat calcuclateEnergy(Mat &image);
    Mat propagateEnergy(Mat &energyImage, bool vertical);
    vector<int> chooseSeam(Mat &propagationImage, bool vertical);
    Mat resizeImage(Mat &image, vector<int> seam, bool vertical);
    QVector<QImage> seamCarving(Mat &image, bool vertical, int objectif);


private:
    Ui::MainWindow *ui;
    QLabel *imageLabel;
    QLabel *energyLabel;
    QLabel *seamLabel;
    QLabel *scaledLabel;
};
#endif // MAINWINDOW_H
