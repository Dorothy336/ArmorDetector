#include "ArmorDescriptor.h"
#include <algorithm>
#include <cmath>

ArmorDescriptor::ArmorDescriptor(const LightDescriptor &leftLight,
                                 const LightDescriptor &rightLight,
                                 ArmorSize armorType,
                                 const cv::Mat &grayImg,
                                 float rotationScore,
                                 const Params &params)
    : type(armorType), rotationScore(rotationScore)
{

    // 计算装甲板四个顶点（按固定顺序存储）
    cv::Point2f leftPoints[4], rightPoints[4];
    leftLight.rect.points(leftPoints);
    rightLight.rect.points(rightPoints);

    // 获取左灯条两点（左上、左下）
    std::vector<cv::Point2f> leftVec(leftPoints, leftPoints + 4);
    std::sort(leftVec.begin(), leftVec.end(), [](auto &a, auto &b)
              { return a.x < b.x; });
    cv::Point2f leftTop = leftVec[0].y < leftVec[1].y ? leftVec[0] : leftVec[1];
    cv::Point2f leftBottom = leftVec[0].y > leftVec[1].y ? leftVec[0] : leftVec[1];

    // 获取右灯条两点（右上、右下）
    std::vector<cv::Point2f> rightVec(rightPoints, rightPoints + 4);
    std::sort(rightVec.begin(), rightVec.end(), [](auto &a, auto &b)
              { return a.x < b.x; });
    cv::Point2f rightTop = rightVec[2].y < rightVec[3].y ? rightVec[2] : rightVec[3];
    cv::Point2f rightBottom = rightVec[2].y > rightVec[3].y ? rightVec[2] : rightVec[3];

    // 按顺序存入 vertex：左上 → 右上 → 右下 → 左下
    vertex.resize(4);
    vertex[0] = leftTop;
    vertex[1] = rightTop;
    vertex[2] = rightBottom;
    vertex[3] = leftBottom;

    // 计算各种得分
    float armorArea = cv::contourArea(vertex);
    sizeScore = std::exp(-armorArea / 10000.0f);

    cv::Point2f imageCenter(grayImg.cols / 2.0f, grayImg.rows / 2.0f);
    float distance = cv::norm((leftLight.center + rightLight.center) / 2.0f - imageCenter);
    distScore = std::exp(-distance / 100.0f);
}