#ifndef ARMOR_DETECTOR_H
#define ARMOR_DETECTOR_H

#include "ArmorDescriptor.h"
#include "LightDescriptor.h"
#include "Params.h"
#include <memory>
#include <opencv2/opencv.hpp>
#include <vector>

class ArmorDetector
{
public:
    enum DetectFlag
    {
        ARMOR_NO = 0,
        ARMOR_LOCAL = 1,
        ARMOR_GLOBAL = 2
    };

    enum EnemyColor
    {
        RED = 0,
        BLUE = 1
    };

    ArmorDetector();
    ~ArmorDetector() = default;

    // 设置敌人颜色
    void setEnemyColor(EnemyColor color) { _enemy_color = color; }

    // 主检测函数
    DetectFlag detect(const cv::Mat &input);

    // 获取检测结果
    ArmorDescriptor getTargetArmor() const { return _targetArmor; }
    std::vector<ArmorDescriptor> getAllArmors() const { return _armors; }

    // 绘制结果
    void drawResult(cv::Mat &image);
    // 获取当前敌人颜色
    EnemyColor getEnemyColor() const;

private:
    // 预处理
    void preprocess(const cv::Mat &roiImg);

    // 灯条检测
    void findLights(const cv::Mat &binImg);

    // 装甲板匹配
    void matchArmors();

    // 目标选择
    void selectTarget();

    EnemyColor _enemy_color;
    Params _param;
    cv::Mat _grayImg;
    std::vector<cv::Mat> _channels;
    std::vector<LightDescriptor> _lightInfos;
    std::vector<ArmorDescriptor> _armors;
    ArmorDescriptor _targetArmor;
    DetectFlag _flag;
    int _trackCnt;
};

#endif