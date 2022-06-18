#ifndef SHOOTINGSCORE_H
#define SHOOTINGSCORE_H

#include <iomanip>
#include "util.hpp"

// void drawPolyDP(cv::Mat &img, std::vector<cv::Point> &approximation, cv::Scalar &color);
// int imageExists(std::string img_path);
// void getContourCentre(std::vector<cv::Point> &contour, cv::Point &centre);

class ShootingScore
{
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

    void prepareImage(cv::Mat &img, cv::Mat &img_greyscale, cv::Mat &img_blur, cv::Mat &img_thresh);

public:
    cv::Point target_centre;
    cv::Point shot_location;
    cv::Mat result_plot;
    double total_radius;
    double shot_distance;
    double score;

    ShootingScore(std::string model_img_path, std::string src_img_path, std::string last_img_path);
    ~ShootingScore();

    void getShotContours();
    int computeTargetCentre();
    void detectTargetBoard();
    void drawShootingResult();
    void computeShotLocation();
    // TODO uses target centre and shot contours to compute the score...
    // or consider using the radius
    void computeShootingScore();
};

#endif