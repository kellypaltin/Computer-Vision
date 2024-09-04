#include <jni.h>
#include <string>
#include <opencv2/ml/ml.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <android/log.h>
#include <vector>
#include <opencv2/core/utils/filesystem.hpp>
#include <android/log.h>
#include "LBP.h"

using namespace cv;

#define LOG_TAG "NativeLib"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

struct HuMoment {
    double hu[7];
};

struct Resultado {
    std::string nombre;
    double distMedia{};
};


std::vector<HuMoment> huMomentsList;
std::vector<Resultado> resultados;
std::vector<HuMoment> huMomentsMean;
Mat maskInverted;

HuMoment meanHuMoments = {};
double mome;
double momentResult[7];

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_tarea_MainActivity_stringFromJNI(JNIEnv* env, jobject /* this */) {
    std::string hello = "Practica";
    return env->NewStringUTF(hello.c_str());
}

// Calcular la distancia Euclídea
double distanciaEuclidea(const double momentosHu[7], const double meanHu[7]) {
    double suma = 0;
    for (int i = 0; i < 7; i++) {
        suma += ((meanHu[i] - momentosHu[i]) * (meanHu[i] - momentosHu[i]));
    }
    return sqrt(suma);
}

extern "C" JNIEXPORT jdouble JNICALL
Java_com_example_tarea_MainActivity_processCorpus(JNIEnv* env, jobject, jstring ruta) {
    Mat inputMat, imgGray, mask;
    double distMedia = 0;
    const char *ph = env->GetStringUTFChars(ruta, nullptr);
    std::vector<std::string> imagePaths;
    String sd = std::string(ph) + "/*.PNG";
    cv::glob(sd, imagePaths, false);
    for (const auto& path : imagePaths) {
        Mat img = imread(path);
        resize(img, img, Size(200, 200));
        cvtColor(img, imgGray, COLOR_BGR2GRAY);
        threshold(imgGray, mask, 200, 255, THRESH_BINARY);
        bitwise_not(mask, maskInverted);
        Moments momentos = moments(maskInverted, true);
        double momentosHu[7];
        HuMoments(momentos, momentosHu);
        huMomentsList.push_back({momentosHu[0], momentosHu[1], momentosHu[2], momentosHu[3],
                                 momentosHu[4], momentosHu[5], momentosHu[6]});
        mome += momentos.m00;
    }
    for (const auto& hu : huMomentsList) {
        for (int i = 0; i < 7; ++i) {
            meanHuMoments.hu[i] += hu.hu[i];}
    }
    for (double & i : meanHuMoments.hu) {
        i /= huMomentsList.size();}
    double distanciaEuclidiaTotal = 0;
    for (const auto& hu : huMomentsList) {
        distanciaEuclidiaTotal += distanciaEuclidea(hu.hu, meanHuMoments.hu);}
    distMedia = distanciaEuclidiaTotal / huMomentsList.size();
    std::string fullPath(ph);
    size_t pos = fullPath.find_last_of('/');
    std::string nombre = (pos == std::string::npos) ? fullPath : fullPath.substr(pos + 1);
    huMomentsMean.push_back(meanHuMoments);
    resultados.push_back({nombre, distMedia});
    huMomentsList.clear();
    meanHuMoments = {};
    return (jdouble)distMedia;
}

// Función JNI para obtener y registrar todos los resultados almacenados
extern "C" JNIEXPORT void JNICALL
Java_com_example_tarea_MainActivity_logResults(JNIEnv* env, jobject /* this */) {
    LOGI("Registrando todos los resultados:");
    for (const auto& resultado : resultados) {
        LOGI("Resultado para: %s", resultado.nombre.c_str());
        LOGI("Distancia Euclidea Promedio: %f", resultado.distMedia);
        LOGI("------------------------");
    }

    for (const auto& result : huMomentsMean) {
        LOGI("Momentos de hu media:");
        for (int i = 0; i < 7; ++i) {
            LOGI("Momento hu %d: %f", i, result.hu[i]);
        }
        LOGI("------------------------");
    }
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_tarea_viewCamara_Classification(JNIEnv* env, jobject, jobject mat) {
    Mat imgGray, mask;
    Mat& inputMat = *(Mat*)mat;

    resize(inputMat, inputMat, Size(200, 200));
    rotate(inputMat, inputMat, ROTATE_90_CLOCKWISE);
    cv::cvtColor(inputMat, imgGray, COLOR_BGR2GRAY);
    threshold(imgGray, mask, 120, 255, THRESH_BINARY);
    bitwise_not(mask, maskInverted);

    Moments mo = moments(maskInverted, true);
    double momet[7];
    HuMoments(mo, momet);

    for (int i = 0; i < 7; ++i) {
        momentResult[i] = momet[i];}

    std::vector<double> disEuList;
    for (const auto& huMoment : huMomentsMean) {
        double result = distanciaEuclidea(momet, huMoment.hu);
        LOGI("El resultado euclidiano es: %f", result);
        disEuList.push_back(result);}

    // Encuentra la menor diferencia
    double minDiferencia = std::numeric_limits<double>::max();
    std::string figura;
    for (size_t i = 0; i < disEuList.size(); ++i) {
        double diferencia = std::abs(disEuList[i] - resultados[i].distMedia);
        if (diferencia < minDiferencia) {
            minDiferencia = diferencia;
            figura = resultados[i].nombre;}
    }

    LOGI("La figura clasificada es: %s", figura.c_str());
    String name = figura.c_str();
    return env->NewStringUTF(figura.c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_tarea_viewCamara_momentResult(JNIEnv* env, jobject) {
    String ad = "Hu 1:(" + std::to_string(momentResult[0]) + ")\n"
                + "Hu 2:(" + std::to_string(momentResult[1]) + ")\n"
                + "Hu 3:(" + std::to_string(momentResult[2]) + ")\n"
                + "Hu 4:(" + std::to_string(momentResult[3]) + ")\n"
                + "Hu 5:(" + std::to_string(momentResult[4]) + ")\n"
                + "Hu 6:(" + std::to_string(momentResult[5]) + ")\n"
                + "Hu 7:(" + std::to_string(momentResult[6]) + ")";
    return env->NewStringUTF(ad.c_str());
}


extern "C" JNIEXPORT jlong JNICALL
Java_com_example_tarea_viewCamara_matMascara(JNIEnv* env, jobject) {
    return (jlong)(new Mat(maskInverted));
}


