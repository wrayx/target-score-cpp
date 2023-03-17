#include "ShotTracking.hpp"

ShotTracking::ShotTracking() {}

ShotTracking::~ShotTracking() {}

void ShotTracking::getResultPlot(std::string model_img_path,
                                 std::string src_img_path,
                                 std::string last_img_path,
                                 std::string output_path) {
    // check path
    if (!util::imageExists(model_img_path))
        throw std::invalid_argument("invalid model image path");
    if (!util::imageExists(src_img_path))
        throw std::invalid_argument("invalid input image path");
    if (!util::imageExists(last_img_path))
        throw std::invalid_argument("invalid (last) input image path");

    // three input images are required
    cv::Mat model_img = cv::imread(model_img_path);
    cv::Mat src_img = cv::imread(src_img_path);
    cv::Mat last_img = cv::imread(last_img_path);

    cv::Mat src_img_greyscale, src_img_blur, src_img_binary;
    cv::Mat last_img_greyscale, last_img_blur, last_img_binary;
    cv::Mat model_img_greyscale, model_img_blur, model_img_binary;

    // prepare images in the format of greyscale, blurred and binary
    prepareImage(model_img, model_img_greyscale, model_img_blur,
                 model_img_binary);
    prepareImage(src_img, src_img_greyscale, src_img_blur, src_img_binary);
    prepareImage(last_img, last_img_greyscale, last_img_blur, last_img_binary);

    // initialise the result plot
    cv::Mat result_plot = cv::Mat(src_img.size(), CV_8UC3, cv::Scalar(0, 0, 0));

    // draw target contour
    drawTargetContours(model_img_binary, result_plot);
    // get target board centre and radius
    float radius;
    cv::Point target_centre = cv::Point(0, 0);
    getTargetCentreRadi(model_img_greyscale, target_centre, radius);
    // get shot contours
    std::vector<std::vector<cv::Point>> shot_contours;
    getShotContours(src_img_blur, last_img_blur, shot_contours);
    // get shot location
    cv::Point shot_location = cv::Point(0, 0);
    getShotLocation(shot_contours, shot_location);
    this->score = computeScore(target_centre, shot_location, radius);

    // stringstream for string concatnations
    std::stringstream ss;

    // draw target centre
    cv::circle(result_plot, target_centre, 2, util::WHITE, 3, cv::LINE_AA);

    // add target centre location values
    ss << "CENTRE (" << target_centre.x << ", " << target_centre.y << ")";
    cv::putText(result_plot, ss.str(),
                cv::Point(target_centre.x + 20, target_centre.y),
                cv::FONT_HERSHEY_SIMPLEX, 1, util::WHITE, 3);

    // draw outermost circle on result_plot
    cv::circle(result_plot, target_centre, radius, util::WHITE, 4, cv::LINE_AA);

    // if the there is a valid score, draw it out on result plot
    if (score != 0) {
        // draw shot contour
        for (size_t idx = 0; idx < shot_contours.size(); idx++) {
            cv::drawContours(result_plot, shot_contours, idx, util::GREY, 3);
        }

        // draw shot location
        cv::drawMarker(result_plot, shot_location, util::LIGHT_GREEN,
                       cv::MARKER_CROSS, 20, 3);

        // add shot location values
        ss.str(std::string());   // clear stringstream
        ss << "Location: (" << shot_location.x << ", " << shot_location.y
           << ")";
        cv::putText(result_plot, ss.str(),
                    cv::Point(shot_location.x + 20, shot_location.y),
                    cv::FONT_HERSHEY_SIMPLEX, 1, util::LIGHT_GREEN, 3);
    }

    // add scores
    ss.str(std::string());   // clear stringstream
    ss << "SCORE: " << std::fixed << std::setprecision(2) << score;
    cv::putText(result_plot, ss.str(), cv::Point(20, 60),
                cv::FONT_HERSHEY_SIMPLEX, 1.5, util::LIGHT_GREEN, 3);

    cv::imshow("shot", result_plot);
    cv::waitKey(0);
    cv::destroyAllWindows();
}

