#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <opencv2/opencv.hpp>
// compiler will include everything selected in CMake

namespace util {
// Constants are all upper case
const cv::Scalar GREEN = cv::Scalar(69, 255, 83);
const cv::Scalar LIGHT_GREEN = cv::Scalar(204, 255, 204);
const cv::Scalar DARK_GREEN = cv::Scalar(0, 120, 0);
const cv::Scalar RED = cv::Scalar(67, 57, 249);
const cv::Scalar DARK_RED = cv::Scalar(0, 0, 180);
const cv::Scalar GREY = cv::Scalar(50, 50, 50);
const cv::Scalar WHITE = cv::Scalar(255, 255, 255);
const cv::Scalar BLACK = cv::Scalar(0, 0, 0);
const cv::Scalar BACKGROUND = cv::Scalar(32, 32, 32);

void drawPolyDP(cv::Mat &img, std::vector<cv::Point> &approximation, cv::Scalar color,
                int thickness = 3);
bool imageExists(std::string img_path);
void getContourCentre(std::vector<cv::Point> &contour, cv::Point &centre);
void filterImage(cv::Mat &img, cv::Mat &img_greyscale, cv::Mat &img_blur,
                 cv::Mat &img_thresh);
// void prepareImage(cv::Mat &img, cv::Mat &img_greyscale, cv::Mat &img_blur,
// cv::Mat &img_thresh);
};   // namespace util

#endif