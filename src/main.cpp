#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

void showWindows(std::initializer_list<cv::Mat> windows);
void getCroppedBoard(cv::Mat& input_img, cv::Mat& output_img);
void getTokenImgs(cv::Mat& cropped_board, std::vector<cv::Vec3f>& circles);

const int MEDIAN_BLUR_SIZE = 3;

int main(int argc, char** argv){
    cv::Mat rgb_board;
    rgb_board = cv::imread("../../images/catan.jpg", cv::IMREAD_COLOR);

    if (rgb_board.empty()){
        std::cout << "Cannot read the input source image." << std::endl;
        return -1;
    }
    cv::Mat cropped_board;
    getCroppedBoard(rgb_board, cropped_board);
    //Now lets extract the tokens from the board and create CatanToken objects.
    std::vector<cv::Vec3f> circles;
    getTokenImgs(cropped_board, circles);
    
    for (std::size_t i = 0; i < circles.size(); ++i){
        cv::Vec3i c = circles[i];
        cv::Point center = cv::Point(c[0], c[1]);
        circle( cropped_board, center, 1, 
                cv::Scalar(0, 100, 100), 3, cv::LINE_AA);
        int radius = c[2];
        std::cout << "Radius: "<< radius << std::endl;
        circle( cropped_board, center, radius, 
                cv::Scalar(255, 0, 255), 3, cv::LINE_AA);
    }

    //Once tokens are extracted, we will extract the tiles and assign CatanTokens to be their members.

    showWindows({rgb_board, cropped_board});
    return 0;
}

void getTokenImgs(cv::Mat& cropped_board, std::vector<cv::Vec3f>& circles){
    cv::Mat hsv_board;
    cv::cvtColor(cropped_board, hsv_board, cv::COLOR_BGR2HSV);
    cv::Mat yellow_board;
    cv::medianBlur(hsv_board, hsv_board, 11);
    cv::inRange(hsv_board, cv::Scalar(10, 30, 150), 
                cv::Scalar(30, 100, 255), yellow_board);
    cv::HoughCircles(   yellow_board, circles, cv::HOUGH_GRADIENT, 1,
                        20,
                        150, 10, 4, 20);
}

void getCroppedBoard(cv::Mat& input_img, cv::Mat& output_img){
    //we will need to convert to HSV coordinates to allow for easier color detection
    cv::Mat hsv_board;
    cv::cvtColor(input_img, hsv_board, cv::COLOR_BGR2HSV);
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
    output_img = input_img.clone();
    output_img.setTo(cv::Scalar(0,0,0));
    input_img.copyTo(output_img, cont_mask);
}

void showWindows(std::initializer_list<cv::Mat> windows){
    unsigned int i = 0, j = 0;
    for (auto window : windows){
        cv::imshow(std::to_string(i), window);
        const int windowWidth = cv::getWindowImageRect(std::to_string(i)).size().width;
        const int windowHeight = cv::getWindowImageRect(std::to_string(i)).size().height;
        cv::moveWindow(std::to_string(i), windowWidth * (i % 3), windowHeight * (j % 3));
        // cv::setMouseCallback(std::to_string(i), onMouse, static_cast<void*>(window));
        ++i;
        if (i % 3 == 0)
            ++j;
    }
    cv::waitKey(0);
}