#include "util.hpp"

namespace util
{

    void getContourCentre(std::vector<cv::Point> &contour, cv::Point &centre)
    {
        cv::Moments mo = cv::moments(contour);
        centre = cv::Point(mo.m10 / mo.m00, mo.m01 / mo.m00);
    }

    void drawPolyDP(cv::Mat &img, std::vector<cv::Point> &approximation, cv::Scalar color = RED)
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

    void prepareImage(cv::Mat &img, cv::Mat &img_greyscale, cv::Mat &img_blur, cv::Mat &img_thresh)
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
        cv::adaptiveThreshold(img_blur, img_thresh, 255, cv::ADAPTIVE_THRESH_MEAN_C,
            cv::THRESH_BINARY_INV, 11, 3);
    }

}
