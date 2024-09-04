#include <jni.h>
#include <string>
#include <opencv2/ml/ml.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <android/log.h>


using namespace cv;
int lowThreshold = 50, highThreshold = 150;
int hmin = 12, hmax = 37, smin = 71, smax = 255, vmin = 80, vmax = 255;

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_project_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT jlong JNICALL
    Java_com_example_project_CameraActivity_camara(JNIEnv* env, jobject, jlong mat) {
    Mat& inputMat = *(Mat*)mat;
    cv::cvtColor(inputMat, inputMat, cv::COLOR_BGR2RGB);
    cv::medianBlur(inputMat,inputMat,3);
    rotate(inputMat, inputMat, ROTATE_90_CLOCKWISE);
    return (jlong)(new Mat(inputMat));
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_example_project_CameraActivity_filterOne(JNIEnv* env, jobject, jlong MatCa) {
    Mat& inputMat = *(Mat*)MatCa;
    Mat img,outputMat,gray,blur,edges,tem,imR;
    img = inputMat.clone();
    imR = inputMat.clone();

    for (int i = 0; i < 2; ++i) {
        pyrDown(img, img);
    }
    for (int i = 0; i < 7; ++i) {
        bilateralFilter(img,tem, 9, 1, 0.01);
    }
    for (int i = 0; i < 2; ++i) {
        pyrUp(tem, tem);
    }
    cvtColor(imR, gray, COLOR_BGR2GRAY);
    medianBlur(gray,blur,5);
    adaptiveThreshold(blur, edges, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 9, 2);
    cvtColor(edges,edges, COLOR_BGR2RGB);
    bitwise_and(tem,edges,outputMat);
    return (jlong)(new Mat(outputMat));
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_example_project_CameraActivity_filterTwo(JNIEnv* env, jobject, jlong mat) {
    Mat& inputMat = *(Mat*)mat;
    Mat grayMat, outputMat;
    cvtColor(inputMat, grayMat, COLOR_BGR2GRAY);
    threshold(grayMat, grayMat, 128, 255, THRESH_BINARY);
    Canny(grayMat, outputMat, lowThreshold, highThreshold);
    return (jlong)(new Mat(outputMat));
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_example_project_CameraActivity_filterThree(JNIEnv* env, jobject, jlong mat) {
    Mat& inputMat = *(Mat*)mat;
    Mat outputMat;
    Laplacian(inputMat, outputMat, CV_16S, 3);
    return (jlong)(new Mat(outputMat));
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_example_project_CameraActivity_filterFour(JNIEnv* env, jobject, jlong matAddr) {
    Mat& inputMat = *(Mat*)matAddr;
    Mat outputMat;
    cvtColor(inputMat, outputMat, COLOR_BGR2HSV);
    return (jlong)(new Mat(outputMat));
}
