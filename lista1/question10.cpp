#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void calculateDFT(Mat &src, Mat &dst) {
    Mat srcComplex[2] = {src, Mat::zeros(src.size(), CV_32F)};
    Mat dftReady;
    merge(srcComplex, 2, dftReady);

    Mat srcDFT;
    dft(dftReady, srcDFT, DFT_REAL_OUTPUT);

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

double calculateRMSE(Mat &mat1, Mat &mat2){
    Mat diff = mat1 - mat2;
    Mat square = diff.mul(diff);
    double s = sum(square)[0];
    double rmse = s / (mat1.rows * mat1.cols); 
    cout << rmse << endl;
    return rmse;
}

int main(int argc, char ** argv)
{
    Mat src, srcGT, srcFloat, srcDFT, idft;
    
    src = imread("images/lighthouse_blurred.png", CV_LOAD_IMAGE_GRAYSCALE);
    srcGT = imread("images/lighthouse.png", CV_LOAD_IMAGE_GRAYSCALE);

    src.convertTo(srcFloat, CV_32FC1, 1.0 / 255.0);
    calculateDFT(srcFloat, srcDFT);

    double k = calculateRMSE(srcGT, src);
    
    Mat power = srcDFT.mul(srcDFT);
    Mat inverse = 1/srcDFT;

    Mat wiener = inverse.mul(power / (power + k));
    Mat result = srcDFT.mul(wiener);

    
    

    // mulSpectrums(1/srcDFT, (power / (power + 1)), wiener, DFT_REAL_OUTPUT);
    // mulSpectrums(srcDFT, wiener, result, DFT_REAL_OUTPUT);
    Mat finalF;
    calculateIDFT(result, idft);
    
    normalize(idft, finalF, 0, 1, NORM_MINMAX);
    cout << finalF << endl;
    // Mat x = srcDFT * srcDFT;

    cout << srcDFT.at<float>(10, 10) << " " << power.at<float>(10, 10) << endl;

    imshow("Result", finalF);
    waitKey();

    return 0;
}