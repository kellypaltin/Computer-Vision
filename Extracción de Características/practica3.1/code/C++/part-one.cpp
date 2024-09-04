#include <iostream>
#include <opencv2/opencv.hpp>
#include <jni.h>
#include "zernike.h"
#include <android/log.h>
#include <string>

std::vector<std::vector<double>> allZernikeMoments;
double meanZernikeMoments = {};

#define LOG_TAG "Part-One"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)


// Función para preprocesar la imagen y extraer los píxeles no cero
void preprocess_image(const cv::Mat& img, std::vector<double>& X, std::vector<double>& Y, std::vector<double>& P, double& psum, double& moment10,
                      double& moment00, double& moment01) {
    int rows = img.rows;
    int cols = img.cols;

    X.clear();
    Y.clear();
    P.clear();
    psum = 0.0;
    moment10 = 0.0;
    moment00 = 0.0;
    moment01 = 0.0;

    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            double intensity = img.at<uchar>(y, x);
            if (intensity != 0) {
                Y.push_back(y + 1);
                X.push_back(x + 1);
                P.push_back(intensity);
                psum += intensity;
                moment10 += (x + 1) * intensity;
                moment00 += intensity;
                moment01 += (y + 1) * intensity;
            }
        }
    }
}


extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_tarea_viewCamara_momentsZernike(JNIEnv *env, jobject,jobject mat) {
    cv::Mat imgGray, mask;
    cv::Mat& zernike = *(cv::Mat*)mat;
    cv::Mat zernikeTest = zernike;
    cv::cvtColor(zernikeTest, zernikeTest, cv::IMREAD_GRAYSCALE);
    std::vector<double> X, Y, P;
    double psum, moment10, moment00, moment01;
    preprocess_image(zernikeTest, X, Y, P, psum, moment10, moment00, moment01);
    double D = 15;
    double R = std::min(zernikeTest.rows, zernikeTest.cols) / 2.0;
    double m10_m00 = moment10 / moment00;
    double m01_m00 = moment01 / moment00;
    std::vector<double> zvalues;
    mb_Znl(X, Y, P, D, m10_m00, m01_m00, R, psum, zvalues);
    std::vector<double> first7ZernikeMoments(zvalues.begin(), zvalues.begin() + 12);
    for (size_t i = 0; i < first7ZernikeMoments.size(); i++) {
        LOGI("Zernike Moments %f", first7ZernikeMoments[i]);
    }

    cv::String ad = "Zernike 1:(" + std::to_string(first7ZernikeMoments[0]) + ")\n"
                + "Zernike 2:(" + std::to_string(first7ZernikeMoments[1]) + ")\n"
                + "Zernike 3:(" + std::to_string(first7ZernikeMoments[2]) + ")\n"
                + "Zernike 4:(" + std::to_string(first7ZernikeMoments[3]) + ")\n"
                + "Zernike 5:(" + std::to_string(first7ZernikeMoments[4]) + ")\n"
                + "Zernike 6:(" + std::to_string(first7ZernikeMoments[5]) + ")\n"
                + "Zernike 7:(" + std::to_string(first7ZernikeMoments[6]) + ")\n"
                + "Zernike 8:(" + std::to_string(first7ZernikeMoments[7]) + ")\n"
                + "Zernike 9:(" + std::to_string(first7ZernikeMoments[8]) + ")\n"
                + "Zernike 10:(" + std::to_string(first7ZernikeMoments[9]) + ")\n"
                + "Zernike 11:(" + std::to_string(first7ZernikeMoments[10]) + ")\n"
                + "Zernike 12:(" + std::to_string(first7ZernikeMoments[11]) + ")\n";

    return env->NewStringUTF(ad.c_str());
}
