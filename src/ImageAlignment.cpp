#include "ImageAlignment.hpp"

ImageAlignment::ImageAlignment(std::string input_img_path, std::string reference_img_path)
{
    if (util::imageExists(input_img_path) != 0 or util::imageExists(reference_img_path) != 0)
    {
        return;
    }

    input_img = cv::imread(input_img_path);
    reference_img = cv::imread(reference_img_path);

    cv::cvtColor(input_img, input_img_greyscale, cv::COLOR_BGR2GRAY);
    cv::cvtColor(reference_img, reference_img_greyscale, cv::COLOR_BGR2GRAY);
}

ImageAlignment::~ImageAlignment() {}

int ImageAlignment::orbFeatureExtractionAlignment()
{
    std::vector<cv::KeyPoint> input_img_keypoints, reference_img_keypoints;
    cv::Mat input_img_descriptors, reference_img_descriptors;
    cv::Ptr<cv::ORB> orb_detector = cv::ORB::create();

    orb_detector->detectAndCompute(input_img_greyscale, cv::Mat(), input_img_keypoints,
        input_img_descriptors);
    orb_detector->detectAndCompute(reference_img_greyscale, cv::Mat(), reference_img_keypoints,
        reference_img_descriptors);

    // match descriptors between images
    cv::BFMatcher matcher(cv::NORM_HAMMING);
    std::vector<std::vector<cv::DMatch>> matches, good_matches;
    std::vector<cv::Point2f> input_img_matched_keypoints, ref_img_matched_keypoints;
    // matcher.match(input_img_descriptors, reference_img_descriptors, matches);
    matcher.knnMatch(input_img_descriptors, reference_img_descriptors, matches, 2);

    // ratio test
    for (unsigned i = 0; i < matches.size(); i++)
    {
        if (matches[i][0].distance < MATCH_RATIO * matches[i][1].distance)
        {
            input_img_matched_keypoints.push_back(input_img_keypoints[matches[i][0].queryIdx].pt);
            ref_img_matched_keypoints.push_back(reference_img_keypoints[matches[i][0].trainIdx].pt);
            good_matches.push_back(matches[i]);
        }
    }

    // find homography
    cv::Mat homography;
    if (good_matches.size() >= 4)
    {
        homography =
            cv::findHomography(input_img_matched_keypoints, ref_img_matched_keypoints, cv::RANSAC);
    }

    cv::Mat warped_img;
    cv::warpPerspective(input_img, warped_img, homography, reference_img.size());
    cv::imwrite("../output/warped_img.png", warped_img);
    cv::imshow("wraped perspective", warped_img);

    // draw matches
    cv::Mat match_img, good_match_img;
    cv::drawMatches(input_img, input_img_keypoints, reference_img, reference_img_keypoints, matches,
        match_img);

    cv::drawMatches(input_img, input_img_keypoints, reference_img, reference_img_keypoints,
        good_matches, good_match_img);

    std::cout << "matches.size()=" << matches.size() << std::endl;
    std::cout << "good_matches.size()=" << good_matches.size() << std::endl;

    cv::imwrite("../output/match_img.png", match_img);
    cv::imwrite("../output/good_match_img.png", good_match_img);
    // cv::imshow("good matches", good_match_img);
    // cv::imshow("matches", match_img);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}