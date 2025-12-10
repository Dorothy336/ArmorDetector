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