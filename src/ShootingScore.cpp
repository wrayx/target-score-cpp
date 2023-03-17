#include "ShootingScore.hpp"

ShootingScore::ShootingScore(std::string model_img_path,
                             std::string src_img_path,
                             std::string last_img_path) {
    // check path
    if (!util::imageExists(model_img_path))
        throw std::invalid_argument("invalid model image path");
    if (!util::imageExists(src_img_path))
        throw std::invalid_argument("invalid input image path");
    if (!util::imageExists(last_img_path))
        throw std::invalid_argument("invalid (last) input image path");

    // three input images are required
    this->model_img = cv::imread(model_img_path);
    this->src_img = cv::imread(src_img_path);
    this->last_img = cv::imread(last_img_path);

    // prepare images in the format of greyscale, blurred, binary
    prepareImage(this->model_img, model_img_greyscale, model_img_blur, model_img_thresh);
    prepareImage(this->src_img, src_img_greyscale, src_img_blur, src_img_thresh);
    prepareImage(this->last_img, last_img_greyscale, last_img_blur, last_img_thresh);

    this->result_plot = cv::Mat(src_img.size(), CV_8UC3, cv::Scalar(0, 0, 0));
    this->target_centre = cv::Point(0, 0);
}

ShootingScore::~ShootingScore() {}

void ShootingScore::prepareImage(cv::Mat &img,
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

void ShootingScore::getShotContours() {

    cv::Mat img_diff;

    // calculate differences
    cv::absdiff(src_img_blur, last_img_blur, img_diff);

    // convert the diff image into binary format
    cv::threshold(img_diff, img_diff, 150, 255, cv::THRESH_BINARY);

    // obtain contours of the shot
    cv::findContours(img_diff, shot_contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
}

void ShootingScore::drawTargetContours() {
    std::vector<std::vector<cv::Point>> model_img_contours;
    // detect contour from the target model
    cv::findContours(model_img_thresh, model_img_contours, cv::RETR_LIST,
                     cv::CHAIN_APPROX_SIMPLE);

    // TODO safety check: loop
    for (size_t idx = 0; idx < model_img_contours.size(); idx++) {
        // polygon approximations
        std::vector<cv::Point> approx;
        cv::approxPolyDP(model_img_contours[idx], approx,
                         0.01 * cv::arcLength(model_img_contours[idx], true), true);

        // detect other irregular shapes (numbers, circles)
        if (approx.size() >= 10 and cv::contourArea(model_img_contours[idx]) > 10) {
            cv::drawContours(result_plot, model_img_contours, idx, util::GREY, 3);
        }
    }
}

cv::Point ShootingScore::computeTargetCentre() {
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
            // target_centre = cv::Point(c[0], c[1]);
            // calculate an avearge of target centre on fly
            target_centre.x = target_centre.x + (c[0] - target_centre.x) / n;
            target_centre.y = target_centre.y + (c[1] - target_centre.y) / n;
            total_radius = c[2];
            ++n;
            // std::cout << target_centre << std::endl;
            // draw newly detected circle
            cv::circle(model_img, target_centre, total_radius, util::DARK_GREEN, 3,
                       cv::LINE_AA);
        }
        max_radii += 50;
    }
    return target_centre;
}

void ShootingScore::getResultPlot() {
    // TODO safety check: loop
    drawTargetContours();

    // stringstream for constract strings
    std::stringstream ss;

    // draw target centre
    cv::circle(result_plot, target_centre, 2, util::WHITE, 3, cv::LINE_AA);
    cv::drawMarker(model_img, target_centre, util::DARK_RED, cv::MARKER_CROSS, 20, 3);
    cv::circle(model_img, target_centre, total_radius, util::DARK_RED, 4, cv::LINE_AA);

    // add target centre location values
    ss << "CENTRE (" << target_centre.x << ", " << target_centre.y << ")";
    cv::putText(result_plot, ss.str(), cv::Point(target_centre.x + 20, target_centre.y),
                cv::FONT_HERSHEY_SIMPLEX, 1, util::WHITE, 3);
    cv::putText(model_img, ss.str(), cv::Point(target_centre.x + 20, target_centre.y),
                cv::FONT_HERSHEY_SIMPLEX, 1, util::DARK_RED, 3);

    // draw outermost circle on result_plot
    cv::circle(result_plot, target_centre, total_radius, util::WHITE, 4, cv::LINE_AA);

    if (this->score != 0) {
        // draw shot contour
        for (size_t idx = 0; idx < shot_contours.size(); idx++) {
            cv::drawContours(result_plot, shot_contours, idx, util::GREY, 3);
            cv::drawContours(src_img, shot_contours, idx, util::DARK_RED, 3);
        }
        cv::drawMarker(model_img, shot_location, util::WHITE, cv::MARKER_CROSS, 20, 3);

        // draw shot location (centre point)
        // cv::circle(result_plot, shot_location, 2, WHITE, 3, cv::LINE_AA);
        cv::drawMarker(result_plot, shot_location, util::LIGHT_GREEN, cv::MARKER_CROSS,
                       20, 3);

        // add shot location values
        ss.str(std::string());   // clear stringstream
        ss << "Location: (" << shot_location.x << ", " << shot_location.y << ")";
        cv::putText(result_plot, ss.str(),
                    cv::Point(shot_location.x + 20, shot_location.y),
                    cv::FONT_HERSHEY_SIMPLEX, 1, util::LIGHT_GREEN, 3);
        cv::putText(model_img, ss.str(), cv::Point(shot_location.x + 20, shot_location.y),
                    cv::FONT_HERSHEY_SIMPLEX, 1, util::DARK_RED, 3);
    }

    // add scores
    ss.str(std::string());   // clear stringstream
    ss << "SCORE: " << std::fixed << std::setprecision(2) << score;
    cv::putText(result_plot, ss.str(), cv::Point(20, 60), cv::FONT_HERSHEY_SIMPLEX, 1.5,
                util::LIGHT_GREEN, 3);

    cv::imshow("shot", result_plot);
    cv::waitKey(0);
    cv::destroyAllWindows();
}

/* uses shot contours to cumpute the shot location */
void ShootingScore::getShotLocation() {
    this->shot_location = cv::Point(0, 0);

    // as the shot contour can be irregular,
    // we need consider all shapes,
    // and compute the actual centre of all contours
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

int ShootingScore::computeShootingScore() {
    // euclidean distance between the centre and the shot
    shot_distance = cv::norm(target_centre - shot_location);

    // distance between each ring
    float ring_distance = total_radius / 10;

    // highest mark - number of rings away from the centre
    this->score = 11 - (shot_distance / ring_distance);

    // discard negative score
    this->score = this->score < 0 ? 0 : this->score;

    return this->score;
}