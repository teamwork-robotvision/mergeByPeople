#include "mainwindow.h"
#include "QFileDialog"
#include "QMessageBox"
#include "ui_mainwindow.h"
#include <string>
#include <opencv.hpp>
#include "RoadSign.h"
using namespace cv;
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_browse_clicked()
{
    //定义文件对话框类
    QFileDialog *fileDialog = new QFileDialog();
    //定义文件对话框标题
    fileDialog->setWindowTitle(tr("open the video"));
    //设置默认文件路径
    fileDialog->setDirectory(".");
    //设置文件过滤器
    fileDialog->setNameFilter(tr("video(*.wmv *.asf *.rm *.rmvb *.mov *.avi *.dat *.mpg *.mpeg *.mp4 *.mp4v)"));
    //设置可以选择多个文件,默认为只能选择一个文件QFileDialog::ExistingFiles
    fileDialog->setFileMode(QFileDialog::ExistingFiles);
    //设置视图模式
    fileDialog->setViewMode(QFileDialog::Detail);
    if(fileDialog->exec() == QDialog::Accepted) {
        QString path = fileDialog->selectedFiles()[0];
        //QMessageBox::information(nullptr, tr("Path"), tr("You selected ") + path);
        ui->console->append(tr("You selected ---> ") + path + tr("\n"));
        ui->console->moveCursor(QTextCursor::End);

        ui->console->append(tr("please click the button called start to run the program\n"));
        ui->console->moveCursor(QTextCursor::End);
    } else {
        QMessageBox::information(nullptr, tr("Path"), tr("You didn't select any files."));
    }
}

void MainWindow::signCheck(Mat frame){
    //红色路标检测参数
    double colorLimit_red[]={156,43,46,180,255,255};
    int binaryLimit_red[]={235,255};
    int erodeNum_red[]={3,3};
    int dilateNum_red[]={5,5};
    int closeNum_red[]={10,10};
    double angleLimit_red[]={-90/CV_PI,90/CV_PI};
    double wid_hei_red[]={1,5};
    int sizeOfArea_red=5000;

    //黄色路标检测参数
    double colorLimit_yellow[]={20,43,46,24,255,255};
    int binaryLimit_yellow[]={220,255};
    int erodeNum_yellow[]={3,3};
    int dilateNum_yellow[]={5,5};
    int closeNum_yellow[]={3,3};
    double angleLimit_yellow[]={-50/CV_PI,50/CV_PI};
    double wid_hei_yellow[]={0.9,3.1};
    int sizeOfArea_yellow=3500;

    //蓝色路标检测参数
    double colorLimit_blue[]={102.5,43,46,124,255,255};
    int binaryLimit_blue[]={235,255};
    int erodeNum_blue[]={3,3};
    int dilateNum_blue[]={7,7};
    int closeNum_blue[]={10,10};
    double angleLimit_blue[]={-90/CV_PI,90/CV_PI};
    double wid_hei_blue[]={0.91,4.5};
    int sizeOfArea_blue=50000;

    //绿色路标检测参数
    double colorLimit_green[]={96,43,46,99,255,255};
    int binaryLimit_green[]={235,255};
    int erodeNum_green[]={3,3};
    int dilateNum_green[]={7,7};
    int closeNum_green[]={10,10};
    double angleLimit_green[]={-30/CV_PI,30/CV_PI};
    double wid_hei_green[]={1,1};
    int sizeOfArea_green=3500;

    roadsign.checkRoadSign(frame,frame,colorLimit_red,binaryLimit_red,erodeNum_red,dilateNum_red,closeNum_red,angleLimit_red,wid_hei_red,sizeOfArea_red);
    roadsign.checkRoadSign(frame,frame,colorLimit_yellow,binaryLimit_yellow,erodeNum_yellow,dilateNum_yellow,closeNum_yellow,angleLimit_yellow,wid_hei_yellow,sizeOfArea_yellow);
    roadsign.checkRoadSign(frame,frame,colorLimit_blue,binaryLimit_blue,erodeNum_blue,dilateNum_blue,closeNum_blue,angleLimit_blue,wid_hei_blue,sizeOfArea_blue);
    roadsign.checkRoadSign(frame,frame,colorLimit_green,binaryLimit_green,erodeNum_green,dilateNum_green,closeNum_green,angleLimit_green,wid_hei_green,sizeOfArea_green);
}

void MainWindow::laneCheck(Mat frame){

}
