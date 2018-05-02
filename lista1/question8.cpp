#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Mat src, srcCallback, srcCallbackOriginal, srcDFT, srcFloat;
Point begin, end;
bool drag = false;
float D;

void onMouse(int evt, int x, int y, int flags, void *param ) {
    Mat *srcCallback = (Mat*) param;
    if (evt == CV_EVENT_LBUTTONDOWN) {
//        vector<cv::Point> *ptPtr = (vector<Point> *) param;
//        ptPtr->push_back(cv::Point(x, y));
        ::begin.x = x;
        ::begin.y = y;
        drag = true;
    }else if(evt == CV_EVENT_MOUSEMOVE && drag){
        *srcCallback = ::srcCallbackOriginal.clone();
        ::end.x = x;
        ::end.y = y;
        ::D = (float) sqrt(pow(x - ::begin.x, 2) + pow(y - ::begin.y, 2));
        circle(*srcCallback, ::begin, D, Scalar(255, 0, 0));
        imshow("Select ROI", *srcCallback);
    }else if(evt == CV_EVENT_LBUTTONUP && drag){
        *srcCallback = ::srcCallbackOriginal.clone();
        ::end.x = x;
        ::end.y = y;
        ::D = (float) sqrt(pow(x - ::begin.x, 2) + pow(y - ::begin.y, 2));
        circle(*srcCallback, ::begin, D, Scalar(255, 0, 0));
        imshow("Select ROI", *srcCallback);
        ::drag = false;
        cout << ::begin.x << " " << ::begin.y << endl;
        cout << ::end.x << " " << ::end.y << endl;
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

void createIdealFilter(Size size, Mat &dst, int uX, int uY, float radius) {
    Mat mask = Mat(size, CV_32F);

    for (int i = 0; i < size.height; ++i) {
        for (int j = 0; j < size.width; ++j) {
            float D = (float) sqrt(pow(i - uX, 2) + pow(j - uY, 2));
            if (D > radius) {
                mask.at<float>(i, j) = 0.0f;
            } else {
                mask.at<float>(i, j) = 1.0f;
            }
        }
    }
    Mat complexOutput;
    Mat output[2] = {mask, mask};
    merge(output, 2, complexOutput);

    dst = complexOutput;
}

int main(int argc, char **argv) {

    src = imread("../images/mit_noise_periodic.jpg", CV_LOAD_IMAGE_GRAYSCALE);
//    src = imread("../images/Man on the moon - noisy.png", CV_LOAD_IMAGE_GRAYSCALE);

    if (src.empty())
        return -1;

    namedWindow("Select ROI", 1);
    setMouseCallback("Select ROI", onMouse, (void *)&srcCallback);

    src.convertTo(srcFloat, CV_32FC1, 1.0 / 255.0);
    calculateDFT(srcFloat, srcDFT);
    plotDFTMagnitudeSpectrum(srcDFT, srcCallback, true);
    srcCallbackOriginal = srcCallback.clone();

    imshow("Select ROI", srcCallback);
    waitKey();

    Mat mask, filtered, idft;

    createIdealFilter(Size(srcDFT.cols, srcDFT.rows), mask, ::begin.y, ::begin.x, D);
    shiftDFT(srcDFT);
    cout << srcDFT.size() << " " << mask.size() << endl;
    mulSpectrums(srcDFT, mask, filtered, DFT_COMPLEX_OUTPUT);
    shiftDFT(filtered);

    calculateIDFT(filtered, idft);
    normalize(idft, idft, 0, 255, NORM_MINMAX, CV_8UC1);

    namedWindow("Filtered", 1);
    imshow("Filtered", idft);
    waitKey();

    return 0;
}