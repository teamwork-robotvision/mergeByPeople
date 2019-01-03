#include "mainwindow.h"

//合并的路标检测函数
void RoadSign::checkRoadSign(Mat inputImage,Mat outputImage,double colorLimit[],int binaryLimit[],int erodeNum[],int dilateNum[],int closeNum[],double angleLimit[],double wid_hei[],int sizeOfArea,int someArea,int someOperate){
    Mat hsv,operateMat;;
    Mat operateImage = inputImage.clone();

    //增加对比度和饱和度
//    double alpha = 1.2; //1--3
//    int beta = 5; //0--100
//    for( int y = 0; y < operateImage.rows/3; y++ ) {
//        for (int x = 0; x < operateImage.cols * 2 / 3; x++) {
//            for (int c = 0; c < 3; c++) {
//                operateImage.at<Vec3b>(y, x)[c] =
//                        saturate_cast<uchar>(alpha * (operateImage.at<Vec3b>(y, x)[c]) + beta);
//            }
//        }
//    }

    //滤波
    if(someOperate) {
        blur(operateImage, operateImage, Size(3, 3));
        GaussianBlur(operateImage, operateImage, Size(3, 3), 0, 0);
        medianBlur(operateImage, operateImage, 5);
    }

    //RGB转为HSV图像显示
    cv::cvtColor(operateImage, hsv, cv::COLOR_BGR2HSV);

    //阈值分割
    //cv::inRange(hsv, cv::Scalar(214-BLUE,61-BLUE, 62-BLUE), cv::Scalar(214+BLUE, 61+BLUE,62+BLUE), blue_mask);
    cv::inRange(hsv, cv::Scalar(colorLimit[0], colorLimit[1],colorLimit[2]), cv::Scalar(colorLimit[3],colorLimit[4],colorLimit[5]),
                operateMat);

    //二值化
    threshold(operateMat, operateMat, binaryLimit[0],binaryLimit[1], CV_THRESH_BINARY);

//    namedWindow("binary",0);
//    imshow("binary", operateMat);

    //腐蚀膨胀
    erode(operateMat, operateMat, getStructuringElement(MORPH_RECT, Size(erodeNum[0],erodeNum[1])));

//    namedWindow("erode",0);
//    imshow("erode", operateMat);

    dilate(operateMat, operateMat, getStructuringElement(MORPH_RECT, Size(dilateNum[0],dilateNum[1])));

//    namedWindow("dilate",0);
//    imshow("dilate", operateMat);

    //闭运算
    Mat kernel = getStructuringElement(MORPH_RECT, Size(closeNum[0],closeNum[1]));
    morphologyEx(operateMat, operateMat, MORPH_CLOSE, kernel);

//    namedWindow("close",0);
//    imshow("close", operateMat);

    /***************************添加感兴趣区域*****************/
    if(someArea) {
        int width, height;
        width = operateMat.cols;
        height = operateMat.rows;

        //original的参数
        cv::Point pts[4] = {
                Point(width / 5, height / 4),//左下角开始顺时针
                Point(width / 5, 0),
                Point(width * 4 / 5, 0),
                Point(width * 4 / 5, height / 4)
        };

        // 创建二进制多边形，填充多边形获取感兴趣区域
        cv::fillConvexPoly(operateMat, pts, 4, cv::Scalar(0, 0, 0));
        //imshow("blue", operateMat);
    }
    /*************************************************************/

    //查找轮廓并显示
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(operateMat, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
    vector<vector<Point>> approxContours(contours.size());
    unsigned int index = 0;
    while (index < contours.size()){
        approxPolyDP(contours[index],approxContours[index],20,true);
        if(approxContours.at(index).size()>15){
            drawContours(outputImage,approxContours,index,Scalar(0,0,255),2);
            index++;
            continue;
        }

        //轮廓筛选
        if (contours.at(index).size() >= 4 /*轮廓角点初步筛选*/
        && hierarchy.at(index).val[2] >= 0 && hierarchy.at(index).val[3] < 0 /*找具有内轮廓的最外层轮廓*/) {
            RotatedRect cnt = minAreaRect(contours.at(index));//找出外接矩阵
            if (cnt.angle > angleLimit[0]&& cnt.angle <=angleLimit[1]) {//矩阵外界角度筛选
                int middle = cnt.size.width / cnt.size.height;
                if(middle >= wid_hei[0]&&middle <= wid_hei[1]&& cnt.size.area() < sizeOfArea) {//矩阵长宽比和矩阵大小筛选
                    Point2f box[4];
                    cnt.points(box);
                    //画出外接矩阵
                    rectangle(outputImage,box[0],box[2],Scalar(0,255,255),2);
//                    vector<vector<Point>> rec;
//                    vector<Point> v;
//                    for(int i=0;i<4;i++) {
//                        v.push_back(box[i]);
//                    }
//                    rec.push_back(v);
//                    drawContours(imageSave, rec, 0, Scalar(0, 0, 255), 2);
                }
            }
        }
        index++;
    }
}

//获取感兴趣区域
Mat RoadSign::mask(cv::Mat img) {
    cv::Mat output;
    int width, height;
    width = img.cols;
    height = img.rows;

    cv::Mat mask = cv::Mat::zeros(img.size(), img.type());

    //original的参数
//    cv::Point pts[4] = {
//            Point(width / 5, height / 4),//左下角开始顺时针
//            Point(width /5 , 0),
//            Point(width * 4 / 5, 0),
//            Point(width * 4 / 5, height / 4)
//    };

    cv::Point pts[4] = {
            Point(width * 2 / 5, height / 5),//左下角开始顺时针
            Point(width * 2 /5 , 0),
            Point(width * 4 / 5, 0),
            Point(width * 4 / 5, height / 5)
    };

    // 创建二进制多边形，填充多边形获取感兴趣区域
    cv::fillConvexPoly(mask, pts, 4, cv::Scalar(255, 255, 255));

    // 将边缘图像和掩膜进行与操作以获得输出
    cv::bitwise_and(img, mask, output);

    cv::namedWindow("ROI", 0);
    cv::resizeWindow("ROI", 800, 380);
    cv::imshow("ROI", output);

    return output;
}
