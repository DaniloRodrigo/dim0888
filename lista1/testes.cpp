#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Mat src, srcCallback, srcCallbackOriginal, srcDFT, srcFloat;

void onMouse(int evt, int x, int y, int flags, void *param ) {
    if (evt == CV_EVENT_LBUTTONDOWN) {
        cout << x << " " << y << endl;
    }
}

void calculateDFT(Mat &src, Mat &dst) {
    Mat srcComplex[2] = {src, Mat::zeros(src.size(), CV_32F)};
    Mat dftReady;
    merge(srcComplex, 2, dftReady);

    Mat srcDFT;
    dft(dftReady, srcDFT, DFT_COMPLEX_OUTPUT);

    dst = srcDFT;
}

void calculateIDFT(Mat &src, Mat &dst) {
    Mat inverse;

    dft(src, inverse, DFT_INVERSE | DFT_REAL_OUTPUT | DFT_SCALE);

    dst = inverse;
}

void shiftDFT(Mat &src) {
    int centerX = src.cols / 2;
    int centerY = src.rows / 2;

    Mat q1(src, Rect(0, 0, centerX, centerY));
    Mat q2(src, Rect(centerX, 0, centerX, centerY));
    Mat q3(src, Rect(0, centerY, centerX, centerY));
    Mat q4(src, Rect(centerX, centerY, centerX, centerY));

    Mat tmp;

    q1.copyTo(tmp);
    q4.copyTo(q1);
    tmp.copyTo(q4);

    q2.copyTo(tmp);
    q3.copyTo(q2);
    tmp.copyTo(q3);
}

void plotDFTMagnitudeSpectrum(Mat &srcDFT, Mat &dst, bool inverse) {
    Mat splitArray[2] = {Mat::zeros(srcDFT.size(), CV_32F), Mat::zeros(srcDFT.size(), CV_32F)};
    split(srcDFT, splitArray);
    Mat dftMagnitude;
    magnitude(splitArray[0], splitArray[1], dftMagnitude);
    dftMagnitude += Scalar::all(1);
    log(dftMagnitude, dftMagnitude);
    normalize(dftMagnitude, dftMagnitude, 0, 1, NORM_MINMAX);

    if(inverse)
        shiftDFT(dftMagnitude);
    dst = dftMagnitude;
}

int main(int argc, char **argv) {
    Mat test;

    src = imread("images/pompei-periodic-noise.jpg", CV_LOAD_IMAGE_GRAYSCALE);
//    src = imread("../images/Man on the moon - noisy.png", CV_LOAD_IMAGE_GRAYSCALE);

    if (src.empty())
        return -1;
    namedWindow("Select ROI", 1);
    setMouseCallback("Select ROI", onMouse, (void *)&src);
    src.convertTo(srcFloat, CV_32FC1, 1.0 / 255.0);
    calculateDFT(srcFloat, srcDFT);
    plotDFTMagnitudeSpectrum(srcDFT, test, true);

    imshow("Select ROI", test);
    waitKey();

    return 0;
}