#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QHBoxLayout>
#include <QScreen>

using namespace cv;
using namespace std;

typedef std::pair<int, int> coord;

QDebug operator<<(QDebug debug, const std::map<std::pair<int, int>, int> &map)
{
    QDebugStateSaver saver(debug);  // Save the current state of QDebug
    debug.nospace() << "\n";

    // Determine the dimensions of the image
    int maxX = 0, maxY = 0;
    for (const auto &pair : map) {
        if (pair.first.first > maxX) maxX = pair.first.first;
        if (pair.first.second > maxY) maxY = pair.first.second;
    }

    // Print the map in a grid format
    for (int y = 0; y <= maxY; ++y) {
        for (int x = 0; x <= maxX; ++x) {
            auto it = map.find({x, y});
            if (it != map.end()) {
                debug.nospace() << it->second << " ";
            } else {
                debug.nospace() << "  ";  // Print a space for missing pixels
            }
        }
        debug.nospace() << "\n";  // New line at the end of each row
    }

    return debug;
}

Mat MainWindow::QImageToMat(const QImage& image) {
    switch (image.format()) {
    case QImage::Format::Format_RGB32: {
        cv::Mat mat(image.height(), image.width(), CV_8UC4, const_cast<uchar*>(image.bits()), image.bytesPerLine());
        return mat.clone(); // deep copy
    }
    case QImage::Format::Format_RGB888: {
        QImage swapped = image.rgbSwapped();
        return cv::Mat(swapped.height(), swapped.width(), CV_8UC3, const_cast<uchar*>(swapped.bits()), swapped.bytesPerLine()).clone();
    }
    case QImage::Format::Format_Indexed8: {
        return cv::Mat(image.height(), image.width(), CV_8UC1, const_cast<uchar*>(image.bits()), image.bytesPerLine()).clone();
    }
    default:
        qWarning() << "QImage format not supported for conversion to cv::Mat";
        return cv::Mat();
    }
}

QImage MainWindow::MatToQImage(const Mat& mat) {
    switch (mat.type()) {
    case CV_8UC4: {
        return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB32).copy();
    }
    case CV_8UC3: {
        return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888).rgbSwapped().copy();
    }
    case CV_8UC1: {
        return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8).copy();
    }
    default:
        qWarning() << "cv::Mat type not supported for conversion to QImage";
        return QImage();
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Create the QLabels
    imageLabel = new QLabel(this);
    energyLabel = new QLabel(this);
    seamLabel = new QLabel(this);
    scaledLabel = new QLabel(this);

    // Load the image
    QImage image("/home/alexandre/Desktop/SeamCarving/original.jpeg");
    Mat imageMat = imread("/home/alexandre/Desktop/SeamCarving/original.jpeg");

    // Check if the image was loaded successfully
    if (image.isNull()) {
        imageLabel->setText("Error: Could not load image");
        setCentralWidget(imageLabel);
        return;
    }

    // The amount that needs to be withdrawn
    objectif = 500;
    vertical = true;
    QVector<QImage> images(4);
    images = seamCarving(imageMat, vertical, objectif);

    // Calculate the energy / seams of every pixel in the image
    QImage originalImage = images[0];
    QImage energyImage = images[1];
    QImage propagationImage = images[2];
    QImage resizedImage = images[3];

    // Get screen dimensions
    QList<QScreen*> screens = QGuiApplication::screens();
    QScreen* screen = screens[0];
    QRect dim = screen->availableGeometry();
    int screenWidth = dim.width();
    int screenHeight = dim.height();

    // Calculate the maximum size for each label
    int maxLabelWidth = (screenWidth / 2) - 15;
    int maxLabelHeight = (screenHeight / 2) - 50;

    // Set the fixed size for each label
    imageLabel->setFixedSize(maxLabelWidth, maxLabelHeight);
    energyLabel->setFixedSize(maxLabelWidth, maxLabelHeight);
    seamLabel->setFixedSize(maxLabelWidth, maxLabelHeight);
    if(vertical) {
        scaledLabel->setFixedSize(resizedImage.width(), maxLabelHeight);
    } else {
        scaledLabel->setFixedSize(maxLabelWidth, resizedImage.height());
    }


    // Set the pixmaps for the labels and scale them to fit the labels
    if (!originalImage.isNull()) imageLabel->setPixmap(QPixmap::fromImage(originalImage).scaled(maxLabelWidth, maxLabelHeight, Qt::KeepAspectRatio));
    if (!energyImage.isNull()) energyLabel->setPixmap(QPixmap::fromImage(energyImage).scaled(maxLabelWidth, maxLabelHeight, Qt::KeepAspectRatio));
    if (!propagationImage.isNull()) seamLabel->setPixmap(QPixmap::fromImage(propagationImage).scaled(maxLabelWidth, maxLabelHeight, Qt::KeepAspectRatio));
    if (vertical) {
        if (!resizedImage.isNull()) scaledLabel->setPixmap(QPixmap::fromImage(resizedImage).scaled(scaledLabel->width(), maxLabelHeight, Qt::KeepAspectRatio));
    } else {
        if (!resizedImage.isNull()) scaledLabel->setPixmap(QPixmap::fromImage(resizedImage).scaled(maxLabelWidth, scaledLabel->height(), Qt::KeepAspectRatio));
    }
    // Set scaled contents for the labels to fit the images
    imageLabel->setScaledContents(true);
    energyLabel->setScaledContents(true);
    seamLabel->setScaledContents(true);
    scaledLabel->setScaledContents(true);

    // Create a grid layout and add the labels
    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(imageLabel, 0, 0);
    gridLayout->addWidget(energyLabel, 0, 1);
    gridLayout->addWidget(seamLabel, 1, 0);
    gridLayout->addWidget(scaledLabel, 1, 1);

    // Set stretch factors to control the resizing behavior
    gridLayout->setRowStretch(0, 1);
    gridLayout->setRowStretch(1, 1);
    gridLayout->setColumnStretch(0, 1);
    gridLayout->setColumnStretch(1, 1);

    // Create a central widget and set the layout
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(gridLayout);
    setCentralWidget(centralWidget);

    // Adjust window size to fit screen
    resize(screenWidth, screenHeight);
}

