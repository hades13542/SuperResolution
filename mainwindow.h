#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/superres.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/superres/optical_flow.hpp>
#include <iostream>
#include <string>

#include <QFileDialog>
#include <QMessageBox>
#include <QTime>
#include <QDebug>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void on_startButton_clicked();
    void on_pushButton_clicked();
    void prepareVideoFile();
    void importCreatedVideoAsInput();
    void performSuperResolution();
    void on_iterationChanger_valueChanged(int arg1);

    void on_listWidget_activated(const QModelIndex &index);

    void on_listWidget_clicked(const QModelIndex &index);

    void on_checkBox_clicked(bool checked);

    void on_spinBox_valueChanged(int arg1);

    void on_spinBox_2_valueChanged(int arg1);

    void on_spinBox_3_valueChanged(int arg1);

    void on_spinBox_4_valueChanged(int arg1);

    void on_lineEdit_textEdited(const QString &arg1);

private:
    bool addNoise = false;
    QString filename = "Output.jpg";
    QString sourceImage;
    int scale = 2;
    int iterationCount = 5;
    int temporalAreaRadius = 0;
    int blurSigma = 0;
    int blurKernelSize = 1;
    Ui::MainWindow *ui;
    cv::VideoWriter writer;
    cv::Mat source;
    cv::Ptr<cv::superres::FrameSource> frameSource;
    cv::Ptr<cv::superres::SuperResolution> superRes = cv::superres::createSuperResolution_BTVL1();
    cv::Ptr<cv::superres::DenseOpticalFlowExt> of = cv::superres::createOptFlow_Farneback();
};

#endif // MAINWINDOW_H
