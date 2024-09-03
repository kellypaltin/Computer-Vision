
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <random>
#include <fstream>
#include <sstream>
#include <cstring>

// Librerías de OpenCV
//#include <opencv2/opencv.hpp>

#include <opencv2/imgproc.hpp> // Librería que contiene operaciones para realizar procesamiento de imágenes
#include <opencv2/imgcodecs.hpp> // Librería contiene los métodos para cargar imágenes de acuerdo a un códec
#include <opencv2/core.hpp> // Librería que contiene las estructuras y funciones base para representar las imágenes como matrices
#include <opencv2/video.hpp> // Librería para realizar lectura de vídeos
#include <opencv2/videoio.hpp> // Librería para escritura de vídoeos y streaming
#include <opencv2/highgui.hpp> // Librería para crear interfaces básicas de usuario


using namespace std;
using namespace cv;

int umbral = 0;
int betaEntero = 0;
double betaDecimal = 0;
int k = 3, kImpar = 3; // Tamaño del filtro de la mediana

void funcionTrack(int valor, void *data){

}

void funcionBeta(int valor, void *data){
    betaDecimal = (double) betaEntero;
    betaDecimal/=10;
}

void funcionK(int valor, void *data){
    if(k%2!=0){
        kImpar = k;
    }
}

int sigmoideo(double pixel, double beta, double m){
    // Fórmula:
    // s(pixel, beta, m) = [1/(1+exp(-beta*(pixel-m)))]*255
    double res = 255.0/(1+exp(-beta*(pixel-m)));
    return (int) res;
}

int main(int argc, char* args[]){

    // Manejo de Video
    VideoCapture video(0);

    // Verificamos si la cámara se pudo abrir
    if (video.isOpened()){
        namedWindow("Video", WINDOW_AUTOSIZE);
        namedWindow("Movimiento",WINDOW_AUTOSIZE);
        namedWindow("Threshold",WINDOW_AUTOSIZE);
        namedWindow("ContrastS",WINDOW_AUTOSIZE);
        namedWindow("imgMediana",WINDOW_AUTOSIZE);

        createTrackbar("Umbral", "Video", &umbral, 255, funcionTrack, NULL);
        createTrackbar("Beta", "Video", &betaEntero, 20, funcionBeta, NULL);
        createTrackbar("K", "Video", &k, 25, funcionK, NULL);
        setTrackbarMin("K", "Video", 1);

        Mat frame;
        Mat anterior;
        Mat resta;
        Mat movimiento;
        Mat imgThreshold;
        Mat imgContrast;
        Mat imgMediana;
        int pixel = 0;

        while(3==3){
            video >> frame;
            flip(frame, frame, 1);
            cvtColor(frame, frame, COLOR_BGR2GRAY);
            resize(frame, frame, Size(), 0.8, 0.8);

            if (anterior.empty()){
                anterior = frame.clone();
                cout << "Inicializa el primer frame ..." << endl;
                imgContrast = Mat::zeros(frame.size(), CV_8UC1);
            }

            threshold(frame, imgThreshold, umbral, 255, THRESH_BINARY);

            // Operación que realiza absdiff()
            // resta = |frame - anterior|
            absdiff(frame, anterior, resta);
            anterior = frame.clone();

            threshold(resta, resta, 10, 255, THRESH_BINARY);
            
            for(int i=0;i<frame.rows;i++){
                for(int j=0;j<frame.cols;j++){
                    pixel = frame.at<uchar>(i,j);
                    imgContrast.at<uchar>(i,j) = sigmoideo(pixel, betaDecimal, umbral);
                }
            }

            medianBlur(frame, imgMediana, kImpar);

            imshow("Video", frame);
            imshow("Movimiento", resta);
            imshow("Threshold", imgThreshold);
            imshow("ContrastS", imgContrast);
            imshow("imgMediana", imgMediana);

            if(waitKey(23)==27)
                break;
        }
        video.release();
        destroyAllWindows();
    }

    return 0;
}