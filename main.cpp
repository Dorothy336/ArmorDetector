#include "ArmorDetector.h"
#include <iostream>
#include <opencv2/opencv.hpp>

int main()
{
    // 创建装甲板检测器
    ArmorDetector detector;
    detector.setEnemyColor(ArmorDetector::RED);

    // 打开摄像头
    cv::VideoCapture cap("/home/tianmiao/cpp/TEST01/Detector1/test.mp4");
    if (!cap.isOpened())
    {
        std::cerr << "Error: Cannot open camera!" << std::endl;
        return -1;
    }

    // 设置摄像头参数（可选）
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    cap.set(cv::CAP_PROP_FPS, 30);

    cv::Mat frame;
    int frameCount = 0;
    double totalTime = 0.0;

    std::cout << "Armor Detection Started!" << std::endl;
    std::cout << "Press 'q' to quit, 'r' to switch to RED, 'b' to switch to BLUE" << std::endl;

    while (true)
    {
        auto start = std::chrono::high_resolution_clock::now();

        cap >> frame;
        if (frame.empty())
        {
            std::cerr << "Error: Cannot read frame!" << std::endl;
            break;
        }

        // 检测到目标装甲板
        auto result = detector.detect(frame);

        if (result != ArmorDetector::ARMOR_NO)
        {
            // 绘制检测结果
            detector.drawResult(frame);

            // 获取目标装甲板信息
            auto target = detector.getTargetArmor();
            std::string armorType = (target.type == ArmorDescriptor::BIG_ARMOR) ? "Big" : "Small";
            std::cout << "Detected " << armorType << " armor at: "
                      << target.vertex[0] << target.vertex[1] << target.vertex[2] << target.vertex[3] << std::endl;
        }
        else
        {
            std::cout << "No armor detected." << std::endl;
        }

        auto end = std::chrono::high_resolution_clock::now();
        double processTime = std::chrono::duration<double, std::milli>(end - start).count();
        totalTime += processTime;
        frameCount++;

        // 显示处理时间
        double fps = 1000.0 / processTime;
        std::string timeText = "FPS: " + std::to_string(fps).substr(0, 4);
        cv::putText(frame, timeText, cv::Point(10, 30),
                    cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);

        // main.cpp 显示敌人颜色（左上角）
        std::string colorText = "Enemy: " + std::string(detector.getEnemyColor() == ArmorDetector::RED ? "RED" : "BLUE");
        cv::putText(frame, colorText, cv::Point(10, 60),
                    cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);

        // 显示结果
        cv::imshow("Armor Detection - Simplified", frame);

        // 键盘控制
        char key = cv::waitKey(1);
        if (key == 'q')
        {
            break;
        }
        else if (key == 'r')
        {
            detector.setEnemyColor(ArmorDetector::RED);
            std::cout << "Switched to RED enemy" << std::endl;
        }
        else if (key == 'b')
        {
            detector.setEnemyColor(ArmorDetector::BLUE);
            std::cout << "Switched to BLUE enemy" << std::endl;
        }
    }

    // 输出平均性能
    if (frameCount > 0)
    {
        double avgTime = totalTime / frameCount;
        double avgFPS = 1000.0 / avgTime;
        std::cout << "Average processing time: " << avgTime << " ms" << std::endl;
        std::cout << "Average FPS: " << avgFPS << std::endl;
    }

    std::cout << "Armor Detection Finished!" << std::endl;
    return 0;
}