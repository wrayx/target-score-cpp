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

}
