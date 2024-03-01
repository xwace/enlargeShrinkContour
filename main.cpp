#include <iostream>
#include <chrono>
#include <time.h>
#include <stdarg.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;
cv::Mat DEBUG::src;
cv::Mat DEBUG::src_c1;

std::vector<cv::Point> scale(std::vector<cv::Point> data, float sec_dis) {

    int num = (int) data.size();
    std::vector<cv::Point> scal_data;

    //注意这里遍历次数一定要是num!!!否则会遗漏最后一个值
    for (int i = 1; i <= num; ++i) {
        auto x1 = data[(i) % num].x - data[(i - 1) % num].x;
        auto y1 = data[(i) % num].y - data[(i - 1) % num].y;
        auto x2 = data[(i + 1) % num].x - data[(i) % num].x;
        auto y2 = data[(i + 1) % num].y - data[(i) % num].y;

        auto d_A = std::sqrt(x1 * x1 + y1 * y1);
        auto d_B = std::sqrt(x2 * x2 + y2 * y2);

        auto Vec_Cross = (x1 * y2) - (x2 * y1);
        auto sin_theta = Vec_Cross / (d_A * d_B);

        if (d_A * d_B == 0 or sin_theta == 0) continue;

        auto dv = sec_dis / sin_theta;

        auto v1_x = (dv / d_A) * x1;
        auto v1_y = (dv / d_A) * y1;

        auto v2_x = (dv / d_B) * x2;
        auto v2_y = (dv / d_B) * y2;

        auto PQ_x = v1_x - v2_x;;
        auto PQ_y = v1_y - v2_y;

        //注意这里做类型转换，不做会默认数据截断，导致出错
        int Q_x = cvRound(data[(i) % num].x + PQ_x);
        int Q_y = cvRound(data[(i) % num].y + PQ_y);

        scal_data.emplace_back(Q_x, Q_y);
    }
    
    return scal_data;
}

#include "fstream"
int main() {
    ifstream filein("/home/oswin/Desktop/map/map.txt");
    cv::Mat src;
    string str;
    while (getline(filein, str)) {
        src.push_back(cv::Mat(1, (int)str.length(), 0, &str[0]));
    }
    src = src == 48;
    erode(src, src, Matx<uchar, 5, 5>::ones());
    namedWindow("src", 2);
    imshow("src", src);
    waitKey();

    using VP = std::vector<cv::Point>;
    using VVP = std::vector<std::vector<cv::Point>>;
    VVP cons;
    findContours(src, cons, RETR_EXTERNAL, CHAIN_APPROX_NONE);

    Mat output(src.size(), 0, Scalar(0));
    drawContours(output, VVP{cons[6]}, -1, 255, 1);

    auto contour = cons[6];
    auto C1 = scale(contour, -1);
    auto C2 = scale(contour, +1);
    cvtColor(output, output, COLOR_GRAY2BGR);
    const int connectType = 4;

    polylines(output, VVP{C1}, true, Scalar(0, 0, 255), 1, connectType);
    polylines(output, VVP{C2}, true, Scalar(0, 255, 0), 1, connectType);
    drawContours(output, VVP{contour}, -1, Scalar(255, 0, 0), 1, connectType);
    namedWindow("output", 2);
    imshow("output", output);
    waitKey();
}
