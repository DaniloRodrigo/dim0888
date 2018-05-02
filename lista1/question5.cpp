#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Mat src; Mat dst;
int hist [256];
double equalizedHist [256];
int pixel;

void equalizeHistogram(Mat src){

}

int main() {
//    namedWindow( window_name, WINDOW_AUTOSIZE );
//    src = imread("../images/bear s and p.png", 0);
//    src = imread("../images/Google JAX low contrast.jpg", 0);
    src = imread("../images/woman low contrast.jpg", 0);
    dst = src.clone();

    // Calculating the histogram
    for (int i = 0; i < src.rows; ++i) {
        for (int j = 0; j < src.cols; ++j) {
            int pixel = src.at<uchar>(i, j);
            hist[pixel]++;
        }
    }

    // Equalizing the histogram
    int numberPixel = src.cols * src.rows;
    for (int i = 0; i < 256; ++i) {
        equalizedHist[i] = hist[i] / (double)numberPixel;
    }

    for (int i = 1; i < 256; ++i) {
        equalizedHist[i] = equalizedHist[i] + equalizedHist[i-1];
    }

    for (int i = 0; i < 256; ++i) {
        equalizedHist[i] = equalizedHist[i] * 255;
    }


    // Appling the equalized histogram to the original image
    for (int i = 0; i < src.rows; ++i) {
        for (int j = 0; j < src.cols; ++j) {
            int pixel = dst.at<uchar>(i, j);
            dst.at<uchar>(i, j) = round(equalizedHist[pixel]);
        }
    }

    imshow("Original", src);
    imshow("Equalized", dst);
    imwrite("../output/woman_hist.jpg", dst);
    waitKey(0);
    destroyAllWindows();
    return 0;
}