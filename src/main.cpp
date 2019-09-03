#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

void showWindows(std::initializer_list<cv::Mat> windows);

int main(int argc, char** argv){
    std::cout << "Hello world" << std::endl;
    cv::Mat rgb_board;
    rgb_board = cv::imread("../../images/catan.jpg", cv::IMREAD_COLOR);

    if (rgb_board.empty()){
        std::cout << "Cannot read the input source image." << std::endl;
        return -1;
    }
    
    //we will need to convert to HSV coordinates to allow for easier color detection
    cv::Mat hsv_board;
    cv::cvtColor(rgb_board, hsv_board, cv::COLOR_BGR2HSV);
    cv::Mat blue_board;
    cv::inRange(hsv_board, cv::Scalar(80, 0, 0), cv::Scalar(180, 255, 255), blue_board);

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> heirarchy;
    cv::findContours(blue_board, contours, heirarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    cv::Mat cont_drawing = cv::Mat::zeros(blue_board.size().height, blue_board.size().width, CV_8U);
    for (unsigned int i = 0; i < contours.size(); ++i){
        cv::drawContours(cont_drawing, contours, i, cv::Scalar(100, 100, 50));
    }
    showWindows({rgb_board, hsv_board, blue_board, cont_drawing});
    // cv::imshow(RGB_WINDOW, rgb_board);
    // cv::imshow(HSV_WINDOW, hsv_board);
    // cv::imshow(BLUE_WINDOW, blue_board);
    // cv::imshow("contours", cont_drawing);
    // const int windowSize = cv::getWindowImageRect(RGB_WINDOW).size().width;
    // cv::moveWindow(HSV_WINDOW, windowSize, 0);
    // cv::moveWindow(BLUE_WINDOW, 2*windowSize, 0);
    cv::waitKey(0);
    return 0;
}

void showWindows(std::initializer_list<cv::Mat> windows){
    unsigned int i = 0;
    for (auto window : windows){
        cv::imshow(std::to_string(i), window);
        const int windowSize = cv::getWindowImageRect(std::to_string(i)).size().width;
        cv::moveWindow(std::to_string(i), windowSize * i, 0);
        ++i;
    }
}