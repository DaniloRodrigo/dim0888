#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int calculateMedian(Mat &src, int x, int y, int size){
    vector< int > arr;
    int median;
    int pixel;
    for (int i = x-size; i <= x+size; ++i) {
        for (int j = y-size; j <= y+size; ++j) {
            pixel = src.at<uchar>(i, j);
            arr.push_back(pixel);
        }
    }

    sort(arr.begin(), arr.end());

    return  arr[((int)arr.size()/2)];

}

int calculateMean(Mat &src, int x, int y, int size){
    vector< int > arr;
    int median;
    int pixel;
    for (int i = x-size; i <= x+size; ++i) {
        for (int j = y-size; j <= y+size; ++j) {
            pixel = src.at<uchar>(i, j);
            arr.push_back(pixel);
        }
    }

    int sum = 0;
    for (int k = 0; k < arr.size(); ++k) {
        sum += arr[k];
    }

    return (int) sum/arr.size();
}


float calculateCHMean(Mat &src, int x, int y, int size, float q){
    vector< int > arr;
    int pixel;
    for (int i = x-size; i <= x+size; ++i) {
        for (int j = y-size; j <= y+size; ++j) {
            pixel = src.at<uchar>(i, j);
            arr.push_back(pixel);
        }
    }

    float numerator = 0.0f;
    float denominator = 0.0f;
    for (int k = 0; k < arr.size(); ++k) {
        numerator += (float) pow(arr[k], q+1);
        if(arr[k] != 0)
            denominator += pow(arr[k], q);
        // cout << denominator << endl;
    }

    // float ch;
    // if(denominator == 0.0f)
    //     ch = 0.0f;
    // else
    //     ch = numerator / (float)denominator;

    return numerator / (float)denominator;
}

void applyMedianFilter(Mat &src, Mat &dst, int filterSize, string name){
    int size = (int) 3 / 2;

    copyMakeBorder( src, dst, size, size, size, size, BORDER_CONSTANT, 0 );

    for (int i = size; i < src.rows; ++i) {
        for (int j = size; j < src.cols; ++j) {
            dst.at<uchar>(i, j) = (int) calculateMedian(dst, i, j, size);
        }
    }

    dst = dst.colRange(size, dst.cols - size).rowRange(size, dst.rows - size).clone();

    cout << src.rows << " " << src.cols << endl;
    cout << dst.rows << " " << dst.cols << endl;
    cout << pow(4, 2) << endl;

   imshow("Original", src);
   imshow("Equalized", dst);
    // imwrite("../output/median_" + to_string(filterSize) + "_" + name, dst);
   waitKey(0);
   destroyAllWindows();
}

void applyCHMeanFilter(Mat &src, Mat &dst, int filterSize, float q, string name){
    int size = (int) 3 / 2;

    copyMakeBorder( src, dst, size, size, size, size, BORDER_CONSTANT, 0 );

    for (int i = size; i < src.rows; ++i) {
        for (int j = size; j < src.cols; ++j) {
            float ch = calculateCHMean(dst, i, j, size, q);
            if(ch == INFINITY)
                ch = 255;
            cout << ch << endl;
            dst.at<uchar>(i, j) = ch;
        }
    }

    // cout << dst << endl;

    dst = dst.colRange(size, dst.cols - size).rowRange(size, dst.rows - size).clone();
    normalize(dst, dst, 0, 255, NORM_MINMAX, CV_8UC1);

   imshow("Original", src);
   imshow("Equalizedd", dst);
    // imwrite("../output/median_" + to_string(filterSize) + "_" + name, dst);
   waitKey(0);
   destroyAllWindows();
}

int main() {
    Mat src; Mat dst;
    string images [] = {
            "bear.png",

            // "boat.png",
            // "glass.png",

//            "google.jpg",
//            "lighthouse.png",
//            "lighthouse_blurred.png",

            // "lions.png",
            // "lungs10.jpg",
            // "lungs30.jpg",
            // "lungs50.jpg",

//            "man-on-the-moon-noisy.png",
//            "mit_noise_periodic.jpg",
//            "pompei-periodic-noise.jpg",
//            "woman.jpg"
    };

    for (int i = 0; i < images->size(); ++i) {
        src = imread("images/" + images[i], 0);
        dst = src.clone();
        // applyMedianFilter(src, dst, 3,images[i]);
        applyCHMeanFilter(src, dst, 3, -2, images[i]);
    }

    return 0;
}