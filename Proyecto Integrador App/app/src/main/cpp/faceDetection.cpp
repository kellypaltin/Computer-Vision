#include <jni.h>
#include <string>
#include <opencv2/ml/ml.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <android/log.h>
#include <stdio.h>
#include <opencv2/core/types_c.h>
#include <opencv2/core/core_c.h>

using namespace cv;
using namespace std;

CascadeClassifier faceCascade, eyeCascade, noseCascade, mouthCascade;




void Mouth(Mat& img, Rect face) {
    vector<Rect> mouths;
    Mat faceROI = img(Rect(face.x, face.y + face.height * 2 / 3, face.width, face.height / 3));

    double scaleFactor = 1.1;
    int minNeighbors = 5;
    int flags = 0;
    Size minSize(20, 10);
    Size maxSize;

    mouthCascade.detectMultiScale(faceROI, mouths, scaleFactor, minNeighbors, flags, minSize, maxSize);

    for (size_t i = 0; i < mouths.size(); i++) {
        Rect mouth = mouths[i];
        rectangle(img, Point(face.x + mouth.x, face.y + mouth.y + face.height * 2 / 3),
                  Point(face.x + mouth.x + mouth.width, face.y + mouth.y + mouth.height + face.height * 2 / 3),
                  Scalar(0, 255, 0), 2);
    }
}

void Nose(Mat& img, Rect face)
{
    vector<Rect> noses;
    Mat gray;

    cvtColor(img, gray, COLOR_BGR2GRAY);
    equalizeHist(gray, gray);

    Mat faceROI = gray(Rect(face.x + face.width / 4, face.y + face.height / 4, face.width / 2, face.height / 2));

    noseCascade.detectMultiScale(faceROI, noses, 1.1, 5, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

    if (!noses.empty()) {
        Rect nose = noses[0];

        int x = nose.x + face.x + face.width / 4;
        int y = nose.y + face.y + face.height / 4;
        int width = nose.width;
        int height = nose.height;

        rectangle(img, Point(x, y), Point(x + width, y + height), Scalar(255, 0, 0), 2);
    }
}

void Eyes(Mat& img, Rect face)
{
    vector<Rect> e;
    Mat faceEyes = img(Rect(face.x + face.width / 8, face.y, face.width * 3 / 4, face.height / 2));
    eyeCascade.detectMultiScale(faceEyes, e, 1.1, 5, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

    if (e.size() == 2)
    {
        Rect eye1 = e[0];
        Rect eye2 = e[1];

        int x = min(eye1.x, eye2.x) + face.x + face.width / 8;
        int y = min(eye1.y, eye2.y) + face.y;
        int width = max(eye1.x + eye1.width, eye2.x + eye2.width) - min(eye1.x, eye2.x)+10;
        int height = max(eye1.y + eye1.height, eye2.y + eye2.height) - min(eye1.y, eye2.y)+10;

        rectangle(img, Point(x, y), Point(x + width, y + height), Scalar(135, 0, 116), 6);
    }
}


void FacialFeatures(Mat img) {
    vector<Rect> faces;
    Mat imgGris;
    cvtColor(img, imgGris, COLOR_BGR2GRAY);
    equalizeHist(imgGris, imgGris);
    faceCascade.detectMultiScale(imgGris, faces, 1.1, 5, 0 | CASCADE_SCALE_IMAGE, Size(50, 50));
    for (size_t i = 0; i < faces.size(); i++)
    {
        Rect face = faces[i];
        rectangle(img, Point(face.x, face.y), Point(face.x + face.width, face.y + face.height), Scalar(0, 0, 255), 2);
        Eyes(img,face);
        Mouth(img,face);
        Nose(img,face);
    }
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_example_project_CameraActivity_camprocess(JNIEnv* env, jobject, jlong mat) {
    Mat& inputMat = *(Mat*)mat;
    Mat processedMat = inputMat.clone();
    cvtColor(processedMat, processedMat, COLOR_BGR2RGB);
    rotate(processedMat, processedMat, ROTATE_90_CLOCKWISE);
    return (jlong)(new Mat(processedMat));
}

extern "C"
JNIEXPORT jlong  __attribute__((unused))
JNICALL
Java_com_example_project_CameraActivity_processFace(JNIEnv *env, jobject thiz, jlong mat) {
    Mat& inputMat = *(Mat*)mat;

    FacialFeatures(inputMat);
    rotate(inputMat,inputMat,ROTATE_90_CLOCKWISE);
    flip(inputMat, inputMat, 1);

    return (jlong) &inputMat;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_project_FaceDetector_nativeInitCascades(JNIEnv *env, jobject thiz,
                                                         jstring faceCascadePath,
                                                         jstring eyeCascadePath,
                                                         jstring noseCascadePath,
                                                         jstring mouthCascadePath)
{
    const char* faceCascadePathStr = env->GetStringUTFChars(faceCascadePath, nullptr);
    const char* eyeCascadePathStr = env->GetStringUTFChars(eyeCascadePath, nullptr);
    const char* noseCascadePathStr = env->GetStringUTFChars(noseCascadePath, nullptr);
    const char* mouthCascadePathStr = env->GetStringUTFChars(mouthCascadePath, nullptr);

    faceCascade.load(faceCascadePathStr);
    eyeCascade.load(eyeCascadePathStr);
    noseCascade.load(noseCascadePathStr);
    mouthCascade.load(mouthCascadePathStr);

    env->ReleaseStringUTFChars(faceCascadePath, faceCascadePathStr);
    env->ReleaseStringUTFChars(eyeCascadePath, eyeCascadePathStr);
    env->ReleaseStringUTFChars(noseCascadePath, noseCascadePathStr);
    env->ReleaseStringUTFChars(mouthCascadePath, mouthCascadePathStr);
}
