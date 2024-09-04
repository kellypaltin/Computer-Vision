#include <opencv2/opencv.hpp>
#include "LBP.h"
#include <android/log.h>
#include <iostream>
#include <fstream>  // Asegúrate de incluir esta biblioteca
#include <jni.h>
#include <string>
using namespace std;
using namespace cv;

#define LOG_TAG "Part-Two"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

std::vector<vector<int>> clases;

Mat resultHistogram;
Mat resultLBP;

void saveLBPHistogram(vector<int> histo, const string& filename) {

    std::ofstream archivo(filename);
    if (archivo.is_open()) {
        for (int val : histo) {
            archivo << val << " ";
        }
        archivo.close();
    }
}

vector<int> averageHistograms(vector<string> files) {
    vector<int> avgHistogram(256, 0);
    int numFiles = files.size();
    LOGI("El tamaño del files es: %lu", files.size());

    for (const string& file : files) {
        ifstream infile(file);
        int val;
        for (int i = 0; i < 256; i++) {
            infile >> val;
            avgHistogram[i] += val;
        }
        infile.close();
    }
    for (int i = 0; i < 256; i++) {
        avgHistogram[i] /= numFiles;
    }
    return avgHistogram;
}

void matHistograma(const vector<int>& histo) {
    int histSize = 256;
    int hist_w = 512; int hist_h = 400;
    int bin_w = cvRound((double)hist_w / histSize);
    Mat histImage(hist_h, hist_w, CV_8UC1, Scalar(0, 0, 0));

    for (int i = 1; i < histSize; i++) {
        line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(histo[i - 1])),
             Point(bin_w*(i), hist_h - cvRound(histo[i])),
             Scalar(255, 255, 255), 2, 8, 0);
    }

    resultHistogram = histImage;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_tarea_MainActivity_processImg(JNIEnv *env, jobject, jstring ruta) {
    LBP* lbp = new LBP();
    Mat inputMat, imgGray, mask;

    const char *ph = env->GetStringUTFChars(ruta, nullptr);
    std::vector<std::string> imagePaths;
    std::vector<std::string> imgHisto;

    String sd = std::string(ph) + "/*.jpg";
    cv::glob(sd, imagePaths, false);
    for (const auto& path : imagePaths) {
        Mat img = imread(path);
        cv::cvtColor(img, img, COLOR_BGR2GRAY);

        vector<int> histo = lbp->calcularLBP(img);
        saveLBPHistogram(histo, path + "_histogram.txt");
    }

    String his = std::string(ph) + "/*.txt";
    cv::glob(his, imgHisto, false);
    // Promediar histogramas de entrenamiento
    vector<int> resultHistory = averageHistograms(imgHisto);
    clases.push_back(resultHistory);


    LOGI("El tamaño de clases es: %lu", clases.size());
    return env->NewStringUTF("Procesamiento de imagen exitoso.");
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_tarea_viewCamara_ClassificationMaterial(JNIEnv* env, jobject, jobject mat) {
    LBP* lbp = new LBP();
    Mat imgGray, mask;
    Mat& textura = *(Mat*)mat;
    Mat tex = textura;

    std::string name;

    // Convert input image to grayscale
    cv::cvtColor(tex, tex, COLOR_BGR2GRAY);

    // Calculate LBP image and histogram
    Mat lbpImage = lbp->calcularLBPImage(tex);
    vector<int> textureHistogram = lbp->calcularLBP(tex);

    resultLBP = lbpImage;

    // Generate histogram image
    matHistograma(textureHistogram);

    // Check if classes vector has at least two classes
    if (clases.size() < 2) {
        LOGI("Not enough classes for comparison");
        return env->NewStringUTF("Not enough classes for comparison");
    }

    // Compare histograms and classify
    double distanciaClass1 = norm(clases[0], textureHistogram, NORM_L2);
    double distanciaClass2 = norm(clases[1], textureHistogram, NORM_L2);
    LOGI("Distancia a Clase 1 (Madera): %f", distanciaClass1);
    LOGI("Distancia a Clase 2 (Tela de ropa): %f", distanciaClass2);

    if (distanciaClass1 < distanciaClass2) {
        name = "Clase 1 (Madera)";
    } else {
        name = "Clase 2 (Tela)";
    }

    return env->NewStringUTF(name.c_str());
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_example_tarea_viewCamara_matLBP(JNIEnv* env, jobject) {
    return (jlong)(new Mat(resultLBP));
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_example_tarea_viewCamara_matHistogram(JNIEnv* env, jobject) {
    return (jlong)(new Mat(resultHistogram));
}


