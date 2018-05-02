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

int main() {
    Mat src; Mat dst;
    Scalar value;
    int pixel;

    src = imread("../images/woman low contrast.jpg", 0);
    dst = src.clone();


    int size = (int) 3 / 2;

    copyMakeBorder( src, dst, size, size, size, size, BORDER_CONSTANT, 0 );

    for (int i = size; i < src.rows; ++i) {
        for (int j = size; j < src.cols; ++j) {
            dst.at<uchar>(i, j) = (int) calculateMean(dst, i, j, size);
        }
    }

    dst = dst.colRange(size, dst.cols - size).rowRange(size, dst.rows - size).clone();

    Mat mask = src.clone();


//    mask = src - dst;

    for (int i = 0; i < src.rows; ++i) {
        for (int j = 0; j < src.cols; ++j) {
            int x = abs((int) src.at<uchar>(i, j) - (int) dst.at<uchar>(i, j));
//            mask.at<uchar>(i, j) = src.at<uchar>(i, j) - dst.at<uchar>(i, j);
            mask.at<uchar>(i, j) = x;
            cout << (int) src.at<uchar>(i, j) << " " <<  (int) dst.at<uchar>(i, j) << " " << (int) mask.at<uchar>(i, j) << " " << x << endl;
        }
    }


    Mat final = src +  (2 * mask);

//    cout << final << endl;

    imshow("Original", src);
    imshow("Equalized", dst);
    imshow("Mask", mask);
    imshow("Final", final);
    waitKey(0);
    destroyAllWindows();
    return 0;
}