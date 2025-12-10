#include "LightDescriptor.h"
#include <cmath>

LightDescriptor::LightDescriptor(const cv::RotatedRect &lightRect)
{
    rect = lightRect;
    center = rect.center;
    size = rect.size;
    length = std::max(size.width, size.height);
    angle = rect.angle;
}

void LightDescriptor::adjustRec(cv::RotatedRect &rect, int angleToUp)
{
    if (angleToUp == 1)
    {
        auto &size = rect.size;
        if (size.width < size.height)
        {
            std::swap(size.width, size.height);
            rect.angle += 90.0;
        }
    }

    while (rect.angle > 90.0)
        rect.angle -= 180.0;
    while (rect.angle < -90.0)
        rect.angle += 180.0;
}