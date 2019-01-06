#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <opencv.hpp>
#include "RoadSign.h"
#include <QMainWindow>
#include <QtConcurrent/QtConcurrent>
#include <QFileDialog>
#include "QMessageBox"
#include "ui_mainwindow.h"
#include <string>
#include <opencv2/opencv.hpp>
#include <QGraphicsScene>
#include <QWidget>
#include <QImage>
#include <vector>
#include <semaphore.h>

using namespace std;
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
    static void videoCheck(MainWindow *p);
    void signCheck(Mat frame);
    void laneCheck(Mat frame);
    void changeChoice(int operation=0);
    static MainWindow *ptr;
    int getChoice();
    void setChoice(int cho);
    Ui::MainWindow* getUI();

private slots:
    void on_browse_clicked();

    void on_lane_only_clicked();

    void on_pause_clicked();

    void on_sign_only_clicked();

    void on_both_clicked();

    void on_stop_clicked();

private:
    Ui::MainWindow *ui;
    int programNum=0;
    int choice=-1;
    int choiceSave=0;
    std::string videoPath=".";
    RoadSign roadsign;
    pthread_mutex_t choiceKey=PTHREAD_MUTEX_INITIALIZER;
};
#endif // MAINWINDOW_H
