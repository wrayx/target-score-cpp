#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

cv::Scalar GREEN(69, 255, 83);
cv::Scalar LIGHTGREEN(204, 255, 204);
cv::Scalar GREY(110, 110, 110);
cv::Scalar RED(67, 57, 249);
cv::Scalar DARKRED(0, 0, 255);
cv::Scalar DARKGREEN(28, 168, 23);
cv::Scalar WHITE(255, 255, 255);
cv::Scalar BLACK(0, 0, 0);
cv::Scalar BACKGROUNDCOLOR(32, 32, 32);

int imgExist(const char *img_path);
void displayImage(cv::Mat &img, const std::string window_name);
void drawPolyDP(cv::Mat &img, std::vector<cv::Point> &approximation, cv::Scalar &color);
int targetCentre(cv::Mat &img, cv::Point &center);

int main(int argc, char **argv)
{

    // check command line arguments
    if (argc != 2)
    {
        std::cout << "Expecting a image file to be passed to program" << std::endl;
        return -1;
    }

    // check path
    if (imgExist(argv[1]) != 0)
    {
        return -1;
    }

    // read the image
    cv::Mat src_img = cv::imread(argv[1]);
    cv::Mat greyscale_img;
    cv::Mat threshold_img;

    // convert source image to grayscale
    cv::cvtColor(src_img, greyscale_img, cv::COLOR_BGR2GRAY);
    cv::resize(greyscale_img, threshold_img, cv::Size(greyscale_img.rows, greyscale_img.rows));
    cv::medianBlur(threshold_img, threshold_img, 5);
    cv::adaptiveThreshold(threshold_img, threshold_img, 200, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 11, 3);

    // find contours from the image
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(threshold_img, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

    // initialise a matrix for storing result
    cv::Mat contour_img(threshold_img.size(), CV_8UC3, cv::Scalar(0, 0, 0));
    for (size_t idx = 0; idx < contours.size(); idx++)
    {
        std::vector<cv::Point> approx;
        cv::approxPolyDP(contours[idx], approx, 0.01*cv::arcLength(contours[idx], true), true);
        if (cv::contourArea(contours[idx]) > 500)
        {
            // detect squres
            if (approx.size() == 4)
            {
                drawPolyDP(contour_img, approx, LIGHTGREEN);
                // TODO transform the image by the squre shape
            }
            // detect circles
            else if (approx.size() >= 10)
            {
                cv::drawContours(contour_img, contours, idx, RED, 3);

                // drawPolyDP(contour_img, approx, RED);
            }
        }
    }

    // detect the centre of the target board
    cv::Point target_centre;
    targetCentre(src_img, target_centre);
    // draw circle center
    circle(contour_img, target_centre, 1, LIGHTGREEN, 3, cv::LINE_AA);

    // display image in windows
    cv::imshow("Testing Image", src_img);
    cv::imshow("Contoured Image", contour_img);
    // cv::imshow("detected circles", src_img);

    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}

int targetCentre(cv::Mat &img, cv::Point &center)
{
    cv::Mat blur_img, greyscale_img;
    cv::cvtColor(img, greyscale_img, cv::COLOR_BGR2GRAY);
    cv::blur(greyscale_img, blur_img, cv::Size(3, 3));
    // detecting circle
    std::vector<cv::Vec3f> circles;

    cv::HoughCircles(blur_img, circles, cv::HOUGH_GRADIENT, 1,
                    blur_img.rows / 1, // change this value to detect circles with different distances to each other
                    200, 200, 100, 400 // change the last two parameters
                                    // (min_radius & max_radius) to detect larger circles
    );
    // TODO finish the safety of the function 
    if (!circles.size())
        return 1;
    
    for (size_t i = 0; i < circles.size(); i++)
    {
        cv::Vec3i c = circles[i];
        center = cv::Point(c[0], c[1]);
        return 0;
        // draw circle center
        // circle(img, center, 1, LIGHTGREEN, 3, cv::LINE_AA);
        // draw circle outline
        // int radius = c[2];
        // circle(img, center, radius, cv::Scalar(204, 0, 0), 3, cv::LINE_AA);
    }
}

void drawPolyDP(cv::Mat &img, std::vector<cv::Point> &approximation, cv::Scalar &color=RED)
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

int imgExist(const char *img_path)
{
    // check image
    if (cv::imread(img_path).empty())
    {
        std::cout << "Not a valid image file" << std::endl;
        return -1;
    }
    return 0;
}

void displayImage(cv::Mat &img, const std::string window_name)
{
    // create window
    cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE);
    // place the window at the top
    cv::setWindowProperty(window_name, cv::WND_PROP_TOPMOST, 1);
    cv::imshow(window_name, img);
}