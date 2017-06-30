#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace cv;
using namespace std;

Mat sp_noise(Mat& image) {
    double average = 0.0;
    double standard_deviation = 10.0;
    Mat noise_img(image.size(), CV_16SC3);
    randn(noise_img, Scalar::all(average), Scalar::all(standard_deviation));
    Mat temp_img;
    image.convertTo(temp_img, CV_16SC3);
    addWeighted(temp_img, 1.0, noise_img, 1.0, 0.0, temp_img);
    temp_img.convertTo(image, image.type());
    cv::imshow("Zaladowany obraz", image);
    return image;
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_startButton_clicked()
{
    superRes->clear();
    while(true) {
        try {
            source = imread(sourceImage.toStdString(),1);

            qDebug() << QTime::currentTime().toString() << "iterationCount " <<iterationCount;
            qDebug() << QTime::currentTime().toString() << "addNoise " <<addNoise;

            prepareVideoFile();
            importCreatedVideoAsInput();
            break;
        } catch (cv::Exception e) {
            if(sourceImage.isEmpty()) {
                QMessageBox::information(this,tr("Blad"),"Obraz nie zostal zaladowany!");
                return;
            } else {
                QMessageBox::information(this,tr("Blad"),sourceImage + "jest niepoprawnym plikiem!");
                return;
            }
        }
    }
    qDebug() << QTime::currentTime().toString() << "Rozpoczecie dzialania algorytmu!";
    performSuperResolution();
}

void MainWindow::performSuperResolution() {

    superRes->setScale(scale);
    superRes->setTemporalAreaRadius(temporalAreaRadius);
    superRes->setBlurSigma(blurSigma);
    superRes->setBlurKernelSize(blurKernelSize);
    Mat result;

    QTime timer;
    timer.start();
    do {
        qDebug() << QTime::currentTime().toString() << "Rozpoczynam liczenie" << endl;
        superRes->nextFrame(result);
    } while (result.empty());

    QMessageBox::information(this,tr("Zakonczono!"),"Czas wykonania: " + QString::number(timer.elapsed()/1000.0,'f',2) + " sekund");
    qDebug() << QTime::currentTime().toString() << "zapisuje do pliku";
    imwrite(filename.toStdString(), result);
    Mat res = imread(filename.toStdString(),1);
    cv::imshow("Result", res);
    qDebug() << QTime::currentTime().toString() << "Proces zakonczony";
}

void MainWindow::prepareVideoFile() {
    cv::VideoWriter writer;
    Mat display = Mat::zeros(source.size(), CV_8UC3);
    writer.open("temporary.avi", 1, 2, source.size(), true);
    for (int j = 0; j < 20; j++) {
        if(addNoise) {
            display = sp_noise(source);
        } else {
            display = source;
            cv::imshow("Zaladowany obraz", source);
        }
        writer.write(display);
    }
    writer.release();
}

void MainWindow::importCreatedVideoAsInput() {
    if (!frameSource) {
        frameSource = superres::createFrameSource_Video("temporary.avi");
        qDebug() << QTime::currentTime().toString() << "temporary plik otwarty poprawnie" <<endl;
    }
    superRes->setInput(frameSource);
}

void MainWindow::on_pushButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("Open file"),"C://","JPEG files (*.jpg);;PNG files (*.png);;");
    cv::Mat image = imread(filename.toStdString());
    if(image.size().height <0 || image.size().width < 0) {
        QMessageBox::information(this,tr("Blad"),filename + "jest niepoprawnym plikiem!");
    } else {
        if(filename.isEmpty()) {
            QMessageBox::information(this,tr("Blad"),"Nie wyrbano pliku!");
        } else {
            sourceImage = filename;
            QMessageBox::information(this,tr("Blad"),filename + " załadowany pomyslnie!");
        }
    }
}

void MainWindow::on_iterationChanger_valueChanged(int value)
{
    iterationCount = value;
}

void MainWindow::on_listWidget_activated(const QModelIndex &index)
{
    //DO NOTHING
}

void MainWindow::on_listWidget_clicked(const QModelIndex &index)
{
    qDebug() << QTime::currentTime().toString() << "algorithmID " <<index.row();
    switch(index.row()) {
    case 0:
        superRes = superres::createSuperResolution_BTVL1();
        of = cv::superres::createOptFlow_Farneback();
        break;
    case 1:
        superRes = superres::createSuperResolution_BTVL1();
        of = cv::superres::createOptFlow_DualTVL1();
        break;
        // if CUDA possible
        //    case 2:
        //        superRes = superres::createSuperResolution_BTVL1_CUDA();
        //        of = cv::superres::createOptFlow_Farneback_CUDA();
        //        break;
        //    case 3:
        //        superRes = superres::createSuperResolution_BTVL1_CUDA();
        //        of = cv::superres::createOptFlow_DualTVL1_CUDA();
        //        break;
        //    case 4:
        //        superRes = superres::createSuperResolution_BTVL1_CUDA();
        //        of = cv::superres::createOptFlow_Brox_CUDA();
        //        break;
        //    case 5:
        //        superRes = superres::createSuperResolution_BTVL1_CUDA();
        //        of = cv::superres::createOptFlow_PyrLK_CUDA();
        //        break;
    }
}

void MainWindow::on_checkBox_clicked(bool checked)
{
    addNoise = checked;
    qDebug() << QTime::currentTime().toString() << "addNoise " <<addNoise;
}

void MainWindow::on_spinBox_valueChanged(int arg1)
{
    scale = arg1;
}

void MainWindow::on_spinBox_2_valueChanged(int arg1)
{
    temporalAreaRadius = arg1;
}

void MainWindow::on_spinBox_3_valueChanged(int arg1)
{
    blurSigma = arg1;
}

void MainWindow::on_spinBox_4_valueChanged(int arg1)
{
    blurKernelSize = arg1;
}

void MainWindow::on_lineEdit_textEdited(const QString &arg1)
{
    filename = arg1 + ".jpg";
}
