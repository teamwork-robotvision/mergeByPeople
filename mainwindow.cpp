#include "function.h"
#include <thread>
extern sem_t semKey;
void videoCheck(MainWindow *p);
void signCheck(Mat frame);
void laneCheck(Mat frame);

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

string MainWindow::getVideoPath(){
    return videoPath;
}

void MainWindow::on_browse_clicked()
{
    if(choice>0){
        QMessageBox::information(nullptr,tr("warning"), tr("please stop the video-check program"));
        return;
    }

    ui->console->append(tr("chose the browse\n"));
    ui->console->moveCursor(QTextCursor::End);

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
        videoPath=path.toStdString();
        //QMessageBox::information(nullptr, tr("Path"), tr("You selected ") + path);
        ui->console->append(tr("You selected ---> ") + path + tr("\n"));
        ui->console->moveCursor(QTextCursor::End);

        ui->console->append(tr("please coose a mode\n"));
        ui->console->moveCursor(QTextCursor::End);
        choice=0;
        cv::VideoCapture cap(videoPath);
        Mat frame;
        cap.read(frame);
        cvtColor( frame, frame, CV_BGR2RGB);
        QImage image = QImage( (const unsigned char*)(frame.data), frame.cols, frame.rows, QImage::Format_RGB888 );
        ui->outputImage->setPixmap( QPixmap::fromImage(image.scaled(ui->outputImage->width(),ui->outputImage->height())));
        //ui->outputImage->resize(370,200);
        ui->outputImage->show();
    } else {
        QMessageBox::information(nullptr, tr("warning"), tr("You didn't select any files"));
    }
}

MainWindow* MainWindow::ptr=nullptr;

Ui::MainWindow* MainWindow::getUI(){
    return ui;
}

void MainWindow::changeChoice(int operation)
{
    pthread_mutex_lock(&choiceKey);
    choice=operation;
    pthread_mutex_unlock(&choiceKey);
}

void MainWindow::on_lane_only_clicked()
{
    if(choice<0){
        QMessageBox::information(nullptr,tr("warning"),tr("please choose a video first"));
        return;
    }

    if(choice==0){
        changeChoice(1);
        sem_post(&semKey);
        //QtConcurrent::run(MainWindow::videoCheck,MainWindow::ptr);
    }

    ui->console->append(tr("chose the lane_only\n"));
    ui->console->moveCursor(QTextCursor::End);
    changeChoice(1);
}

void MainWindow::on_pause_clicked()
{
    if(choice<1){
        QMessageBox::information(nullptr,tr("warning"),tr("please choose a video and mode"));
        return;
    }

    if(choice==4){
        ui->console->append(tr("chose the start\n"));
        ui->console->moveCursor(QTextCursor::End);
        changeChoice(choiceSave);
        return;
    }
    ui->console->append(tr("chose the pause\n"));
    ui->console->moveCursor(QTextCursor::End);
    choiceSave=choice;
    changeChoice(4);
}

void MainWindow::on_sign_only_clicked()
{
    if(choice<0){
        QMessageBox::information(nullptr,tr("warning"),tr("please choose a video and mode"));
        return;
    }

    if(choice==0){
        changeChoice(2);
        sem_post(&semKey);
        //QtConcurrent::run(MainWindow::videoCheck,MainWindow::ptr);
    }

    ui->console->append(tr("chose the sign_only\n"));
    ui->console->moveCursor(QTextCursor::End);
    changeChoice(2);
}

void MainWindow::on_both_clicked()
{
    if(choice<0){
        QMessageBox::information(nullptr,tr("warning"),tr("please choose a video and mode"));
        return;
    }

    if(choice==0){
        changeChoice(3);
        sem_post(&semKey);
        //QtConcurrent::run(MainWindow::videoCheck,MainWindow::ptr);
    }

    ui->console->append(tr("chose the both\n"));
    ui->console->moveCursor(QTextCursor::End);
    changeChoice(3);
}

void MainWindow::on_stop_clicked()
{
    if(choice<1){
        QMessageBox::information(nullptr,tr("warning"),tr("please choose a video and mode"));
        return;
    }

    ui->console->append(tr("chose the stop\n"));
    ui->console->moveCursor(QTextCursor::End);
    changeChoice(-1);
}

int MainWindow::getChoice(){
    return choice;
}

void MainWindow::setChoice(int cho){
    choice=cho;
}
