视觉组考核——装甲板识别入门
============
本考核任务要求参与者设计并实现一套完整的 RoboMaster 装甲板识别器。该系统需具备从图像或视频流中(默认为提供的视频)准确识别、定位敌方机器人装甲板的能力，为后续的自动瞄准系统提供可靠的视觉感知基础。

1.特性
----------
    高性能：120-150 FPS实时处理
    高精度：多级几何约束确保高检测准确率
    易配置：所有参数集中管理，一键调整
    可视化：实时显示检测结果与性能指标
    交互式：支持运行时切换敌方颜色
2.系统要求
-------
    操作系统：Ubuntu 22.04 LTS（必须）
    OpenCV：4.x版本
    CMake：3.10或更高版本
    C++编译器：支持C++14标准（GCC 7.0+）
3.主要功能
-----------
|模块名称|主要功能|
|--------------|--------------|
|     颜色预处理|   根据敌方颜色分离通道并增强目标           |
| 灯条检测             |    从二值图像中提取候选灯条          |
| 装甲板匹配             |    将成对灯条组合为装甲板          |
|  目标评分            |     为每个装甲板计算综合得分         |
|   目标选择           |   从多个装甲板中选择最优目标           |
|可视化|实时显示检测结果和性能指标（输出装甲板四角点信息，将颜色属性可视化）|

4.项目编译运行流程
-------------
### 4.1克隆仓库
```Bash
git clone https://github.com/yourusername/ArmorDetector.git
cd ArmorDetector
```
### 4.2检查项目结构
```Bash
ls -la
````
应该可以看见以下文件(不包括演示视频，运行时自行添加)
```text
CMakeLists.txt
main.cpp
ArmorDetector.h
ArmorDetector.cpp
ArmorDescriptor.h
ArmorDescriptor.cpp
LightDescriptor.h
LightDescriptor.cpp
Params.h
README.md
```
### 4.3创建构建目录
创建CMakeLists.txt文件
```Bash
cmake_minimum_required(VERSION 3.10)
project(ArmorDetector)

# 设置 C++ 标准为 C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# 查找 OpenCV 库
find_package(OpenCV REQUIRED)

# 包含头文件目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# 添加可执行文件
add_executable(main
    main.cpp
    ArmorDetector.cpp
    ArmorDetector.h
    ArmorDescriptor.cpp
    ArmorDescriptor.h
    LightDescriptor.cpp
    LightDescriptor.h
    Params.h
)

# 链接 OpenCV 库
target_link_libraries(main ${OpenCV_LIBS})

# 设置输出目录
set_target_properties(main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

```
创建build文件夹
```Bash
# 进入项目目录
cd ArmorDetector

# 创建build目录（首次编译）
mkdir build
cd build
```
### 4.4配置CMake
```Bash
# 基本配置
cmake ..

# 如果需要指定OpenCV路径（如果安装了多个版本）
cmake -D OpenCV_DIR=/path/to/opencv/build ..
```
### 4.5编译项目
```Bash
make
```
### 4.6运行项目<br>
```Bash
#之前的编译已经生成目标文件(我这里是main，可根据需要改动)
./main
```
5.运行界面说明
---------
### 1. 主窗口<br>
* 图像窗口：显示实时检测结果<br>
* 左上角信息：<br>

    FPS：当前处理帧率（120-150为正常）<br>

    Enemy：当前检测的敌方颜色（RED/BLUE）<br>

* 检测框：<br>

    细线框：所有检测到的装甲板<br>

    粗紫线框：最优目标装甲板<br>

    绿色圆点：装甲板中心<br>

    文字标签：装甲板类型（Big/Small）和得分<br>
### 2.终端输出
```Bash
Armor Detection Started!
Press 'q' to quit, 'r' to switch to RED, 'b' to switch to BLUE
灯条数量: 4 -> 装甲板数量: 1
Detected Small armor at: [320,240] [340,240] [340,260] [320,260]
# 每帧都会输出类似信息
```
5.简要说明
-----------
Params.h中设置对应参数
```Bash
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
```
