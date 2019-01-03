#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <opencv.hpp>
#include "RoadSign.h"
#include <QMainWindow>
#include <QFileDialog>

using namespace cv;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void videoCheck();
    void signCheck(Mat frame);
    void laneChaek(Mat frame);

private slots:
    void on_browse_clicked();

private:
    Ui::MainWindow *ui;
    int programNum=0;
    int choice=0;
    QString path=".";
    RoadSign roadsign;
};

#endif // MAINWINDOW_H
