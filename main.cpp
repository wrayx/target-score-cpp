#include "ImageAlignment.hpp"
#include "ShootingScore.hpp"

int main(int argc, char const *argv[]) {
    // ImageAlignment *align_image = new ImageAlignment(
    //     "../input_images/group_1/shot_3.JPG",
    //     "../input_images/group_1/align2d/aligned_shot_1.JPG");

    // align_image->orbFeatureExtractionAlignment();
    // align_image->outlineShapeAlignment();

    // TODO get input images path as command argument
    ShootingScore *ss =
        new ShootingScore("../input_images/group_1/aligned/aligned_shot_0.JPG",
                          "../input_images/group_1/aligned/aligned_shot_1.JPG",
                          "../input_images/group_1/aligned/aligned_shot_0.JPG");
    ss->computeTargetCentre();
    // ss->drawTargetContours();
    ss->getShotContours();
    ss->getShotLocation();
    ss->computeShootingScore();
    ss->getResultPlot();

    return 0;
}