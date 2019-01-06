#include "mainwindow.h"

void signCheck(Mat frame){
    RoadSign roadsign;

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
    int sizeOfArea_blue=25000;

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

void laneCheck(Mat frame){
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

void videoCheck(MainWindow *p){
    cv::VideoCapture cap(p->getVideoPath());            //读入视频
    if (!cap.isOpened()){
        QMessageBox::information(nullptr,"warning","can not open the video");
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
        cout<<p->getChoice()<<endl;
        //读取视频
        if (!cap.read(frame)||p->getChoice()<1)
            break;
        switch(p->getChoice()){
        case 1:
            laneCheck(frame);
            break;
        case 2:
            signCheck(frame);
            break;
        case 3:
            signCheck(frame);
            laneCheck(frame);
            break;
        case 4:
            while(p->getChoice()==4){
                cout<<"sleep"<<endl;
                this_thread::sleep_for(std::chrono::milliseconds(500));
            }
            break;
        default:
            cout<<p->getChoice()<<"|"<<endl;
            p->setChoice(-1);
            return;
        }
        //QImage image(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
        //QGraphicsScene *scene=new QGraphicsScene();

        cvtColor( frame, frame, CV_BGR2RGB );

        QImage image = QImage( (const unsigned char*)(frame.data), frame.cols, frame.rows, QImage::Format_RGB888 );
        p->getUI()->outputImage->setPixmap( QPixmap::fromImage(image.scaled(p->getUI()->outputImage->width(),p->getUI()->outputImage->height())));
        //p->getUI()->outputImage->resize(100,100);
        p->getUI()->outputImage->show();
    }
    cout<<p->getChoice()<<"|"<<endl;
    p->setChoice(-1);
}
