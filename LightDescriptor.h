#ifndef LIGHT_DESCRIPTOR_H
#define LIGHT_DESCRIPTOR_H

#include <opencv2/opencv.hpp>
#include <vector> // 添加 vector 头文件

class LightDescriptor
{
public:
    cv::RotatedRect rect;
    cv::Point2f center;
    float angle;
    float length;
    cv::Size2f size;

    // 添加默认构造函数
    LightDescriptor()
        : center(0, 0), angle(0), length(0), size(0, 0) {}

    explicit LightDescriptor(const cv::RotatedRect &lightRect);

    void adjustRec(cv::RotatedRect &rect, int angleToUp);
};

#endif