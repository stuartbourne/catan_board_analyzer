#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

void showWindows(std::initializer_list<cv::Mat> windows);

const int MEDIAN_BLUR_SIZE = 7;

int main(int argc, char** argv){
    cv::Mat rgb_board;
    rgb_board = cv::imread("../../images/catan.jpg", cv::IMREAD_COLOR);

    if (rgb_board.empty()){
        std::cout << "Cannot read the input source image." << std::endl;
        return -1;
    }
    
    //we will need to convert to HSV coordinates to allow for easier color detection
    cv::Mat hsv_board;
    cv::cvtColor(rgb_board, hsv_board, cv::COLOR_BGR2HSV);
    //blur to allow for less contours
    cv::medianBlur(hsv_board, hsv_board, MEDIAN_BLUR_SIZE);
    cv::Mat blue_board;
    cv::inRange(hsv_board, cv::Scalar(80, 0, 0), cv::Scalar(180, 255, 255), blue_board);

    std::vector<cv::Mat> contours;
    std::vector<cv::Vec4i> heirarchy;
    cv::findContours(blue_board, contours, heirarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    cv::Mat cont_mask = cv::Mat::zeros(blue_board.size().height, blue_board.size().width, CV_8U);
    double maxArea = 0;
    unsigned int maxContIdx = 0;
    for (unsigned int i = 0; i < contours.size(); ++i){
        double currentArea = cv::contourArea(contours.at(i));
        if (currentArea > maxArea){
             maxArea = currentArea;
             maxContIdx = i;
        }
    }
    cv::drawContours(cont_mask, contours, maxContIdx, cv::Scalar(180, 180, 180), cv::FILLED);
    
    cv::Mat cropped(rgb_board.rows, rgb_board.cols, CV_8UC3);
    cropped.setTo(cv::Scalar(0,0,0));
    rgb_board.copyTo(cropped, cont_mask);

    showWindows({rgb_board, hsv_board, blue_board, cont_mask, cropped});
    return 0;
}

void showWindows(std::initializer_list<cv::Mat> windows){
    unsigned int i = 0, j = 0;
    for (auto window : windows){
        cv::imshow(std::to_string(i), window);
        const int windowWidth = cv::getWindowImageRect(std::to_string(i)).size().width;
        const int windowHeight = cv::getWindowImageRect(std::to_string(i)).size().height;
        cv::moveWindow(std::to_string(i), windowWidth * (i % 3), windowHeight * (j % 3));
        ++i;
        if (i % 3 == 0)
            ++j;
    }
    cv::waitKey(0);
}