#include "ShootingScore.hpp"

ShootingScore::ShootingScore(
    std::string model_img_path, std::string src_img_path, std::string last_img_path)
{
    // check path
    if (util::imageExists(model_img_path) != 0 or util::imageExists(src_img_path) != 0 or
        util::imageExists(last_img_path) != 0)
    {
        return;
    }

    // three images are required
    model_img = cv::imread(model_img_path);
    src_img = cv::imread(src_img_path);
    last_img = cv::imread(last_img_path);

    // prepare images
    prepareImage(model_img, model_img_greyscale, model_img_blur, model_img_thresh);
    prepareImage(src_img, src_img_greyscale, src_img_blur, src_img_thresh);
    prepareImage(last_img, last_img_greyscale, last_img_blur, last_img_thresh);

    cv::Mat tmp(src_img.size(), CV_8UC3, cv::Scalar(0, 0, 0));
    result_plot = tmp;
}

ShootingScore::~ShootingScore() {}

void ShootingScore::prepareImage(
    cv::Mat &img, cv::Mat &img_greyscale, cv::Mat &img_blur, cv::Mat &img_thresh)
{

    // transform image into a squre ratio
    cv::resize(img, img, cv::Size(img.rows, img.rows));

    // Setup a rectangle to define your region of interest
    // int adjust = 5;
    int width = img.size().width;
    int start = std::floor(width / 15);
    int end = width - start;
    // cv::Rect active_region(start, start, end, end);

    // Crop the full image to that image contained by the rectangle active_region
    // Note that this doesn't copy the data
    img = img(cv::Range(start, end), cv::Range(start, end));

    cv::cvtColor(img, img_greyscale, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(img_greyscale, img_blur, cv::Size(15, 15), 0);
    cv::adaptiveThreshold(
        img_blur, img_thresh, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 11, 3);
}

void ShootingScore::getShotContours()
{
    cv::Mat img_diff;
    cv::Mat diff_contoured(src_img.size(), CV_8UC3, cv::Scalar(0, 0, 0));

    // calculate differences
    cv::absdiff(src_img_blur, last_img_blur, img_diff);

    // obtain the threshold differences
    cv::threshold(img_diff, img_diff, 150, 255, cv::THRESH_BINARY);

    // obtain contours of the diff (shot)
    cv::findContours(img_diff, shot_contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
}

void ShootingScore::detectTargetBoard()
{
    // detect contour from the target model
    cv::findContours(model_img_thresh, model_img_contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

    // TODO safety check: loop
    for (size_t idx = 0; idx < model_img_contours.size(); idx++)
    {
        std::vector<cv::Point> approx;
        cv::approxPolyDP(model_img_contours[idx], approx,
            0.01 * cv::arcLength(model_img_contours[idx], true), true);

        // if (cv::contourArea(model_img_contours[idx]) > 500)
        // {
        //     // detect squres
        //     if (approx.size() == 4)
        //     {
        //         drawPolyDP(result_plot, approx, LIGHTGREEN);
        //         // TODO transform the image by the squre shape
        //     }
        //     // detect circles
        //     else if (approx.size() >= 10)
        //     {
        //         cv::drawContours(result_plot, model_img_contours, idx, GREY, 3);
        //     }
        // }

        // detect squres
        // if (approx.size() == 4)
        // {
        //     drawPolyDP(result_plot, approx, LIGHTGREEN);
        //     // TODO transform the image by the squre shape
        // }
        // detect other shapes (numbers, circles)
        if (approx.size() >= 10 and cv::contourArea(model_img_contours[idx]) > 10)
        {
            cv::drawContours(result_plot, model_img_contours, idx, util::GREY, 3);
        }
    }
}

int ShootingScore::computeTargetCentre()
{
    // detecting circle
    std::vector<cv::Vec3f> circles;

    // blur the model again with diff params for detecting circles
    cv::Mat img_blur_tmp;
    cv::blur(model_img_greyscale, img_blur_tmp, cv::Size(3, 3));

    cv::HoughCircles(img_blur_tmp, circles, cv::HOUGH_GRADIENT, 1,
        img_blur_tmp.rows /
            1, // change this value to detect circles with different distances to each other
        200, 200, 400, 800 // change the last two parameters
                           // (min_radius & max_radius) to detect larger circles
    );
    // TODO finish the safety check of the function
    if (!circles.size())
    {
        return 1;
    }

    for (size_t i = 0; i < circles.size(); i++)
    {
        cv::Vec3i c = circles[i];
        target_centre = cv::Point(c[0], c[1]);
        total_radius = c[2];
        // std::cout << target_centre << std::endl;
        // draw circle center
        // circle(img, center, 1, LIGHTGREEN, 3, cv::LINE_AA);
    }
    return 0;
}

void ShootingScore::drawShootingResult()
{
    // TODO safety check: loop

    // constract string
    std::string s0 = "CENTRE ";
    std::string s1 = "(";
    std::string s2 = ", ";
    std::string s3 = ")";
    std::stringstream ss;

    // draw target centre
    cv::circle(result_plot, target_centre, 2, util::LIGHT_GREEN, 3, cv::LINE_AA);

    // add target centre location values
    ss << s0 << s1 << target_centre.x << s2 << target_centre.y << s3;
    cv::putText(result_plot, ss.str(), cv::Point(target_centre.x + 20, target_centre.y),
        cv::FONT_HERSHEY_SIMPLEX, 1, util::LIGHT_GREEN, 3);
    ss.str(std::string());

    // draw circle outline
    circle(result_plot, target_centre, total_radius, util::LIGHT_GREEN, 4, cv::LINE_AA);

    // draw shot contour
    // for (size_t idx = 0; idx < shot_contours.size(); idx++)
    // {
    //     cv::drawContours(result_plot, shot_contours, idx,  util::RED, 3);
    // }

    // draw shot location (centre point)
    // cv::circle(result_plot, shot_location, 2, WHITE, 3, cv::LINE_AA);
    cv::drawMarker(result_plot, shot_location, util::WHITE, cv::MARKER_CROSS, 20, 3);

    // add shot location values
    s0 = "LOC ";
    ss << s0 << s1 << shot_location.x << s2 << shot_location.y << s3;
    cv::putText(result_plot, ss.str(), cv::Point(shot_location.x + 20, shot_location.y),
        cv::FONT_HERSHEY_SIMPLEX, 1, util::WHITE, 3);
    ss.str(std::string());

    // add scores
    s1 = "SCORE: ";
    ss << s1 << std::fixed << std::setprecision(2) << score;
    cv::putText(
        result_plot, ss.str(), cv::Point(20, 50), cv::FONT_HERSHEY_SIMPLEX, 1, util::WHITE, 3);

    // TODO get output path as an argument
    cv::imwrite("../output/output.png", result_plot);

    cv::imshow("shot", result_plot);
    cv::waitKey(0);
    cv::destroyAllWindows();
}

/* uses shot contours to cumpute the shot location */
void ShootingScore::computeShotLocation()
{
    shot_location = cv::Point(0, 0);

    // as the shot contour can be irregular,
    // we need consider all shapes,
    // and compute the actual centre of all contours
    for (auto &&contour : shot_contours)
    {
        cv::Point contour_centre;
        util::getContourCentre(contour, contour_centre);
        shot_location.x = shot_location.x + contour_centre.x;
        shot_location.y = shot_location.y + contour_centre.y;
    }

    // get a average centre of all contours
    shot_location.x = shot_location.x / shot_contours.size();
    shot_location.y = shot_location.y / shot_contours.size();
}

void ShootingScore::computeShootingScore()
{
    // compute shot distances
    shot_distance = cv::norm(target_centre - shot_location);

    double distance = total_radius / 10;
    double num_distances = (shot_distance / distance);

    score = 11 - num_distances;
}