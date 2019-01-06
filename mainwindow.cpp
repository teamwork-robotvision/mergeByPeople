#include "mainwindow.h"
#include <thread>
extern sem_t semKey;

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

void MainWindow::signCheck(Mat frame){
    //红色路标检测参数
    double colorLimit_red[]={156.7,43,46,180,255,255};
    int binaryLimit_red[]={235,255};
    int erodeNum_red[]={3,3};
    int dilateNum_red[]={5,5};
    int closeNum_red[]={10,10};
    double angleLimit_red[]={-90/CV_PI,90/CV_PI};
    double wid_hei_red[]={0.9,1.5};
    int sizeOfArea_red=2000;

    //黄色路标检测参数
    double colorLimit_yellow[]={22.9,43,46,24,255,255};
    int binaryLimit_yellow[]={220,255};
    int erodeNum_yellow[]={3,3};
    int dilateNum_yellow[]={5,5};
    int closeNum_yellow[]={3,3};
    double angleLimit_yellow[]={-80/CV_PI,80/CV_PI};
    double wid_hei_yellow[]={0.9,3};
    int sizeOfArea_yellow=2000;

    //蓝色路标检测参数
    double colorLimit_blue[]={100,43,46,124,255,255};
    int binaryLimit_blue[]={220,255};
    int erodeNum_blue[]={3,3};
    int dilateNum_blue[]={7,7};
    int closeNum_blue[]={10,10};
    double angleLimit_blue[]={-110/CV_PI,110/CV_PI};
    double wid_hei_blue[]={0.91,4.5};
    int sizeOfArea_blue=30000;

    //绿色路标检测参数
    double colorLimit_green[]={97,43,46,99,255,255};
    int binaryLimit_green[]={225,255};
    int erodeNum_green[]={3,3};
    int dilateNum_green[]={7,7};
    int closeNum_green[]={10,10};
    double angleLimit_green[]={-90/CV_PI,90/CV_PI};
    double wid_hei_green[]={0.7,1.7};
    int sizeOfArea_green=2000;

    roadsign.checkRoadSign(frame,frame,colorLimit_red,binaryLimit_red,erodeNum_red,dilateNum_red,closeNum_red,angleLimit_red,wid_hei_red,sizeOfArea_red);
    roadsign.checkRoadSign(frame,frame,colorLimit_yellow,binaryLimit_yellow,erodeNum_yellow,dilateNum_yellow,closeNum_yellow,angleLimit_yellow,wid_hei_yellow,sizeOfArea_yellow);
    roadsign.checkRoadSign(frame,frame,colorLimit_blue,binaryLimit_blue,erodeNum_blue,dilateNum_blue,closeNum_blue,angleLimit_blue,wid_hei_blue,sizeOfArea_blue);
    roadsign.checkRoadSign(frame,frame,colorLimit_green,binaryLimit_green,erodeNum_green,dilateNum_green,closeNum_green,angleLimit_green,wid_hei_green,sizeOfArea_green);
}

void MainWindow::laneCheck(Mat frame){
    LaneDetector lanedetector;          // 创建车道检测类

       cv::Mat selected_img;
       cv::Mat img_denoise;
       cv::Mat img_edges;
       cv::Mat img_mask;
       cv::Mat img_wrape;
       cv::Mat M;
       cv::Mat img_lines;
       std::vector<cv::Vec4i> lines;
       std::vector<std::vector<cv::Vec4i> > left_right_lines;
       std::vector<cv::Point> lane;
       std::string turn;
       int flag_plot = -1;
       int i = 0;
       //选取颜色区域
              selected_img=lanedetector.selectColor(frame);

              // 高斯滤波
              img_denoise = lanedetector.deNoise(selected_img);

              // 边缘检测
              img_edges = lanedetector.edgeDetector(img_denoise);

              // 获得ROI(感兴趣区域）＋　滤波
              img_mask = lanedetector.mask(img_edges);
              img_mask=lanedetector.deNoise(img_mask);
              cv::Mat temp=img_mask.clone();

              // 在处理后的图像中获得霍夫线
              lines = lanedetector.houghLines(img_mask);

              if (!lines.empty())
              {
                  // 检测车道是转向
                  left_right_lines = lanedetector.lineSeparation(lines, img_edges);

                  // 应用回归在车道的每一侧获得唯一的线
                  lane = lanedetector.regression(left_right_lines, frame);

                  // 通过确定线的消失点来预测转弯
                  turn = lanedetector.predictTurn();

                  // 在视频中显示方向
                  flag_plot = lanedetector.plotLane(frame, lane, turn);
                  i += 1;
              }
              else {
                  flag_plot = -1;
              }

}

MainWindow* MainWindow::ptr=nullptr;

Ui::MainWindow* MainWindow::getUI(){
    return ui;
}

void MainWindow::videoCheck(MainWindow *p){
    cv::VideoCapture cap(p->videoPath);            //读入视频
    if (!cap.isOpened()){
        QMessageBox::information(nullptr, tr("Path"), tr("can not open the video"));
        return;
    }

    //读取视频帧数
    double rate = cap.get(CV_CAP_PROP_FPS);
    // 获取视频帧的尺寸
    int width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
    int height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);

    //根据打开视频的参数初始化输出视频格式
    cv::VideoWriter lane_detor("./detect_out/lane_detected_orignal.mp4", CV_FOURCC('m', 'p', '4', 'v'), rate, cv::Size(width, height));

    cv::Mat frame;
    while (1){
        cout<<p->choice<<endl;
        //读取视频
        if (!cap.read(frame)||p->choice<1)
            break;
        switch(p->choice){
        case 1:
            p->laneCheck(frame);                        
            break;
        case 2:
            p->signCheck(frame);
            break;
        case 3:
            p->signCheck(frame);
            p->laneCheck(frame);
            break;
        case 4:
            while(p->choice==4){
                cout<<"sleep"<<endl;
                this_thread::sleep_for(std::chrono::milliseconds(500));
            }
            break;
        default:
            cout<<p->choice<<"|"<<endl;
            p->choice=-1;
            return;
        }
        //QImage image(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
        //QGraphicsScene *scene=new QGraphicsScene();

        cvtColor( frame, frame, CV_BGR2RGB );

        QImage image = QImage( (const unsigned char*)(frame.data), frame.cols, frame.rows, QImage::Format_RGB888 );
        p->ui->outputImage->setPixmap( QPixmap::fromImage(image.scaled(p->ui->outputImage->width(),p->ui->outputImage->height())));
        //p->ui->outputImage->resize(100,100);
        p->ui->outputImage->show();
        this_thread::sleep_for(chrono::milliseconds(50));
    }
    cout<<p->choice<<"|"<<endl;
    p->choice=-1;
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