MainWindow::~MainWindow()
{
    delete ui;
}

Mat MainWindow::calcuclateEnergy(cv::Mat &image) {
    Mat blurImage;
    GaussianBlur(image, blurImage, Size(3, 3), 0, 0, BORDER_DEFAULT);

    Mat grayImage;
    cvtColor(blurImage, grayImage, COLOR_BGR2GRAY);

    Mat xGrad, yGrad;
    Scharr(grayImage, xGrad, CV_16S, 1, 0, 1, 0, BORDER_DEFAULT);
    Scharr(grayImage, yGrad, CV_16S, 0, 1, 1, 0, BORDER_DEFAULT);

    Mat xAbsGrad, yAbsGrad;
    convertScaleAbs(xGrad, xAbsGrad);
    convertScaleAbs(yGrad, yAbsGrad);

    Mat grad;
    addWeighted(xAbsGrad, 0.5, yAbsGrad, 0.5, 0, grad);

    Mat energyImage;
    grad.convertTo(energyImage, CV_64F, 1.0/255.0);

    return energyImage;
}

Mat MainWindow::propagateEnergy(Mat &energyImage, bool vertical) {
    double left, center, right;

    int rows = energyImage.rows;
    int cols = energyImage.cols;

    Mat propagationImage = Mat(rows, cols, CV_64F, double(0));

    if (vertical) {
        energyImage.row(0).copyTo(propagationImage.row(0));
        for (int i = 1; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                left = propagationImage.at<double>(i-1, max(j-1, 0));
                center = propagationImage.at<double>(i-1, j);
                right = propagationImage.at<double>(i-1, min(j+1, cols-1));

                propagationImage.at<double>(i, j) = energyImage.at<double>(i, j) + min(left, min(center, right));
            }
        }
    } else {
        energyImage.col(0).copyTo(propagationImage.col(0));
        for (int j = 1; j < cols; j++) {
            for (int i = 0; i < rows; i++) {
                left = propagationImage.at<double>(max(i-1, 0), j-1);
                center = propagationImage.at<double>(i, j-1);
                right = propagationImage.at<double>(min(i+1, rows-1), j-1);

                propagationImage.at<double>(i, j) = energyImage.at<double>(i, j) + min(left, min(center, right));
            }
        }
    }
    return propagationImage;
}

