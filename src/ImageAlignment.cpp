#include "ImageAlignment.hpp"

ImageAlignment::ImageAlignment(std::string input_img_path, std::string reference_img_path)
{
    if (util::imageExists(input_img_path) != 0 or util::imageExists(reference_img_path) != 0)
    {
        return;
    }

    // read images from path
    input_img = cv::imread(input_img_path);
    reference_img = cv::imread(reference_img_path);

    // prepare the input image for shape detection
    util::filterImage(input_img, input_img_greyscale, input_img_blur, input_img_thresh);
    // prepare the reference image for ORB only
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

int ImageAlignment::outlineShapeAlignment()
{
    std::vector<std::vector<cv::Point>> input_img_contours, square_contours;
    cv::Mat result_plot(input_img.size(), CV_8UC3, cv::Scalar(0, 0, 0));
    std::vector<cv::Vec4i> hierarchy;

    // detect contour from the target model
    cv::findContours(input_img_thresh, input_img_contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

    // for (size_t i = 0; i < hierarchy.size(); i++)
    // {

    //     std::cout << "(" << hierarchy[i].rows;
    //     std::cout << ", " << hierarchy[i].cols << ")" << std::endl;
    // }

    std::cout << hierarchy.size() << std::endl;
    std::cout << input_img_contours.size() << std::endl;

    // TODO safety check: loop
    for (size_t idx = 0; idx < input_img_contours.size(); idx++)
    {
        std::vector<cv::Point> approx;
        cv::approxPolyDP(input_img_contours[idx], approx,
            0.01 * cv::arcLength(input_img_contours[idx], true), true);

        if (cv::contourArea(input_img_contours[idx]) > 1000)
        {
            // detect squres
            if (approx.size() == 4)
            {
                util::drawPolyDP(result_plot, approx, util::WHITE);
            }
        }
    }

    cv::Rect board_frame;
    cv::Mat tmp;
    std::vector<cv::Point> board_corners, output_corners;
    cv::cvtColor(result_plot, tmp, cv::COLOR_BGR2GRAY);
    cv::findContours(tmp, square_contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

    int x_min, x_max, y_min, y_max;

    std::cout << "square_contours.size()=" << square_contours.size() << std::endl;

    for (size_t idx = 0; idx < square_contours.size(); idx++)
    {
        if (hierarchy[idx][PARENT_CONTOUR] == 1)
        {
            // This contour has no children, draw it red for demonstration purposes
            // cv::drawContours(result_plot, square_contours, idx, util::RED, 5);

            std::cout << "square_contours[idx].size()=" << square_contours[idx].size() << std::endl;

            findContourCorners(square_contours[idx], x_min, x_max, y_min, y_max);

            std::cout << "x_min=" << x_min << " x_max=" << x_max << " y_min=" << y_min
                      << " y_max=" << y_max << std::endl;

            std::vector<cv::Point> approx;
            cv::approxPolyDP(square_contours[idx], board_corners,
                0.1 * cv::arcLength(square_contours[idx], true), true);

            util::drawPolyDP(result_plot, board_corners, util::RED, 10);

            // // top left corner
            // board_corners.push_back(cv::Point(x_min, y_min));
            // // top right corner
            // board_corners.push_back(cv::Point(x_max, y_min));
            // // bottom left corner
            // board_corners.push_back(cv::Point(x_min, y_max));
            // // bottom right corner
            // board_corners.push_back(cv::Point(x_max, y_max));

            output_corners.push_back(cv::Point(0, 0));
            output_corners.push_back(cv::Point(0, OUTPUT_IMG_SIZE));
            output_corners.push_back(cv::Point(OUTPUT_IMG_SIZE, OUTPUT_IMG_SIZE));
            output_corners.push_back(cv::Point(OUTPUT_IMG_SIZE, 0));

            //(no parent) = -1, (1 parent) = 0 ...
            std::cout << "Contour with 2 parents: #" << idx << " r="
                      << board_frame
                      //   << " h=" << hierarchy[idx] << "\n"
                      << " src corners: "
                      << "\n"
                      << board_corners << "\n"
                      << " dst corners: "
                      << "\n"
                      << output_corners << "\n";
            continue;
        }

        // This contour has children, draw it blue for demonstration purposes
        cv::drawContours(result_plot, square_contours, idx, util::WHITE, 1);
    }

    cv::Mat homography;
    homography = cv::findHomography(board_corners, output_corners, cv::RANSAC);
    cv::Mat warped_img;
    cv::warpPerspective(input_img, warped_img, homography,
        cv::Size(OUTPUT_IMG_SIZE, OUTPUT_IMG_SIZE));

    // Setup a rectangle to define your region of interest
    // int adjust = 5;
    // int width = img.size().width;
    // int start = std::floor(width / 15);
    // int end = width - start;
    // cv::Rect output_shape(0, 0, OUTPUT_IMG_SIZE, OUTPUT_IMG_SIZE);
    // std::vector<cv::Point> v(cv::Point(0, 0), cv::Point(0, 0), cv::Point(0, 0), cv::Point(0, 0));

    // std::vector<cv::Point> *v = new std::vector<cv::Point>();

    // v->push_back(cv::Point(0, 0));
    // v->push_back(cv::Point(0, 0));
    // v->push_back(cv::Point(0, 0));
    // v->push_back(cv::Point(0, 0));

    // delete &v;

    // Crop the full image to that image contained by the rectangle active_region
    // Note that this doesn't copy the data
    // img = img(cv::Range(start, end), cv::Range(start, end));

    cv::imshow("shapes", warped_img);
    cv::imwrite("../output/warped_img.png", warped_img);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}

void findContourCorners(std::vector<cv::Point> &contour, int &x_min, int &x_max, int &y_min,
    int &y_max)
{
    // initialise values
    x_max = -1;
    y_max = -1;
    x_min = 2000;
    y_min = 2000;

    for (auto &&point : contour)
    {
        if (point.x > x_max)
        {
            x_max = point.x;
        }
        else if (point.x < x_min)
        {
            x_min = point.x;
        }

        if (point.y > y_max)
        {
            y_max = point.y;
        }
        else if (point.y < y_min)
        {
            y_min = point.y;
        }
    }
}