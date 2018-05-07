#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void calculateDFT(Mat& src, Mat& dst){
    Mat srcComplex[2] = {src, Mat::zeros(src.size(), CV_32F)};
    Mat dftReady;
    merge(srcComplex, 2, dftReady);

    Mat srcDFT;
    dft(dftReady, srcDFT, DFT_COMPLEX_OUTPUT);

    dst = srcDFT;
}

void calculateIDFT(Mat& src, Mat& dst){
    Mat inverse;

    dft(src, inverse, DFT_INVERSE | DFT_REAL_OUTPUT | DFT_SCALE);

    dst = inverse;
}

void shiftDFT(Mat& src){
    int centerX = src.cols/2;
    int centerY = src.rows/2;

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

void plotDFTMagnitudeSpectrum(Mat& srcDFT){
    Mat splitArray[2] = {Mat::zeros(srcDFT.size(), CV_32F), Mat::zeros(srcDFT.size(), CV_32F)};
    split(srcDFT, splitArray);
    Mat dftMagnitude;
    magnitude(splitArray[0], splitArray[1], dftMagnitude);
    dftMagnitude += Scalar::all(1);
    log(dftMagnitude, dftMagnitude);
    normalize(dftMagnitude, dftMagnitude, 0, 1, NORM_MINMAX);

    shiftDFT(dftMagnitude);
    imshow("Magnitude Spectrum", dftMagnitude);
    waitKey();
}

void createIdealFilter(Size size, Mat& dst, int uX, int uY){
    Mat mask = Mat(size, CV_32F);

    for (int i = 0; i < size.height; ++i) {
        for (int j = 0; j < size.width; ++j) {
            float D = (float) sqrt(pow(i - uX, 2) + pow(j - uY, 2));
            if(D > 40){
                mask.at<float>(i, j) = 0.0f;
            }else{
                mask.at<float>(i, j) = 1.0f;
            }
        }
    }
    Mat complexOutput;
    Mat output[2] = {mask, mask};
    merge(output, 2, complexOutput);

    dst = complexOutput;
}

void createButterworthFilter(Size size, Mat& dst, int uX, int uY, int n, int d0){
    Mat mask = Mat(size, CV_32F);

    for (int i = 0; i < size.height; ++i) {
        for (int j = 0; j < size.width; ++j) {
            float D = (float) sqrt(pow(i - uX, 2) + pow(j - uY, 2));
            float value = ( 1 / ( 1 + (pow(D/(float)d0, 2 * n))));
            mask.at<float>(i, j) = value;
        }
    }
    Mat complexOutput;
    Mat output[2] = {mask, mask};
    merge(output, 2, complexOutput);

    dst = complexOutput;
}

void createButterworthFilter4(Size size, Mat& dst, int uX, int uY, int n, int d0){
    Mat mask = Mat(size, CV_32F);


    for (int i = 0; i < size.height/2; ++i) {
        for (int j = 0; j < size.width/2 ; ++j) {
            float D = (float) sqrt(pow(i - uX/2, 2) + pow(j - uY/2 , 2));
            float value = ( 1 / ( 1 + (pow(D/(float)d0, 2 * n))));
            mask.at<float>(i, j) = value;
            mask.at<float>(i + uX, j) = value; 
            mask.at<float>(i, j + uY) = value;
            mask.at<float>(i + uX, j + uY) = value;
        }
    }
    Mat complexOutput;
    Mat output[2] = {mask, mask};
    merge(output, 2, complexOutput);

    dst = complexOutput;
}


int main(int argc, char ** argv)
{

    Mat src = imread("images/man-on-the-moon-noisy.png", CV_LOAD_IMAGE_GRAYSCALE);
    // Mat src = imread("images/pompei-periodic-noise.jpg", CV_LOAD_IMAGE_GRAYSCALE);

    Mat srcFloat, srcDFT;
    src.convertTo(srcFloat, CV_32FC1, 1.0 / 255.0);

    calculateDFT(srcFloat, srcDFT);
    Mat mask;

    createButterworthFilter(Size(srcDFT.cols, srcDFT.rows), mask, srcDFT.rows/2 , srcDFT.cols/2, 1, 25);
    // shiftDFT(mask);
    // plotDFTMagnitudeSpectrum(mask);
    shiftDFT(srcDFT);

//    Mat filtered =  srcDFT * mask;
    Mat filtered;
    mulSpectrums(srcDFT, mask, filtered, DFT_COMPLEX_OUTPUT);
    shiftDFT(filtered);
    // plotDFTMagnitudeSpectrum(filtered);


    Mat idft;
    calculateIDFT(filtered, idft);
//    for (int i = 0; i < srcDFT.size().height; ++i) {
//        for (int j = 0; j < srcDFT.size().width; ++j) {
//            idft.at<float>(i, j) = (float) srcDFT.at<float>(i, j) * (float) mask.at<float>(i, j);
//        }
//    }


    cout << mask.channels() << " " << srcDFT.channels() << endl;


    normalize(idft, idft, 0, 255, NORM_MINMAX, CV_8UC1);
//    cout << idft << endl;

    imshow("Original", src);
//    imshow("Mask", mask);
    imshow("Recovered", idft);
    waitKey();

    return 0;
}