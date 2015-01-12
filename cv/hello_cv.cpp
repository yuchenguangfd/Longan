/*
 * hello_cv.cpp
 * Created on: Jan 10, 2015
 * Author: chenguangyu
 */

#include <opencv2/opencv.hpp>
using namespace cv;

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: hellp_cv img_filename\n");
        return -1;
    }
    Mat image;
    image = imread(argv[1], 1);
    if (!image.data) {
        printf("No image data \n");
        return -1;
    }
    namedWindow("Display Image", WINDOW_AUTOSIZE);
    imshow("Display Image", image);
    waitKey(0);
    return 0;
}
