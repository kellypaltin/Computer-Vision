
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

int hmin = 12, hmax = 37, smin = 71, smax = 255, vmin = 80, vmax = 255;

void eventoTrack(int valor, void *data){
    //cout << "Valor: " << valor << endl;
    //cout << "hmin: " << valor << endl;
}


int main(int argc, char* args[]){

    // Manejo de Video
    VideoCapture video(0);

    // Verificamos si la cámara se pudo abrir
    if (video.isOpened()){
        namedWindow("Binaria", WINDOW_AUTOSIZE);
        namedWindow("Total",WINDOW_AUTOSIZE);
        namedWindow("AND",WINDOW_AUTOSIZE);

        createTrackbar("HMin", "Total", &hmin, 180, eventoTrack, NULL);
        createTrackbar("SMin", "Total", &smin, 255, eventoTrack, NULL);
        createTrackbar("VMin", "Total", &vmin, 255, eventoTrack, NULL);

        createTrackbar("HMax", "Total", &hmax, 180, eventoTrack, NULL);
        createTrackbar("SMax", "Total", &smax, 255, eventoTrack, NULL);
        createTrackbar("VMax", "Total", &vmax, 255, eventoTrack, NULL);

        Mat frame;
        Mat imgHSV;
        Mat imgLab;
        Mat imgTotal;
        Mat binaria;
        Mat imgAND;

        while(3==3){
            video >> frame;
            flip(frame, frame, 1);

            // El método resize permite cambiar el tamaño de una imagen
            resize(frame, frame, Size(), 0.8, 0.8);

            if(imgTotal.empty()){
                imgTotal = Mat::zeros(Size(frame.cols*3, frame.rows), CV_8UC3);
            }

            cvtColor(frame, imgHSV, COLOR_BGR2HSV);
            cvtColor(frame, imgLab, COLOR_BGR2Lab);

            frame.copyTo(imgTotal(Rect(0,0,frame.cols, frame.rows)));
            imgLab.copyTo(imgTotal(Rect(imgLab.cols, 0, imgLab.cols, imgLab.rows)));
            imgHSV.copyTo(imgTotal(Rect(imgHSV.cols*2, 0, imgHSV.cols, imgHSV.rows)));

            inRange(imgHSV, Scalar(hmin, smin, vmin), Scalar(hmax, smax, vmax), binaria);

            imgAND = Mat::zeros(frame.size(), CV_8UC3);

            bitwise_and(frame, frame, imgAND, binaria);
            
            imshow("Total", imgTotal);
            imshow("Binaria", binaria);
            imshow("AND", imgAND);

            if(waitKey(23)==27)
                break;
        }
        video.release();
        destroyAllWindows();
    }

    return 0;
}