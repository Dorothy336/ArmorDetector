#ifndef PARAMS_H
#define PARAMS_H

#include <opencv2/opencv.hpp>

struct Params
{
    // ============ 亮度阈值参数 ============
    int brightness_threshold = 70; // 降低阈值

    // ============ 灯条筛选参数 ============
    float light_min_area = 20.0f;                 // 减小最小面积
    float light_max_ratio = 4.0f;                 // 增大宽高比
    float light_contour_min_solidity = 0.5f;      // 降低紧实度要求
    float light_color_detect_extend_ratio = 1.1f; // 添加这个缺失的参数！

    // ============ 灯条匹配参数 ============
    float light_max_angle_diff_ = 20.0f;       // 增大角度容差
    float light_max_height_diff_ratio_ = 0.4f; // 增大高度差容差
    float light_max_y_diff_ratio_ = 0.6f;      // 增大Y方向容差
    float light_min_x_diff_ratio_ = 0.7f;      // 减小X方向最小距离

    // ============ 装甲板参数 ============
    float armor_max_aspect_ratio_ = 6.0f; // 增大最大宽高比
    float armor_min_aspect_ratio_ = 1.0f; // 减小最小宽高比
    float armor_big_armor_ratio = 3.2f;   // 调整大小装甲分界
    float armor_small_armor_ratio = 1.8f;

    // ============ 调试参数 ============
    bool show_binary_image = false;
    bool show_light_contours = false;
    bool debug_mode = false;
};

#endif // PARAMS_H