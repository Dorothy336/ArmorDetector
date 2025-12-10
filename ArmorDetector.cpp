#include "ArmorDetector.h"
#include <algorithm>
#include <numeric>

// 距离计算函数
namespace cvex
{
    float distance(const cv::Point2f &p1, const cv::Point2f &p2)
    {
        return cv::norm(p1 - p2);
    }
}

ArmorDetector::ArmorDetector()
    : _enemy_color(RED), _flag(ARMOR_NO), _trackCnt(0)
{
    // 初始化目标装甲板
    _targetArmor = ArmorDescriptor(LightDescriptor(), LightDescriptor(),
                                   ArmorDescriptor::SMALL_ARMOR, cv::Mat(), 0.0f, _param);
}

void ArmorDetector::preprocess(const cv::Mat &roiImg)
{
    // 分离色彩通道
    cv::split(roiImg, _channels);

    // 预处理删除己方装甲板颜色
    if (_enemy_color == RED)
    {
        _grayImg = _channels.at(2) - _channels.at(0); // Get red-blue image
    }
    else
    {
        _grayImg = _channels.at(0) - _channels.at(2); // Get blue-red image
    }
}

void ArmorDetector::findLights(const cv::Mat &binImg)
{
    _lightInfos.clear();

    // 形态学操作
    cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
    cv::Mat dilatedImg;
    cv::dilate(binImg, dilatedImg, element);

    // 查找轮廓
    std::vector<std::vector<cv::Point>> lightContours;
    cv::findContours(dilatedImg.clone(), lightContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for (const auto &contour : lightContours)
    {
        // 面积筛选
        float lightContourArea = cv::contourArea(contour);
        if (lightContourArea < _param.light_min_area)
            continue;

        // 椭圆拟合
        cv::RotatedRect lightRec = cv::fitEllipse(contour);

        // 矫正灯条
        LightDescriptor lightDesc(lightRec);
        lightDesc.adjustRec(lightRec, 1);

        // 宽高比、凸度筛选
        if (lightRec.size.width / lightRec.size.height > _param.light_max_ratio ||
            lightContourArea / lightRec.size.area() < _param.light_contour_min_solidity)
        {
            continue;
        }

        // 对灯条范围适当扩大
        lightRec.size.width *= _param.light_color_detect_extend_ratio;
        lightRec.size.height *= _param.light_color_detect_extend_ratio;

        cv::Rect lightRect = lightRec.boundingRect();
        const cv::Rect srcBound(cv::Point(0, 0), _grayImg.size());
        lightRect &= srcBound;

        // 保存灯条信息
        _lightInfos.emplace_back(lightRec);
    }
}

void ArmorDetector::matchArmors()
{
    _armors.clear();

    if (_lightInfos.empty())
    {
        _flag = ARMOR_NO;
        return;
    }

    // 按灯条中心x坐标排序
    std::sort(_lightInfos.begin(), _lightInfos.end(),
              [](const LightDescriptor &ld1, const LightDescriptor &ld2)
              {
                  return ld1.center.x < ld2.center.x;
              });

    // 灯条配对
    for (size_t i = 0; i < _lightInfos.size(); i++)
    {
        for (size_t j = i + 1; j < _lightInfos.size(); j++)
        {
            const LightDescriptor &leftLight = _lightInfos[i];
            const LightDescriptor &rightLight = _lightInfos[j];

            // 角度差筛选
            float angleDiff = std::abs(leftLight.angle - rightLight.angle);
            float lenDiffRatio = std::abs(leftLight.length - rightLight.length) /
                                 std::max(leftLight.length, rightLight.length);

            if (angleDiff > _param.light_max_angle_diff_ ||
                lenDiffRatio > _param.light_max_height_diff_ratio_)
            {
                continue;
            }

            // 位置关系筛选
            float distance = cvex::distance(leftLight.center, rightLight.center);
            float meanLen = (leftLight.length + rightLight.length) / 2;
            float yDiff = std::abs(leftLight.center.y - rightLight.center.y);
            float yDiffRatio = yDiff / meanLen;
            float xDiff = std::abs(leftLight.center.x - rightLight.center.x);
            float xDiffRatio = xDiff / meanLen;
            float ratio = distance / meanLen;

            if (yDiffRatio > _param.light_max_y_diff_ratio_ ||
                xDiffRatio < _param.light_min_x_diff_ratio_ ||
                ratio > _param.armor_max_aspect_ratio_ ||
                ratio < _param.armor_min_aspect_ratio_)
            {
                continue;
            }

            // 确定装甲板类型
            int armorType = (ratio > _param.armor_big_armor_ratio) ? ArmorDescriptor::BIG_ARMOR : ArmorDescriptor::SMALL_ARMOR;

            // 计算旋转得分
            float ratioOff = (armorType == ArmorDescriptor::BIG_ARMOR) ? std::max(_param.armor_big_armor_ratio - ratio, 0.0f) : std::max(_param.armor_small_armor_ratio - ratio, 0.0f);
            float yOff = yDiff / meanLen;
            float rotationScore = -(ratioOff * ratioOff + yOff * yOff);

            // 创建装甲板描述符（不再进行数字识别验证）
            ArmorDescriptor armor(leftLight, rightLight,
                                  static_cast<ArmorDescriptor::ArmorSize>(armorType),
                                  _channels.at(1), rotationScore, _param);

            _armors.push_back(armor);
            break;
        }
    }
}

void ArmorDetector::selectTarget()
{
    if (_armors.empty())
    {
        _flag = ARMOR_NO;
        return;
    }

    // 简化版目标选择：直接选择得分最高的装甲板
    for (auto &armor : _armors)
    {
        armor.finalScore = armor.sizeScore + armor.distScore + armor.rotationScore;
    }

    // 选择得分最高的目标
    std::sort(_armors.begin(), _armors.end(),
              [](const ArmorDescriptor &a, const ArmorDescriptor &b)
              {
                  return a.finalScore > b.finalScore;
              });

    _targetArmor = _armors[0];
    _trackCnt++;
    _flag = ARMOR_LOCAL;
}

ArmorDetector::DetectFlag ArmorDetector::detect(const cv::Mat &input)
{
    _armors.clear();
    _lightInfos.clear();

    preprocess(input);

    cv::Mat binBrightImg;
    cv::threshold(_grayImg, binBrightImg, _param.brightness_threshold, 255, cv::THRESH_BINARY);

    // 显示二值化图像
    if (_param.show_binary_image)
    {
        cv::imshow("Binary Image", binBrightImg);
    }

    findLights(binBrightImg);

    // 输出详细检测信息
    std::cout << "灯条数量: " << _lightInfos.size();

    matchArmors();

    std::cout << " -> 装甲板数量: " << _armors.size() << std::endl;

    selectTarget();

    return _flag;
}

void ArmorDetector::drawResult(cv::Mat &image)
{
    if (_armors.empty())
        return;

    // 遍历所有装甲板绘制
    for (const auto &armor : _armors)
    {
        const auto &vertices = armor.vertex;
        // 绘制装甲板边框
        for (size_t i = 0; i < 4; i++)
        {
            cv::line(image, vertices[i], vertices[(i + 1) % 4],
                     (_enemy_color == RED) ? cv::Scalar(0, 0, 255) : cv::Scalar(255, 0, 0),
                     2); // 所有装甲板用细线
        }

        // 绘制中心点
        cv::Point2f center = (vertices[0] + vertices[1] + vertices[2] + vertices[3]) / 4.0f;
        cv::circle(image, center, 5, cv::Scalar(0, 255, 0), -1);
    }

    // 高亮绘制最优目标
    const auto &bestVertices = _targetArmor.vertex;
    for (size_t i = 0; i < 4; i++)
    {
        cv::line(image, bestVertices[i], bestVertices[(i + 1) % 4],
                 cv::Scalar(255, 0, 255), 4); // 最优装甲板用粗线高亮
    }

    // 最优目标中心点
    cv::Point2f bestCenter = (bestVertices[0] + bestVertices[1] + bestVertices[2] + bestVertices[3]) / 4.0f;
    cv::circle(image, bestCenter, 3, cv::Scalar(255, 0, 255), -1);

    // 最优装甲板类型
    std::string armorType = (_targetArmor.type == ArmorDescriptor::BIG_ARMOR) ? "Big" : "Small";
    std::string text = armorType + " Armor";
    cv::putText(image, text, cv::Point(bestVertices[0].x, bestVertices[0].y - 10),
                cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 255), 1);

    // 最优装甲板得分
    std::string scoreText = "Score: " + std::to_string(_targetArmor.finalScore).substr(0, 5);
    cv::putText(image, scoreText, cv::Point(bestVertices[0].x, bestVertices[0].y - 30),
                cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(255, 255, 0), 1);
}

ArmorDetector::EnemyColor ArmorDetector::getEnemyColor() const
{
    return _enemy_color;
}