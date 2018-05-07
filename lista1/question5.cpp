#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


void equalizeHistogram(Mat &src, Mat &dst, string name){
    int hist [256];
    double equalizedHist [256];

    // inicializing
    for (int i = 0; i < 256; ++i) {
        equalizedHist[i] = 0;
        hist[i] = 0;
    }

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
    // imwrite("output/question5/" + name, dst);
    waitKey(0);
    destroyAllWindows();
}

int main() {
    Mat src; Mat dst;
    string images [] = {
            // "bear.png",
            // "boat.png",
            // "glass.png",
           "google.jpg",
//            "lighthouse.png",
//            "lighthouse_blurred.png",
            // "lions.png",
            // "lungs10.jpg",
            // "lungs30.jpg",
            // "lungs50.jpg",
//            "man-on-the-moon-noisy.png",
//            "mit_noise_periodic.jpg",
//            "pompei-periodic-noise.jpg",
           "woman.jpg"
    };

    for (int i = 0; i < images->size(); ++i) {
        src = imread("images/" + images[i], 0);
        dst = src.clone();
        equalizeHistogram(src, dst, images[i]);
    }
    
    return 0;
}