#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

int imgExist(char *src_path);
void displayImage(cv::Mat &src_img, const std::string window_name);

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

    // convert source image to grayscale
    cv::cvtColor(src_img, src_img, cv::COLOR_BGR2GRAY);
    cv::threshold(src_img, src_img, 128, 255, cv::THRESH_BINARY);
    cv::blur(src_img, src_img, cv::Size(3, 3));

    // find contours from the image
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(src_img.clone(), contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

    // initialise a matrix for storing result
    cv::Mat contour_img(src_img.size(), CV_8UC3, cv::Scalar(0, 0, 0));
    for (size_t idx = 0; idx < contours.size(); idx++)
    {
        cv::drawContours(contour_img, contours, idx, cv::Scalar(255, 255, 255));
    }

    // display image in windows
    displayImage(src_img, "Testing Image");
    displayImage(contour_img, "Contoured Image");

    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}

int imgExist(char *src_path)
{
    // check image
    if (cv::imread(src_path).empty())
    {
        std::cout << "Not a valid image file" << std::endl;
        return -1;
    }
    return 0;
}

void displayImage(cv::Mat &src_img, const std::string window_name)
{
    // create window
    cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE);
    // place the window at the top
    cv::setWindowProperty(window_name, cv::WND_PROP_TOPMOST, 1);
    cv::imshow(window_name, src_img);
}