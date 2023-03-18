#include "ShotTracking.hpp"

ShotTracking::ShotTracking(std::string template_img_path) {
    if (!util::imageExists(template_img_path))
        throw std::invalid_argument("invalid model image path");
    template_img = cv::imread(template_img_path);
    prepareImage(template_img, template_img_greyscale, template_img_blur,
                 template_img_binary);
}

ShotTracking::~ShotTracking() {}

void ShotTracking::getResultPlot(std::string input_img_path,
                                 std::string previous_img_path,
                                 std::string output_path) {
    // check path
    if (!util::imageExists(input_img_path))
        throw std::invalid_argument("invalid input image path");
    if (!util::imageExists(previous_img_path))
        throw std::invalid_argument("invalid (last) input image path");

    // read input images
    cv::Mat input_img = cv::imread(input_img_path);
    cv::Mat previous_img = cv::imread(previous_img_path);

    cv::Mat input_img_greyscale, input_img_blur, input_img_binary;
    cv::Mat previous_img_greyscale, previous_img_blur, previous_img_binary;

    // prepare images in the format of greyscale, blurred and binary
    prepareImage(input_img, input_img_greyscale, input_img_blur,
                 input_img_binary);
    prepareImage(previous_img, previous_img_greyscale, previous_img_blur,
                 previous_img_binary);

    // initialise the result plot
    result_plot = cv::Mat(input_img.size(), CV_8UC3, cv::Scalar(0, 0, 0));

    // draw target contour
    drawTargetContours(template_img_binary, result_plot);
    // get target board centre and radius
    float radius;
    cv::Point target_centre = cv::Point(0, 0);
    getTargetCentreRadi(template_img_greyscale, target_centre, radius);
    // get shot contours
    std::vector<std::vector<cv::Point>> shot_contours;
    getShotContours(input_img_blur, previous_img_blur, shot_contours);
    // get shot location
    cv::Point shot_location = cv::Point(0, 0);
    getShotLocation(shot_contours, shot_location);
    score = computeScore(target_centre, shot_location, radius);

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

void ShotTracking::drawTargetContours(cv::Mat template_img_binary,
                                      cv::Mat &result_plot) {
    std::vector<std::vector<cv::Point>> template_img_contours;
    // detect contour from the target model
    cv::findContours(template_img_binary, template_img_contours, cv::RETR_LIST,
                     cv::CHAIN_APPROX_SIMPLE);

    // loop through contours detected from the binary image
    for (size_t idx = 0; idx < template_img_contours.size(); idx++) {
        // polygon approximations
        std::vector<cv::Point> approx;
        cv::approxPolyDP(template_img_contours[idx], approx,
                         0.01 * cv::arcLength(template_img_contours[idx], true),
                         true);

        // detect other irregular shapes (numbers, circles)
        if (approx.size() >= 10 and
            cv::contourArea(template_img_contours[idx]) > 10) {
            cv::drawContours(result_plot, template_img_contours, idx,
                             util::GREY, 3);
        }
    }
}

void ShotTracking::getTargetCentreRadi(cv::Mat template_img_greyscale,
                                       cv::Point &target_centre,
                                       float &radius) {
    // detecting circle
    std::vector<cv::Vec3f> circles;

    // blur the model again with diff params for detecting circles
    cv::Mat img_blur_tmp;
    cv::blur(template_img_greyscale, img_blur_tmp, cv::Size(3, 3));

    int max_radii = 0, n = 1;

    while (max_radii <= 800) {

        cv::HoughCircles(
            img_blur_tmp, circles, cv::HOUGH_GRADIENT, 1, img_blur_tmp.rows / 1,
            200, 200, 200,
            max_radii   // change the max_radius to detect larger circles
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

void ShotTracking::getShotContours(
    cv::Mat input_img_blur,
    cv::Mat previous_img_blur,
    std::vector<std::vector<cv::Point>> &shot_contours) {

    cv::Mat img_diff;

    // calculate differences
    cv::absdiff(input_img_blur, previous_img_blur, img_diff);

    // convert the diff image into binary format
    cv::threshold(img_diff, img_diff, 150, 255, cv::THRESH_BINARY);

    // obtain contours of the shot
    cv::findContours(img_diff, shot_contours, cv::RETR_LIST,
                     cv::CHAIN_APPROX_SIMPLE);
}

/* uses shot contours to cumpute the shot location */
void ShotTracking::getShotLocation(
    std::vector<std::vector<cv::Point>> shot_contours,
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