vector<int> MainWindow::chooseSeam(Mat &propagationImage, bool vertical) {
    double left, center, right;

    int rows = propagationImage.rows;
    int cols = propagationImage.cols;

    int offset = 0;

    vector<int> seam;

    if (vertical) {
        Mat lastRow = propagationImage.row(rows-1);

        double minVal, maxVal;
        Point minPt, maxPt;
        minMaxLoc(lastRow, &minVal, &maxVal, &minPt, &maxPt);


        seam.resize(rows);

        int minIndex = minPt.x;
        seam[rows-1] = minIndex;

        for (int i = rows-2; i >= 0; i--) {
            left = propagationImage.at<double>(i, max(minIndex-1, 0));
            center = propagationImage.at<double>(i, minIndex);
            right = propagationImage.at<double>(i, min(minIndex+1,  cols-1));

            if (min(left, center) > right) {
                offset = 1;
            } else if (min(left, right) > center) {
                offset = 0;
            } else if (min(center, right) > left) {
                offset = -1;
            }

            minIndex += offset;
            minIndex = min(max(minIndex, 0), cols-1);
            seam[i] = minIndex;
        }
    } else {
        Mat lastCol = propagationImage.col(cols-1);

        double minVal, maxVal;
        Point minPt, maxPt;
        minMaxLoc(lastCol, &minVal, &maxVal, &minPt, &maxPt);

        vector<int> seam;
        seam.resize(cols);

        int minIndex = minPt.y;
        seam[cols-1] = minIndex;

        for (int i = cols-2; i >= 0; i--) {
            left = propagationImage.at<double>(max(minIndex-1, 0), i);
            center = propagationImage.at<double>(minIndex, i);
            right = propagationImage.at<double>(min(minIndex+1,  cols-1), i);

            if (min(left, center) > right) {
                offset = 1;
            } else if (min(left, right) > center) {
                offset = 0;
            } else if (min(center, right) > left) {
                offset = -1;
            }

            minIndex += offset;
            minIndex = min(max(minIndex, 0), rows-1);
            seam[i] = minIndex;
        }
    }
    return seam;
}

Mat MainWindow::resizeImage(Mat &image, vector<int> seam, bool vertical) {
    int rows = image.rows;
    int cols = image.cols;

    Mat resizedImage;

    Mat placeholder(1, 1, CV_8UC3, Vec3b(0, 0, 0));

    if (vertical) {
        for (int i = 0; i < rows; i++) {
            Mat newRow;
            Mat lower = image.rowRange(i, i+1).colRange(0, seam[i]);
            Mat upper = image.rowRange(i, i+1).colRange(seam[i]+1, cols);

            if (!lower.empty() && !upper.empty()) {
                hconcat(lower, upper, newRow);
                hconcat(newRow, placeholder, newRow);
            } else if (lower.empty()) {
                hconcat(upper, placeholder, newRow);
            } else if (upper.empty()) {
                hconcat(lower, placeholder, newRow);
            }
            newRow.copyTo(image.row(i));
        }
        resizedImage = image.colRange(0, cols-1);
    } else {
        for (int i = 0; i < cols; i++) {
            Mat newCol;
            Mat lower = image.colRange(i, i+1).rowRange(0, seam[i]);
            Mat upper = image.colRange(i, i+1).rowRange(seam[i]+1, cols);

            if (!lower.empty() && !upper.empty()) {
                vconcat(lower, upper, newCol);
                vconcat(newCol, placeholder, newCol);
            } else if (lower.empty()) {
                vconcat(upper, placeholder, newCol);
            } else if (upper.empty()) {
                vconcat(lower, placeholder, newCol);
            }
            newCol.copyTo(image.col(i));
        }
        resizedImage = image.rowRange(0, rows-1);
    }

    return resizedImage;
}

QVector<QImage> MainWindow::seamCarving(Mat &image, bool vertical, int objectif) {
    QVector<QImage> images(4);
    images[0] = MatToQImage(image);

    Mat energyImage;
    Mat propagationImage;
    vector<int> seam;
    Mat resizedImage = image;
    image.copyTo(resizedImage);
    for (int i = 0; i < objectif; i++) {
        energyImage = calcuclateEnergy(resizedImage);
        propagationImage = propagateEnergy(energyImage, vertical);
        seam = chooseSeam(propagationImage, vertical);
        resizedImage = resizeImage(resizedImage, seam, vertical);

        if (i == 0) {
            Mat colorEnergyImage;
            double minEnergy, maxEnergy;
            minMaxLoc(energyImage, &minEnergy, &maxEnergy);
            float scaleEnergy = 255.0 / (maxEnergy - minEnergy);
            energyImage.convertTo(colorEnergyImage, CV_8UC1, scaleEnergy);
            applyColorMap(colorEnergyImage, colorEnergyImage, cv::COLORMAP_JET);
            images[1] = MatToQImage(colorEnergyImage);

            Mat colorPropagationImage;
            double minPropagation, maxPropagation;
            minMaxLoc(propagationImage, &minPropagation, &maxPropagation);
            float scalePropagation = 255.0 / (maxPropagation - minPropagation);
            propagationImage.convertTo(colorPropagationImage, CV_8UC1, scalePropagation);
            applyColorMap(colorPropagationImage, colorPropagationImage, cv::COLORMAP_JET);
            images[2] = MatToQImage(colorPropagationImage);
        }
        if (i == objectif - 1) {
            images[3] = MatToQImage(resizedImage);
            imwrite("/home/alexandre/Desktop/SeamCarving/result.jpg", resizedImage);
        }
    }

    return images;
}
