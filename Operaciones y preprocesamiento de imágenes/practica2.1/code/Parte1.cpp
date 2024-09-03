#include <iostream>
#include <opencv2/opencv.hpp>
#include <chrono>

using namespace std;
using namespace cv;

inline void detailEnhance(const Mat& src, Mat& dst, float sigma_s = 10, float sigma_r = 0.15f) {
    Mat smoothed;
    bilateralFilter(src, smoothed, -1, sigma_s, sigma_r);

    subtract(src, smoothed, dst);
    dst = src + dst;
}

int main(int argc, char* args[]) {
    VideoCapture video("http://47.51.131.147/-wvhttp-01-/GetOneShot?image_size=1280x720&frame_count=1000000000");

    if (!video.isOpened()) {
        cout << "No se puede abrir la cámara..." << endl;
        return -1;
    }

    Ptr<BackgroundSubtractorMOG2> pMOG2 = createBackgroundSubtractorMOG2();
    const int MAX_FRAMES = 1000;
    const double LEARNING_RATE = -1;

    Mat frame, motion_mask, equalized_frame, clahe_frame, detail_enhanced_frame;
    Mat background;

    auto start = chrono::steady_clock::now();
    int frameCount = 0;

    namedWindow("Video Original", WINDOW_FREERATIO);
    namedWindow("Deteccion de Movimiento", WINDOW_FREERATIO);
    namedWindow("Filtro de Equalizacion de Histograma", WINDOW_FREERATIO);
    namedWindow("Filtro de CLAHE", WINDOW_FREERATIO);
    namedWindow("Filtro de Detail Enhanced", WINDOW_FREERATIO);

    for (int t = 0; t < MAX_FRAMES; ++t) {
        video >> frame;

        pMOG2->apply(frame, motion_mask, LEARNING_RATE);

        cvtColor(frame, equalized_frame, COLOR_BGR2YUV);
        vector<Mat> channels;
        split(equalized_frame, channels);
        equalizeHist(channels[0], channels[0]);
        merge(channels, equalized_frame);
        cvtColor(equalized_frame, equalized_frame, COLOR_YUV2BGR);

        cvtColor(frame, clahe_frame, COLOR_BGR2Lab);
        vector<Mat> lab_channels;
        split(clahe_frame, lab_channels);
        Ptr<CLAHE> clahe = createCLAHE();
        clahe->apply(lab_channels[0], lab_channels[0]);
        merge(lab_channels, clahe_frame);
        cvtColor(clahe_frame, clahe_frame, COLOR_Lab2BGR);

        detailEnhance(frame, detail_enhanced_frame);

        auto end = chrono::steady_clock::now();
        double elapsed_seconds = chrono::duration_cast<chrono::duration<double>>(end - start).count();
        if (elapsed_seconds >= 1.0) {
            double fps = frameCount / elapsed_seconds;
            frameCount = 0;
            start = chrono::steady_clock::now();
            putText(frame, "FPS: " + to_string(int(fps)), Point(10, 30), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 1);
        }
        frameCount++;

        imshow("Video Original", frame);
        imshow("Deteccion de Movimiento", motion_mask);

        imshow("Filtro de Equalizacion de Histograma", equalized_frame);
        imshow("Filtro de CLAHE", clahe_frame);
        imshow("Filtro de Detail Enhanced", detail_enhanced_frame);

        if (frame.empty() || waitKey(23) == 27) break;
    }

    destroyAllWindows();
    return 0;
}
