#ifndef IMAGEALIGNMENT_H
#define IMAGEALIGNMENT_H

#include "util.hpp"

class ImageAlignment
{
private:
    cv::Mat input_img;
    cv::Mat reference_img;
public:
    ImageAlignment(std::string input_img_path, std::string reference_img_path);
    ~ImageAlignment();
};


#endif
