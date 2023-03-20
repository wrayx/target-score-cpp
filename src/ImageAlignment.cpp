#include "ImageAlignment.hpp"

ImageAlignment::ImageAlignment() {}

ImageAlignment::~ImageAlignment() {}

void ImageAlignment::processInputImage(std::string input_img_path) {
    if (!util::imageExists(input_img_path)) {
        throw std::invalid_argument("invalid input image path");
    }

    // read images from path
    input_img = cv::imread(input_img_path);

    // prepare the input image for shape detection
    util::filterImage(input_img, input_img_greyscale, input_img_blur,
                      input_img_binary);
    // cv::imwrite("../output/input_original_blurred.png", input_img_blur);
}

void ImageAlignment::orbFeatureExtractionAlignment(
    std::string input_img_path, std::string reference_img_path) {
    processInputImage(input_img_path);

    if (!util::imageExists(reference_img_path)) {
        throw std::invalid_argument("invalid input image path");
    }

    // read images from path
    cv::Mat reference_img = cv::imread(reference_img_path);
    // prepare the reference image for ORB
    cv::Mat reference_img_greyscale;
    cv::cvtColor(reference_img, reference_img_greyscale, cv::COLOR_BGR2GRAY);

    // ORB feature extraction
    std::vector<cv::KeyPoint> input_img_keypoints, reference_img_keypoints;
    cv::Mat input_img_descriptors, reference_img_descriptors;
    cv::Ptr<cv::ORB> orb_detector = cv::ORB::create();
    orb_detector->detectAndCompute(input_img_greyscale, cv::Mat(),
                                   input_img_keypoints, input_img_descriptors);
    orb_detector->detectAndCompute(reference_img_greyscale, cv::Mat(),
                                   reference_img_keypoints,
                                   reference_img_descriptors);

    // Match features
    std::vector<cv::DMatch> matches;
    cv::Ptr<cv::DescriptorMatcher> matcher =
        cv::DescriptorMatcher::create("BruteForce-Hamming");
    matcher->match(input_img_descriptors, reference_img_descriptors, matches,
                   cv::Mat());

    // Sort matches by score
    std::sort(matches.begin(), matches.end());

    // Remove not so good matches
    const int numGoodMatches = matches.size() * MATCH_RATIO;
    matches.erase(matches.begin() + numGoodMatches, matches.end());

    // Draw top matches
    cv::Mat imMatches;
    cv::drawMatches(input_img, input_img_keypoints, reference_img,
                    reference_img_keypoints, matches, imMatches);
    // cv::imwrite("../output/orb_match_group4.png", imMatches);
    // cv::imshow("matches", imMatches);
    // cv::waitKey(0);
    // cv::destroyAllWindows();

    // Extract location of good matches
    std::vector<cv::Point2f> points1, points2;

    for (size_t i = 0; i < matches.size(); i++) {
        points1.push_back(input_img_keypoints[matches[i].queryIdx].pt);
        points2.push_back(reference_img_keypoints[matches[i].trainIdx].pt);
    }
    cv::Mat homography;
    homography = cv::findHomography(points1, points2, cv::RANSAC);

    // transform the image with regard to the homography
    cv::warpPerspective(input_img, aligned_img, homography,
                        reference_img.size());
    // cv::imwrite("../output/orb_warped.png", aligned_img);
}

void ImageAlignment::contourShapeAlignment(std::string input_img_path) {
    processInputImage(input_img_path);
    std::vector<std::vector<cv::Point>> input_img_contours, square_contours;
    cv::Mat quadrilaterals_plot(input_img.size(), CV_8UC3, cv::Scalar(0, 0, 0));
    std::vector<cv::Vec4i> hierarchy;

    // detect contours from the input image
    cv::findContours(input_img_binary, input_img_contours, cv::RETR_LIST,
                     cv::CHAIN_APPROX_SIMPLE);
    // cv::imwrite("../output/binary_input.png", input_img_binary);
    // loop through contours, find quadrilaterals
    for (size_t idx = 0; idx < input_img_contours.size(); idx++) {
        std::vector<cv::Point> approx;
        cv::approxPolyDP(input_img_contours[idx], approx,
                         0.05 * cv::arcLength(input_img_contours[idx], true),
                         true);
        if (cv::contourArea(input_img_contours[idx]) > 1000) {
            // detect quadrilaterals, contour with 4 corners
            if (approx.size() == 4) {
                util::drawPolyDP(quadrilaterals_plot, approx, util::WHITE);
            }
        }
    }

    cv::Mat tmp;
    std::vector<cv::Point> board_corners, output_corners;
    cv::cvtColor(quadrilaterals_plot, tmp, cv::COLOR_BGR2GRAY);
    cv::findContours(tmp, square_contours, hierarchy, cv::RETR_TREE,
                     cv::CHAIN_APPROX_SIMPLE);

    for (size_t idx = 0; idx < square_contours.size(); idx++) {
        // contour with 2 parents
        // (no parent) = -1, (1 parent) = 0 ...
        if (hierarchy[idx][PARENT_CONTOUR] == 1) {
            // approximate the contour as a quadrilateral
            cv::approxPolyDP(square_contours[idx], board_corners,
                             0.1 * cv::arcLength(square_contours[idx], true),
                             true);

            // draw the detected edge of the target board
            util::drawPolyDP(quadrilaterals_plot, board_corners, util::RED, 10);

            // define the output image shape
            output_corners.push_back(cv::Point(0, 0));
            output_corners.push_back(cv::Point(0, OUTPUT_IMG_SIZE));
            output_corners.push_back(
                cv::Point(OUTPUT_IMG_SIZE, OUTPUT_IMG_SIZE));
            output_corners.push_back(cv::Point(OUTPUT_IMG_SIZE, 0));

            // (no parent) = -1, (1 parent) = 0 ...
            // std::cout << "Contour with 2 parents: #" << idx << "\n"
            //           << " src corners: "
            //           << "\n"
            //           << board_corners << "\n"
            //           << " dst corners: "
            //           << "\n"
            //           << output_corners << "\n";
            // continue;
        } else {
            // draw the remaining quadrilateral contours
            cv::drawContours(input_img, square_contours, idx, util::DARK_GREEN,
                             20);
        }
    }
    util::drawPolyDP(input_img, board_corners, util::RED, 20);
    // cv::imwrite("../output/hirachy_shapes.png", input_img);
    // cv::imshow("shapes", input_img);
    // cv::waitKey(0);
    // cv::destroyAllWindows();

    // findHomography from the input image
    cv::Mat homography;
    homography = cv::findHomography(board_corners, output_corners, cv::RANSAC);

    // warp perspective onto the output image shape
    cv::warpPerspective(input_img, aligned_img, homography,
                        cv::Size(OUTPUT_IMG_SIZE, OUTPUT_IMG_SIZE));
}
