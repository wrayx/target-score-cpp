#ifndef SHOOTINGSCORE_H
#define SHOOTINGSCORE_H

#include "util.hpp"
#include <iomanip>
#include <stdexcept>

class ShootingScore {
  private:
    /* data */
    cv::Mat src_img;
    cv::Mat src_img_greyscale;
    cv::Mat src_img_blur;
    cv::Mat src_img_thresh;

    cv::Mat last_img;
    cv::Mat last_img_greyscale;
    cv::Mat last_img_blur;
    cv::Mat last_img_thresh;

    cv::Mat model_img;
    cv::Mat model_img_greyscale;
    cv::Mat model_img_blur;
    cv::Mat model_img_thresh;

    std::vector<std::vector<cv::Point>> model_img_contours;
    std::vector<std::vector<cv::Point>> shot_contours;

    void prepareImage(cv::Mat &img, cv::Mat &img_greyscale, cv::Mat &img_blur,
                      cv::Mat &img_thresh);

  public:
    cv::Point target_centre;
    cv::Point shot_location;
    cv::Mat result_plot;
    float total_radius;
    float shot_distance;
    float score;

    ShootingScore(std::string model_img_path, std::string src_img_path,
                  std::string last_img_path);
    ~ShootingScore();

    void getShotContours();
    int computeTargetCentre();
    void detectTargetBoard();
    void drawShootingResult();
    void computeShotLocation();
    void computeShootingScore();
};

#endif