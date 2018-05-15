#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Mat src, srcCallback, srcCallbackOriginal, srcDFT, srcFloat;

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

void plotDFTMagnitudeSpectrum(Mat srcDFT, bool inverse) {
    Mat splitArray[2] = {Mat::zeros(srcDFT.size(), CV_32F), Mat::zeros(srcDFT.size(), CV_32F)};
    split(srcDFT, splitArray);
    Mat dftMagnitude;
    magnitude(splitArray[0], splitArray[1], dftMagnitude);
    dftMagnitude += Scalar::all(1);
    log(dftMagnitude, dftMagnitude);
    normalize(dftMagnitude, dftMagnitude, 0, 1, NORM_MINMAX);

    if(inverse)
        shiftDFT(dftMagnitude);

    imshow("DFT", dftMagnitude);
    waitKey();
}

int main(int argc, char **argv) {
    Mat original, blurred, originalCropped, blurredCropped;

    original = imread("images/lighthouse.png", 0);
    blurred = imread("images/lighthouse_blurred.png", 0);

    originalCropped = Mat(original.rows, original.cols, CV_8UC1, Scalar(0));
    blurredCropped = Mat(original.rows, original.cols, CV_8UC1, Scalar(0));

    // Used on the region estimation trying
    // Rect roi = selectROI("Select region to estimate", original, false);
    // original.rowRange(roi.y, roi.y+roi.height).colRange(roi.x, roi.x+roi.width).copyTo(originalCropped.rowRange(roi.y, roi.y+roi.height).colRange(roi.x, roi.x+roi.width));
    // blurred.rowRange(roi.y, roi.y+roi.height).colRange(roi.x, roi.x+roi.width).copyTo(blurredCropped.rowRange(roi.y, roi.y+roi.height).colRange(roi.x, roi.x+roi.width));

    Mat originalFloat, blurredFloat, originalDFT, blurredDFT; 


    // originalCropped.convertTo(originalFloat, CV_32FC1, 1.0 / 255.0);
    // blurredCropped.convertTo(blurredFloat, CV_32FC1, 1.0 / 255.0);
    original.convertTo(originalFloat, CV_32FC1, 1.0 / 255.0);
    blurred.convertTo(blurredFloat, CV_32FC1, 1.0 / 255.0);
    
    calculateDFT(originalFloat, originalDFT);
    calculateDFT(blurredFloat, blurredDFT);

    // plotDFTMagnitudeSpectrum(originalDFT, true);
    // plotDFTMagnitudeSpectrum(blurredDFT, true);

    Mat mask = blurredDFT / originalDFT;
    // plotDFTMagnitudeSpectrum(mask, true);

    Mat filtered = blurredDFT.clone();
    float k = 0; 
    for (int i = 0; i < original.rows; i++){
        for (int j = 0; j < original.cols; j++){
            float H = mask.at<float>(i, j);
            float G = blurredDFT.at<float>(i, j);
            // if(H != 0){ 
            float wiener = (1/H)*(pow(H, 2) / (pow(H, 2) + k));
                // cout << wiener << endl;
            filtered.at<float>(i, j) = wiener * G;
        }
    }

    // plotDFTMagnitudeSpectrum(filtered, true);

    Mat idft, deblurred;
    calculateIDFT(filtered, idft);
    
    normalize(idft, deblurred, 0, 255, NORM_MINMAX, CV_8UC1);

    // imshow("Original", original);
    imshow("Blurred", blurred);
    imshow("Deblurred", deblurred);
    imwrite("output/question10/lighthouse_deblurred.png", deblurred);
    waitKey();
    destroyAllWindows();

    return 0;
}