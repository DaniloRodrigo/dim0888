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



int main(int argc, char **argv) {

    src = imread("../images/mit_noise_periodic.jpg", CV_LOAD_IMAGE_GRAYSCALE);
//    src = imread("../images/Man on the moon - noisy.png", CV_LOAD_IMAGE_GRAYSCALE);

    if (src.empty())
        return -1;
    namedWindow("Select ROI", 1);
    setMouseCallback("Select ROI", onMouse, (void *)&src);
    imshow("Select ROI", src);
    waitKey();

    return 0;
}