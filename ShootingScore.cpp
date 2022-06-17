#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

cv::Scalar GREEN(69, 255, 83);
cv::Scalar LIGHTGREEN(204, 255, 204);
cv::Scalar GREY(110, 110, 110);
cv::Scalar RED(67, 57, 249);
cv::Scalar DARKRED(0, 0, 255);
cv::Scalar DARKGREEN(28, 168, 23);
cv::Scalar WHITE(255, 255, 255);
cv::Scalar BLACK(0, 0, 0);
cv::Scalar BACKGROUNDCOLOR(32, 32, 32);

void drawPolyDP(cv::Mat &img, std::vector<cv::Point> &approximation, cv::Scalar &color);
int imageExists(std::string img_path);

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
    cv::Point shotLocation;
    cv::Mat result_plot;
    double score;

    ShootingScore(std::string model_img_path, std::string src_img_path, std::string last_img_path);
    ~ShootingScore();

    void getShotContours();
    int computeTargetCentre();
    void detectTargetBoard();
    void drawShootingResult();
};

ShootingScore::ShootingScore(std::string model_img_path, std::string src_img_path, std::string last_img_path)
{
    // check path
    if (imageExists(model_img_path) != 0 or imageExists(src_img_path) != 0 or imageExists(last_img_path) != 0)
    {
        return;
    }

    // three images are required
    model_img = cv::imread(model_img_path);
    src_img = cv::imread(src_img_path);
    last_img = cv::imread(last_img_path);

    // prepare images
    prepareImage(model_img, model_img_greyscale, model_img_blur, model_img_thresh);
    prepareImage(src_img, src_img_greyscale, src_img_blur, src_img_thresh);
    prepareImage(last_img, last_img_greyscale, last_img_blur, last_img_thresh);

    cv::Mat tmp(src_img.size(), CV_8UC3, cv::Scalar(0, 0, 0));
    result_plot = tmp;
}

ShootingScore::~ShootingScore() {}

void ShootingScore::prepareImage(cv::Mat &img, cv::Mat &img_greyscale, cv::Mat &img_blur, cv::Mat &img_thresh)
{

    // transform image into a squre ratio
    cv::resize(img, img, cv::Size(img.rows, img.rows));

    // Setup a rectangle to define your region of interest
    // int adjust = 5;
    int width = img.size().width;
    int start = std::floor(width / 15);
    int end = width - start;
    // cv::Rect active_region(start, start, end, end);

    // Crop the full image to that image contained by the rectangle active_region
    // Note that this doesn't copy the data
    img = img(cv::Range(start, end), cv::Range(start, end));

    cv::cvtColor(img, img_greyscale, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(img_greyscale, img_blur, cv::Size(15, 15), 0);
    cv::adaptiveThreshold(img_blur, img_thresh, 255,
                          cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 11, 3);
}

void ShootingScore::getShotContours()
{
    cv::Mat img_diff;
    cv::Mat diff_contoured(src_img.size(), CV_8UC3, cv::Scalar(0, 0, 0));

    // calculate differences
    cv::absdiff(src_img_blur, last_img_blur, img_diff);

    // obtain the threshold differences
    cv::threshold(img_diff, img_diff, 150, 255, cv::THRESH_BINARY);

    // obtain contours of the diff (shot)
    cv::findContours(img_diff, shot_contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
}

void ShootingScore::detectTargetBoard()
{
    // detect contour from the target model
    cv::findContours(model_img_thresh, model_img_contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

    // TODO safety check: loop
    for (size_t idx = 0; idx < model_img_contours.size(); idx++)
    {
        std::vector<cv::Point> approx;
        cv::approxPolyDP(model_img_contours[idx], approx, 0.01 * cv::arcLength(model_img_contours[idx], true), true);
        if (cv::contourArea(model_img_contours[idx]) > 500)
        {
            // detect squres
            if (approx.size() == 4)
            {
                drawPolyDP(result_plot, approx, LIGHTGREEN);
                // TODO transform the image by the squre shape
            }
            // detect circles
            else if (approx.size() >= 10)
            {
                cv::drawContours(result_plot, model_img_contours, idx, GREY, 3);
            }
        }
    }
}

int ShootingScore::computeTargetCentre()
{
    // detecting circle
    std::vector<cv::Vec3f> circles;

    // blur the model again with diff params for detecting circles
    cv::Mat img_blur_tmp;
    cv::blur(model_img_greyscale, img_blur_tmp, cv::Size(3, 3));

    cv::HoughCircles(img_blur_tmp, circles, cv::HOUGH_GRADIENT, 1,
                     img_blur_tmp.rows / 1, // change this value to detect circles with different distances to each other
                     200, 200, 100, 400     // change the last two parameters
                                            // (min_radius & max_radius) to detect larger circles
    );
    // TODO finish the safety check of the function
    if (!circles.size())
    {
        return 1;
    }

    for (size_t i = 0; i < circles.size(); i++)
    {
        cv::Vec3i c = circles[i];
        target_centre = cv::Point(c[0], c[1]);
        // std::cout << target_centre << std::endl;
        // draw circle center
        // circle(img, center, 1, LIGHTGREEN, 3, cv::LINE_AA);
        // draw circle outline
        // int radius = c[2];
        // circle(img, center, radius, cv::Scalar(204, 0, 0), 3, cv::LINE_AA);
    }
    return 0;
}

void ShootingScore::drawShootingResult()
{
    // TODO safety check: loop

    cv::circle(result_plot, target_centre, 1, LIGHTGREEN, 3, cv::LINE_AA);

    // draw shot contour
    for (size_t idx = 0; idx < shot_contours.size(); idx++)
    {
        cv::drawContours(result_plot, shot_contours, idx, RED, 3);
    }

    cv::imshow("shot", result_plot);
    cv::waitKey(0);
    cv::destroyAllWindows();
}

void drawPolyDP(cv::Mat &img, std::vector<cv::Point> &approximation, cv::Scalar &color = RED)
{
    // Iterate over each segment and draw it
    auto itp = approximation.begin();
    while (itp != (approximation.end() - 1))
    {
        cv::line(img, *itp, *(itp + 1), color, 3);
        ++itp;
    }
    // last point linked to first point
    cv::line(img, *(approximation.begin()), *(approximation.end() - 1), color, 3);
}

int imageExists(std::string img_path)
{

    // check image
    if (cv::imread(img_path).empty())
    {
        std::cout << "Not a valid image file" << std::endl;
        return -1;
    }
    return 0;
}

int main(int argc, char const *argv[])
{
    /* code */
    ShootingScore *ss = new ShootingScore("../test_img_1/aligned_shot_0.JPG", "../test_img_1/aligned_shot_1.JPG", "../test_img_1/aligned_shot_0.JPG");
    ss->computeTargetCentre();
    ss->detectTargetBoard();
    ss->getShotContours();
    ss->drawShootingResult();
    return 0;
}