void ShotTracking::prepareImage(cv::Mat &img,
                                cv::Mat &img_greyscale,
                                cv::Mat &img_blur,
                                cv::Mat &img_binary) {

    // transform image into a squre ratio
    cv::resize(img, img, cv::Size(img.rows, img.rows));

    // Setup a rectangle to define the region of interest
    int width = img.size().width;
    int start = std::floor(width / 15);
    int end = width - start;

    // Crop the full image to the region of interest
    img = img(cv::Range(start, end), cv::Range(start, end));

    util::filterImage(img, img_greyscale, img_blur, img_binary);
}

void ShotTracking::getShotContours(
    cv::Mat &src_img_blur,
    cv::Mat &last_img_blur,
    std::vector<std::vector<cv::Point>> &shot_contours) {

    cv::Mat img_diff;

    // calculate differences
    cv::absdiff(src_img_blur, last_img_blur, img_diff);

    // convert the diff image into binary format
    cv::threshold(img_diff, img_diff, 150, 255, cv::THRESH_BINARY);

    // obtain contours of the shot
    cv::findContours(img_diff, shot_contours, cv::RETR_LIST,
                     cv::CHAIN_APPROX_SIMPLE);
}

void ShotTracking::drawTargetContours(cv::Mat &model_img_binary,
                                      cv::Mat &result_plot) {
    std::vector<std::vector<cv::Point>> model_img_contours;
    // detect contour from the target model
    cv::findContours(model_img_binary, model_img_contours, cv::RETR_LIST,
                     cv::CHAIN_APPROX_SIMPLE);

    // loop through contours detected from the binary image
    for (size_t idx = 0; idx < model_img_contours.size(); idx++) {
        // polygon approximations
        std::vector<cv::Point> approx;
        cv::approxPolyDP(model_img_contours[idx], approx,
                         0.01 * cv::arcLength(model_img_contours[idx], true),
                         true);

        // detect other irregular shapes (numbers, circles)
        if (approx.size() >= 10 and
            cv::contourArea(model_img_contours[idx]) > 10) {
            cv::drawContours(result_plot, model_img_contours, idx, util::GREY,
                             3);
        }
    }
}

void ShotTracking::getTargetCentreRadi(cv::Mat &model_img_greyscale,
                                       cv::Point &target_centre,
                                       float &radius) {
    // detecting circle
    std::vector<cv::Vec3f> circles;

    // blur the model again with diff params for detecting circles
    cv::Mat img_blur_tmp;
    cv::blur(model_img_greyscale, img_blur_tmp, cv::Size(3, 3));

    int max_radii = 0, n = 1;

    while (max_radii <= 800) {

        cv::HoughCircles(
            img_blur_tmp, circles, cv::HOUGH_GRADIENT, 1,
            img_blur_tmp.rows / 1,   // change this value to detect circles with
                                     // different distances to each other
            200, 200, 200,
            max_radii   // change the last parameter
                        // max_radius to detect larger or smaller circles
        );

        for (size_t i = 0; i < circles.size(); i++) {
            // [x, y, radii] 3 items in circles[i]
            cv::Vec3i c = circles[i];
            // calculate an avearge of target centre on fly
            target_centre.x = target_centre.x + (c[0] - target_centre.x) / n;
            target_centre.y = target_centre.y + (c[1] - target_centre.y) / n;
            radius = c[2];
            ++n;
        }
        max_radii += 50;
    }
}

/* uses shot contours to cumpute the shot location */
void ShotTracking::getShotLocation(
    std::vector<std::vector<cv::Point>> &shot_contours,
    cv::Point &shot_location) {
    // compute the shot centre as the centre of an irrigular shape
    for (auto &&contour : shot_contours) {
        cv::Point contour_centre;
        util::getContourCentre(contour, contour_centre);
        shot_location.x = shot_location.x + contour_centre.x;
        shot_location.y = shot_location.y + contour_centre.y;
    }

    // get a average centre of all contours
    shot_location.x = shot_location.x / shot_contours.size();
    shot_location.y = shot_location.y / shot_contours.size();
}

float ShotTracking::computeScore(cv::Point target_centre,
                                 cv::Point shot_location,
                                 float radius) {
    // euclidean distance between the centre and the shot
    double shot_distance = cv::norm(target_centre - shot_location);

    // distance between each ring
    float ring_distance = radius / 10;

    // highest mark - number of rings away from the centre
    float score = 11 - (shot_distance / ring_distance);

    // discard negative score
    score = score < 0 ? 0 : score;

    return score;
}