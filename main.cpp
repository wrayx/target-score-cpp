#include "ImageAlignment.hpp"
#include "ShotTracking.hpp"

int main(int argc, char const *argv[]) {
    ImageAlignment *align_image = new ImageAlignment();

    // align_image->orbFeatureExtractionAlignment(
    //     "../input_images/group_1/shot_1.JPG",
    //     "../input_images/group_1/aligned/aligned_shot_0.JPG");
    // align_image->contourShapeAlignment("../input_images/group_1/shot_1.JPG");
    cv::imshow("wraped perspective", align_image->aligned_img);
    cv::waitKey(0);
    cv::destroyAllWindows();

    // TODO get input images path as command argument
    // ShotTracking *st =
    //     new
    //     ShotTracking("../input_images/group_1/aligned/aligned_shot_0.JPG");
    // st->getResultPlot("../input_images/group_1/aligned/aligned_shot_3.JPG",
    //                   "../input_images/group_1/aligned/aligned_shot_2.JPG");
    // std::cout << "score = " << st->score << std::endl;
    // delete st;
    return 0;
}