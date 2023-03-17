#ifndef IMAGEALIGNMENT_H
#define IMAGEALIGNMENT_H

#include "util.hpp"

#define NEXT_SIBLING   0
#define PREV_SIBLING   1
#define CHILD_CONTOUR  2
#define PARENT_CONTOUR 3

#define OUTPUT_IMG_SIZE 1080

class ImageAlignment {
  private:
    cv::Mat input_img;
    cv::Mat input_img_greyscale;
    cv::Mat input_img_blur;
    cv::Mat input_img_thresh;
    cv::Mat reference_img;
    cv::Mat reference_img_greyscale;
    cv::Mat reference_img_blur;
    cv::Mat reference_img_thresh;

    const float MATCH_RATIO = 0.7f;

  public:
    cv::Mat output_img_orb_aligned, output_img_shape_aligned;
    ImageAlignment(std::string input_img_path, std::string reference_img_path);
    ~ImageAlignment();
    int orbFeatureExtractionAlignment();
    int outlineShapeAlignment();
};

#endif
