#include <opencv2/opencv.hpp>
#include <filesystem>
#include <iostream>
#include <string>
#include <jni.h>

using namespace cv;
using namespace std;

vector<float> computeHOG(const Mat &image) {
    vector<float> features;
    Size newSize(28, 28);
    resize(image, image, newSize);

    HOGDescriptor hog(
            Size(28, 28),
            Size(14, 14),
            Size(7, 7),
            Size(14, 14),
            9);

    hog.compute(image, features);

    return features;
}


extern "C" JNIEXPORT jlong JNICALL
Java_com_example_project_CameraActivity_processHog(JNIEnv* env, jobject, jlong mat) {
    Mat& inputMat = *(Mat*)mat;
    Mat processedMat = inputMat.clone();
    cvtColor(processedMat, processedMat, COLOR_BGR2RGB);
    resize(processedMat, processedMat, Size(28,28));
    rotate(processedMat, processedMat, ROTATE_90_CLOCKWISE);
    Mat gray;

    cvtColor(processedMat, gray, COLOR_RGB2GRAY);
    threshold(gray, gray, 127, 255, THRESH_BINARY_INV);
    return (jlong)(new Mat(gray));
}

extern "C" JNIEXPORT jfloatArray JNICALL
Java_com_example_project_CameraActivity_computeHOG(JNIEnv *env, jobject, jlong matAddr) {
    Mat &image = *(Mat *)matAddr;
    vector<float> features = computeHOG(image);

    jfloatArray result = env->NewFloatArray(features.size());
    if (result == nullptr) {
        return nullptr;
    }

    env->SetFloatArrayRegion(result, 0, features.size(), features.data());
    return result;
}