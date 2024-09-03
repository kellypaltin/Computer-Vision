
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

int betaEntero = 0;
double betaDecimal = 0;
int k = 3, kImpar = 3; // Tamaño del filtro de la mediana

void funcionBeta(int valor, void *data){
    betaDecimal = (double) betaEntero;
    betaDecimal/=10;
}

void funcionK(int valor, void *data){
    if(k%2!=0){
        kImpar = k;
    }
}


int main(int argc, char* args[]){

    // Manejo de Video
    VideoCapture video(0);

    // Verificamos si la cámara se pudo abrir
    if (video.isOpened()){
        namedWindow("Video", WINDOW_AUTOSIZE);
        namedWindow("imgMediana",WINDOW_AUTOSIZE);
        namedWindow("imgGauss",WINDOW_AUTOSIZE);
        namedWindow("gX",WINDOW_AUTOSIZE);
        namedWindow("gY",WINDOW_AUTOSIZE);

        createTrackbar("Sigma", "Video", &betaEntero, 20, funcionBeta, NULL);
        createTrackbar("K", "Video", &k, 25, funcionK, NULL);
        setTrackbarMin("K", "Video", 1);

        Mat frame;
        Mat imgMediana;
        Mat imgGauss;
        Mat gX;
        Mat gY;
        Mat gXAbs;
        Mat gYAbs;
        Mat bordes;
        Mat imgLaplace;
        Mat canny;


        while(3==3){
            video >> frame;
            flip(frame, frame, 1);
            cvtColor(frame, frame, COLOR_BGR2GRAY);
            resize(frame, frame, Size(), 0.8, 0.8);

            medianBlur(frame, imgMediana, kImpar);
            GaussianBlur(frame, imgGauss, Size(kImpar, kImpar), betaDecimal, betaDecimal);

            Sobel(frame, gX, CV_16S, 1, 0, 1);
            Sobel(frame, gY, CV_16S, 0, 1, 1);

            Laplacian(frame, imgLaplace, CV_16S, kImpar);
            
            convertScaleAbs(imgLaplace, imgLaplace);

            convertScaleAbs(gX, gXAbs);
            convertScaleAbs(gY, gYAbs);

            addWeighted(gXAbs, 0.5, gYAbs, 0.5, 0, bordes);

            Canny(frame, canny, 100, 100*3);

            imshow("Video", frame);
            imshow("imgMediana", imgMediana);
            imshow("gX", gXAbs);
            imshow("gY", gYAbs);
            imshow("Sobel", bordes);
            imshow("Laplace", imgLaplace);
            imshow("Canny", canny);
            

            if(waitKey(23)==27)
                break;
        }
        video.release();
        destroyAllWindows();
    }


    Mat imgJ = imread("Imagen-J.png", IMREAD_GRAYSCALE);
    resize(imgJ, imgJ, Size(), 1.5, 1.5);
    Mat elemento = getStructuringElement(MORPH_CROSS , Size(7,7));
    Mat imgJO; 

    morphologyEx(imgJ, imgJO, MORPH_OPEN, elemento, Point(-1,-1), 1);

    imshow("Original", imgJ);
    imshow("Open", imgJO);

    waitKey(0);
    destroyAllWindows();

    return 0;
}