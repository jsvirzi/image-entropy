#include <iostream>

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/stitching.hpp"

using namespace std;
using namespace cv;

bool try_use_gpu = false;
bool divide_images = false;
Stitcher::Mode mode = Stitcher::PANORAMA;
vector<Mat> imgs;
string result_name = "result.jpg";

int xmain() {
    std::cout << "Hello, World!" << std::endl;
    Mat pano;
    Ptr<Stitcher> stitcher = Stitcher::create(mode);
    Stitcher::Status status = stitcher->stitch(imgs, pano);
    return 0;
}

double log_factorial_lut[256];

void make_lut() {
    log_factorial_lut[0] = 0.0;
    log_factorial_lut[1] = 0.0;
    double acc = 0.0;
    for (int i = 2; i < 256; ++i) {
        acc = acc + log(i);
        double stirling = i * log(i) - i;
        log_factorial_lut[i] = acc;
        printf("%d => factorial = %g. compare to %g\n", i, acc, stirling);
    }
}

int main(int argc, char **argv)
{

    make_lut();

    cv::Mat img = cv::imread(argv[1], IMREAD_GRAYSCALE);

    if (img.empty()) {
        cout << "Not a valid image file" << endl;
        return -1;
    }

    cv::namedWindow("Simple Demo", cv::WINDOW_AUTOSIZE);
    cv::imshow("Simple Demo", img);

    int type = img.type();

    printf("type = %d. CV_U8 = %d / %d / %d\n", type, CV_8U, CV_8UC1);

    int rows = img.rows;
    int cols = img.cols;
    int N = 0;

    unsigned long int n = 0;
    double acc = 0;
    int start_row = 1 * img.rows / 4;
    int final_row = 3 * img.rows / 4;
    int start_col = 1 * img.cols / 4;
    int final_col = 3 * img.cols / 4;
    for (int i = start_row; i < final_row; ++i) {
        for (int j = start_col; j < final_col; ++j) {
            uint8_t byte = img.at<uchar>(i, j);
            n += byte;
            acc += log_factorial_lut[byte];
            ++N;
        }
    }

    unsigned long int n_lo = n / N;
    unsigned long int n_diff = N * n_lo;
    double n_bar = ((double) n) / N;
    double n_max = N * log(gamma(n_bar + 1.0));
    printf("total energy = %lu. rows = %d/%d. cols = %d/%d. average energy = %g\n", n, start_row, final_row, start_col, final_col, n_bar);
    printf("measure = %g = (%g - %g)\n", n_max - acc, n_max, acc);

    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
