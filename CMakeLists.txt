cmake_minimum_required(VERSION 3.10)
project(monitor)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)
include_directories(src)

find_package(PkgConfig REQUIRED)
pkg_check_modules(UDEV REQUIRED libudev)

find_package(Threads)
add_executable(monitor main.cpp src/monitor.cpp src/exampleObserver.cpp)
target_link_libraries(monitor ${UDEV_LIBRARIES}  ${CMAKE_THREAD_LIBS_INIT})
