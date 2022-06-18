#include "ShootingScore.hpp"

int main(int argc, char const *argv[])
{
    /* code */

    // TODO get input images path as command argument
    ShootingScore *ss = new ShootingScore("../test_img_1/aligned_shot_0.JPG", "../test_img_1/aligned_shot_1.JPG", "../test_img_1/aligned_shot_0.JPG");
    ss->computeTargetCentre();
    ss->detectTargetBoard();
    ss->getShotContours();
    ss->computeShotLocation();
    ss->computeShootingScore();
    ss->drawShootingResult();
    return 0;
}