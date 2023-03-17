#ifndef SHOOTINGSCORE_H
#define SHOOTINGSCORE_H

#include "util.hpp"
#include <iomanip>
#include <stdexcept>

class ShootingScore {
  private:
    void prepareImage(cv::Mat &img,
                      cv::Mat &img_greyscale,
                      cv::Mat &img_blur,
                      cv::Mat &img_binary);

    void drawTargetContours(cv::Mat &model_img_binary, cv::Mat &result_plot);

    void computeTargetCentreRadi(cv::Mat &model_img_greyscale,
                                 cv::Point &target_centre,
                                 float &radius);

    void getShotContours(cv::Mat &src_img_blur,
                         cv::Mat &last_img_blur,
                         std::vector<std::vector<cv::Point>> &shot_contours);

    void getShotLocation(std::vector<std::vector<cv::Point>> &shot_contours,
                         cv::Point &shot_location);

    float computeScore(cv::Point target_centre, cv::Point shot_location, float radius);

  public:
    ShootingScore();
    ~ShootingScore();
    float score;
    void getResultPlot(std::string model_img_path,
                       std::string src_img_path,
                       std::string last_img_path);
};

#endif