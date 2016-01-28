/**
 * @author Thomas Munoz
 */

#include <chrono>
#include <thread>
#include "MotionTrack.h"

MotionTrack::MotionTrack(Webcam webcam)
        : webcam(webcam)
{}

#if DEBUG == 1
void MotionTrack::drawContour(cv::Mat & frame, std::vector<std::vector<cv::Point>> contours) {
    // In this part we will assume that the largest moving object is the one we want to track
    // todo : enable multiple object tracking
    std::vector<std::vector<cv::Point>> largestContour;
    largestContour.push_back(contours.at(contours.size() - 1));

    // Surround the moving object by a rectangle
    cv::Rect rectangle = cv::boundingRect(largestContour.at(0));

    // Set the center of the rectangle (moving object position)
    //int x = rectangle.x + rectangle.width / 2;
    //int y = rectangle.y + rectangle.height / 2;
    //cv::circle(frame, cv::Point(x, y), 20, cv::Scalar(0, 255, 0), 2);

    // Draw a rectangle surrounding the moving object
    cv::rectangle(frame, rectangle, cv::Scalar(0, 255, 0));
}
#endif

cv::Mat MotionTrack::detect() {
    // We first get two frames (we will compare them
    cv::Mat first = this->webcam.getCurrentFrame();
    cv::Mat second = this->webcam.getCurrentFrame();

    // Convert both frames in gray (it's better to detect motion)
    cv::Mat grayFirst, graySecond;
    cv::cvtColor(first, grayFirst, cv::COLOR_BGR2GRAY);
    cv::cvtColor(second, graySecond, cv::COLOR_BGR2GRAY);

    // We compute the absolute difference between the two frames
    cv::Mat difference;
    cv::absdiff(grayFirst, graySecond, difference);

    // Generate the threshold image that will show the intensity of the difference
    cv::Mat threshold;
    cv::threshold(difference, threshold, SENSITIVITY, 255, CV_THRESH_BINARY);

    // Temporary frame in order to draw the contours
    cv::Mat temp;
    threshold.copyTo(temp);

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;

    // Retrieves external contours
    cv::findContours(temp, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    // We find an object (a motion
    if(contours.size() > 0){
        #if DEBUG == 1
            std::cout << "Motion detected !" << std::endl;
            drawContour(second, contours);
        #endif

        save(second);
    }

    return second;
}

void MotionTrack::save(cv::Mat frame) {
    std::stringstream ss;

    std::chrono::time_point<std::chrono::system_clock> start;
    start = std::chrono::system_clock::now();

    std::time_t end_time = std::chrono::system_clock::to_time_t(start);

    // Use of random to avoid same filename for the same second
    ss << std::ctime(&end_time) << '-' << rand() % 1000 + 0 << ".jpg";

    cv::imwrite(ss.str(), frame);
}
