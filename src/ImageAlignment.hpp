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
    cv::Mat input_img_greyscale, input_img_blur, input_img_binary;

    const float MATCH_RATIO = 0.15f;

  public:
    cv::Mat input_img, aligned_img;
    ImageAlignment();
    ~ImageAlignment();
    void processInputImage(std::string input_img_path);
    void orbFeatureExtractionAlignment(std::string input_img_path,
                                       std::string reference_img_path);
    void contourShapeAlignment(std::string input_img_path);
};

#endif
