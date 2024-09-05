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

int hmin = 0, hmax = 130, smin = 28, smax = 197, vmin = 80, vmax = 255;

double momentosHuBaseCirculo[7] = {0.159482, 4.68371e-05, 2.49876e-06, 1.22589e-10, -2.14555e-18, 3.03404e-13, -7.98216e-21};
double momentosHuBaseEstrella[7] = {0.213549, 0.00013518, 8.70805e-05, 2.16844e-06, -1.55161e-11, 2.00708e-08, -2.54392e-11};

void eventoTrack(int valor, void *data){
    //cout << "Valor: " << valor << endl;
    //cout << "hmin: " << valor << endl;
}

double distanciaEuclidea(double momentosHu[7], double momentosHuBase[7]){
    double suma = 0;
    for (int i = 0; i < 7; i++){
        suma += ((momentosHuBase[i] - momentosHu[i]) * (momentosHuBase[i] - momentosHu[i]));
    }
    return sqrt(suma);
}

int main(int argc, char* args[]){

    // Manejo de Video
    VideoCapture video(0);

    // Verificamos si la cámara se pudo abrir
    if (video.isOpened()){
        namedWindow("Binaria", WINDOW_AUTOSIZE);
        namedWindow("Total", WINDOW_AUTOSIZE);
        namedWindow("AND", WINDOW_AUTOSIZE);

        createTrackbar("HMin", "Total", &hmin, 180, eventoTrack, NULL);
        createTrackbar("SMin", "Total", &smin, 255, eventoTrack, NULL);
        createTrackbar("VMin", "Total", &vmin, 255, eventoTrack, NULL);

        createTrackbar("HMax", "Total", &hmax, 180, eventoTrack, NULL);
        createTrackbar("SMax", "Total", &smax, 255, eventoTrack, NULL);
        createTrackbar("VMax", "Total", &vmax, 255, eventoTrack, NULL);

        Mat frame;
        Mat imgHSV;
        Mat imgTotal;
        Mat binaria;
        Mat binariaColor;
        double Cx = 0, Cy = 0;
        double distanciaCirculo = 0;
        double distanciaEstrella = 0;

        Moments momentos;
        double momentosHu[7];

        while (true){
            video >> frame;
            flip(frame, frame, 1);

            // El método resize permite cambiar el tamaño de una imagen
            resize(frame, frame, Size(), 0.8, 0.8);

            if (imgTotal.empty()){
                imgTotal = Mat::zeros(Size(frame.cols * 2, frame.rows), CV_8UC3);
            }

            cvtColor(frame, imgHSV, COLOR_BGR2HSV);

            inRange(imgHSV, Scalar(hmin, smin, vmin), Scalar(hmax, smax, vmax), binaria);

            // Calculamos los momentos RAW
            momentos = moments(binaria, true);
            Cx = momentos.m10 / momentos.m00;
            Cy = momentos.m01 / momentos.m00;

            cout << "Cx: " << Cx << " Cy: " << Cy << " área: " << momentos.m00 << endl;

            if (momentos.m00 > 10000){
                HuMoments(momentos, momentosHu);

                distanciaCirculo = distanciaEuclidea(momentosHu, momentosHuBaseCirculo);
                distanciaEstrella = distanciaEuclidea(momentosHu, momentosHuBaseEstrella);

                cout << "Distancia Circulo: " << distanciaCirculo << endl;
                cout << "Distancia Estrella: " << distanciaEstrella << endl;

                if (distanciaCirculo < 0.001){
                    putText(frame, "Circulo", Point(Cx, Cy), FONT_HERSHEY_SIMPLEX, 1, Scalar(233, 1, 1), 2);
                } else if (distanciaEstrella < 0.005){
                    putText(frame, "Estrella", Point(Cx, Cy), FONT_HERSHEY_SIMPLEX, 1, Scalar(1, 233, 1), 2);
                }

                for (int i = 0; i < 7; i++){
                    cout << momentosHu[i] << ",";
                }
                cout << endl;
            }

            frame.copyTo(imgTotal(Rect(0, 0, frame.cols, frame.rows)));
            cvtColor(binaria, binariaColor, COLOR_GRAY2BGR);
            binariaColor.copyTo(imgTotal(Rect(binariaColor.cols, 0, binariaColor.cols, binariaColor.rows)));

            imshow("Total", imgTotal);

            if (waitKey(23) == 27)
                break;
        }
        video.release();
        destroyAllWindows();
    }

    return 0;
}
