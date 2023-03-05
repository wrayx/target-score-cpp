# CMakeLists.txt

# CMake version that the current computer is using
cmake_minimum_required(VERSION "3.23")

# name of the project
project(shooting_score)

# set OpenCV_DIR variable equal to the path to the cmake
# files within the previously installed opencv program
set(OpenCV_DIR /opt/homebrew/Cellar/opencv/4.5.5_2/include/opencv4)

# Tell compiler to use C++ 14 features which is needed because
# Clang version is often behind in the XCode installation
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_OSX_DEPLOYMENT_TARGET 13.0)

# set working directory
set(SOURCE_DIR src)

# configure the necessary common CMake environment variables
# needed to include and link the OpenCV program into this
# demo project, namely OpenCV_INCLUDE_DIRS and OpenCV_LIBS
find_package(OpenCV REQUIRED)

# tell the build to include the headers from OpenCV
include_directories(${OpenCV_INCLUDE_DIRS} ${SOURCE_DIR})

# add library that compute shooting score
add_library(ShootingScore ${SOURCE_DIR}/ShootingScore.cpp)
add_library(ImageAlignment ${SOURCE_DIR}/ImageAlignment.cpp)
add_library(util ${SOURCE_DIR}/util.cpp)

# specify the executable target to be built
add_executable(main main.cpp)

target_link_libraries(ShootingScore util)
target_link_libraries(ImageAlignment util)

# tell it to link the executable target against OpenCV
target_link_libraries(main ${OpenCV_LIBS} util ShootingScore ImageAlignment)