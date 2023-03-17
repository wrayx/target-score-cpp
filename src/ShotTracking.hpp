#ifndef SHOTTRACKING_H
#define SHOTTRACKING_H

#include "util.hpp"
#include <iomanip>
#include <stdexcept>

class ShotTracking {
  private:
    cv::Mat template_img_greyscale, template_img_blur, template_img_binary;
    void prepareImage(cv::Mat &img,
                      cv::Mat &img_greyscale,
                      cv::Mat &img_blur,
                      cv::Mat &img_binary);

    void drawTargetContours(cv::Mat &template_img_binary, cv::Mat &result_plot);

    void getTargetCentreRadi(cv::Mat &template_img_greyscale,
                             cv::Point &target_centre,
                             float &radius);

    void getShotContours(cv::Mat &input_img_blur,
                         cv::Mat &previous_img_blur,
                         std::vector<std::vector<cv::Point>> &shot_contours);

    void getShotLocation(std::vector<std::vector<cv::Point>> &shot_contours,
                         cv::Point &shot_location);

    float computeScore(cv::Point target_centre,
                       cv::Point shot_location,
                       float radius);

  public:
    ShotTracking(std::string template_img_path);
    ~ShotTracking();
    float score;
    cv::Mat template_img;
    void getResultPlot(std::string input_img_path,
                       std::string previous_img_path,
                       std::string output_path = "../output/result.png");
};

#endif