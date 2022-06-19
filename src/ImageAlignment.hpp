#ifndef IMAGEALIGNMENT_H
#define IMAGEALIGNMENT_H

#include "util.hpp"

class ImageAlignment
{
private:
    cv::Mat input_img;
    cv::Mat reference_img;
    cv::Mat input_img_greyscale;
    cv::Mat reference_img_greyscale;

    const float MATCH_RATIO = 0.85f;

public:
    ImageAlignment(std::string input_img_path, std::string reference_img_path);
    ~ImageAlignment();
    int orbFeatureExtractionAlignment();
};

#endif
