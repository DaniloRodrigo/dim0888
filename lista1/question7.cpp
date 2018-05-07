#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int calculateGaussian(Mat &src, int x, int y, int size){
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

//    for (int k = 0; k < arr.size(); ++k) {
//        cout << arr[k] << endl;
//    }

//    cout << arr[((int)arr.size()/2)] << endl;

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

int calculateVariance(Mat &src, int x, int y, int size){
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

    return round(sum/arr.size());
}

void applyHighBoost(Mat &src, Mat &dst, int filterSize, int k, string name){
    int size = (int) filterSize / 2;

    copyMakeBorder( src, dst, size, size, size, size, BORDER_CONSTANT, 0 );

    for (int i = size; i < src.rows; ++i) {
        for (int j = size; j < src.cols; ++j) {
            dst.at<uchar>(i, j) = (int) calculateMean(dst, i, j, size);
        }
    }

    dst = dst.colRange(size, dst.cols - size).rowRange(size, dst.rows - size).clone();

    Mat mask = src.clone();


   mask = src - dst;

    // for (int i = 0; i < src.rows; ++i) {
    //     for (int j = 0; j < src.cols; ++j) {
    //         int x = abs((int) src.at<uchar>(i, j) - (int) dst.at<uchar>(i, j));
    //         mask.at<uchar>(i, j) = x;
    //         cout << (int) src.at<uchar>(i, j) << " " <<  (int) dst.at<uchar>(i, j) << " " << (int) mask.at<uchar>(i, j) << " " << x << endl;
    //     }
    // }


    Mat result = src +  (k * mask);

    // imshow("Original", src);
    // imshow("Equalized", dst);
    imwrite("output/question7/blurred_" + name, dst);
    // imshow("Mask", mask);
    imwrite("output/question7/mask_" + name, mask);
    // imshow("Final", result);
    imwrite("output/question7/result_" + name, result);
    waitKey(0);
    // destroyAllWindows(); 
}

int main() {
    Mat src; Mat dst;
    string images [] = {
           "images/google.jpg",
           "output/question5/google.jpg",
           "images/woman.jpg",
           "output/question5/woman.jpg"
    };
    string names [] = {
           "google.jpg",
           "google_hist.jpg",
           "woman.jpg",
           "woman_hist.jpg"
    };


    for (int i = 0; i < images->size(); ++i) {
        src = imread(images[i], 0);
        dst = src.clone();
        applyHighBoost(src, dst, 3, 2, names[i]);
    }

    return 0;
}