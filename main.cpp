#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

int imgExist(char *img_path);
void displayImage(cv::Mat &img, const std::string window_name);

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
    cv::Mat median_blur_img;
    cv::Mat threshold_img;

    // convert source image to grayscale
    cv::cvtColor(src_img, greyscale_img, cv::COLOR_BGR2GRAY);
    cv::medianBlur(greyscale_img, median_blur_img, 5);
    cv::blur(greyscale_img, threshold_img, cv::Size(3, 3));
    cv::threshold(threshold_img, threshold_img, 210, 255, cv::THRESH_BINARY);

    // find contours from the image
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(threshold_img, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

    // initialise a matrix for storing result
    cv::Mat contour_img(threshold_img.size(), CV_8UC3, cv::Scalar(0, 0, 0));
    for (size_t idx = 0; idx < contours.size(); idx++)
    {
        cv::drawContours(contour_img, contours, idx, cv::Scalar(255, 255, 255));
    }

    // display image in windows
    cv::imshow("Testing Image", src_img);
    cv::imshow("Contoured Image", contour_img);

    // detecting circle
    std::vector<cv::Vec3f> circles;

    cv::HoughCircles(median_blur_img, circles, cv::HOUGH_GRADIENT, 1,
                     median_blur_img.rows / 1, // change this value to detect circles with different distances to each other
                     200, 100, 100, 800             // change the last two parameters
                                                // (min_radius & max_radius) to detect larger circles
    );

    for (size_t i = 0; i < circles.size(); i++)
    {
        cv::Vec3i c = circles[i];
        cv::Point center = cv::Point(c[0], c[1]);
        // circle center
        circle(src_img, center, 1, cv::Scalar(204, 0, 0), 3, cv::LINE_AA);
        // circle outline
        int radius = c[2];
        circle(src_img, center, radius, cv::Scalar(204, 0, 0), 3, cv::LINE_AA);
    }

    cv::imshow("detected circles", src_img);

    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}

int imgExist(char *img_path)
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