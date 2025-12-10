#ifndef ARMOR_DESCRIPTOR_H
#define ARMOR_DESCRIPTOR_H

#include "LightDescriptor.h" // 包含 LightDescriptor 定义
#include "Params.h"
#include <opencv2/opencv.hpp>
#include <vector> // 添加 vector 头文件

class ArmorDescriptor
{
public:
    enum ArmorSize
    {
        SMALL_ARMOR = 0,
        BIG_ARMOR = 1
    };
    enum ArmorColor
    {
        BLUE = 0,
        RED = 1
    };

    std::vector<cv::Point2f> vertex;
    ArmorSize type;
    ArmorColor color;
    float sizeScore;
    float distScore;
    float rotationScore;
    float finalScore;

    // 默认构造函数
    ArmorDescriptor()
        : type(SMALL_ARMOR),
          color(BLUE),
          sizeScore(0.0f),
          distScore(0.0f),
          rotationScore(0.0f),
          finalScore(0.0f)
    {
        // 初始化顶点数组
        vertex.resize(4, cv::Point2f(0, 0));
    }

    // 参数化构造函数
    ArmorDescriptor(const LightDescriptor &leftLight,
                    const LightDescriptor &rightLight,
                    ArmorSize armorType,
                    const cv::Mat &grayImg,
                    float rotationScore,
                    const Params &params);
};

#endif