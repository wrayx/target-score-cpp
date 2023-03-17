#include "ImageAlignment.hpp"
#include "ShootingScore.hpp"

int main(int argc, char const *argv[]) {
    // ImageAlignment *align_image = new ImageAlignment(
    //     "../input_images/group_1/shot_3.JPG",
    //     "../input_images/group_1/align2d/aligned_shot_1.JPG");

    // align_image->orbFeatureExtractionAlignment();
    // align_image->outlineShapeAlignment();

    // TODO get input images path as command argument
    ShootingScore *ss = new ShootingScore();
    ss->getResultPlot("../input_images/group_1/aligned/aligned_shot_0.JPG",
                      "../input_images/group_1/aligned/aligned_shot_2.JPG",
                      "../input_images/group_1/aligned/aligned_shot_1.JPG");
    std::cout << "score = " << ss->score << std::endl;
    delete ss;
    return 0;
}