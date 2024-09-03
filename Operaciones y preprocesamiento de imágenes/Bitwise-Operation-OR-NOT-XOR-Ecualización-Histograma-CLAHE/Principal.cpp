
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

int main(int argc, char* args[]){

    // Manejo de Video
    VideoCapture video(0);

    // Verificamos si la cámara se pudo abrir
    if (video.isOpened()){
        namedWindow("Video", WINDOW_AUTOSIZE);
        namedWindow("Video-Color", WINDOW_AUTOSIZE);
        namedWindow("Mascara",WINDOW_AUTOSIZE);
        namedWindow("NOT",WINDOW_AUTOSIZE);
        namedWindow("AND",WINDOW_AUTOSIZE);
        namedWindow("San-Basilio",WINDOW_AUTOSIZE);
        namedWindow("Weighted-Sum",WINDOW_AUTOSIZE);

        Mat frame;
        Mat gris;
        Mat imgNOT;
        Mat imgAND;
        Mat sanBasilio = imread("San-Basilio.jpg");
        Mat sumaPonderada;
        bool primera = true;

        while(3==3){
            video >> frame;
            flip(frame, frame, 1);

            // El método resize permite cambiar el tamaño de una imagen
            resize(frame, frame, Size(), 0.8, 0.8);

            if (primera){
                primera = false;
                resize(sanBasilio, sanBasilio, frame.size());
            }

            cvtColor(frame, gris, COLOR_BGR2GRAY);

            // Weighted sum (suma ponderada):
            // Restulado(x,y) = alfa*Img1(x,y) + beta*Img2(x,y)
            // donde: alfa + beta = 1.0

            addWeighted(frame, 0.8, sanBasilio, 0.2, 0, sumaPonderada);

            bitwise_not(gris, imgNOT);
            bitwise_and(frame, sanBasilio, imgAND);
            //negada = cv2.bitwise_not(gris)
            
            imshow("Video", gris);
            imshow("Mascara", binaria);
            imshow("NOT", imgNOT);
            imshow("AND", imgAND);
            imshow("San-Basilio", sanBasilio);
            imshow("Weighted-Sum", sumaPonderada);

            if(waitKey(23)==27)
                break;
        }
        video.release();
        destroyAllWindows();
    }

    namedWindow("Oscura", WINDOW_AUTOSIZE);
    namedWindow("Ecualizada", WINDOW_AUTOSIZE);
    namedWindow("CLAHE", WINDOW_AUTOSIZE);

    Mat imgOscura = imread("Imagen-Obscura-[12].png", IMREAD_GRAYSCALE);
    Mat imgEcualizada;
    Mat imgCLAHE;

    equalizeHist(imgOscura, imgEcualizada);

    Ptr<CLAHE> clahe = createCLAHE();
    clahe->apply(imgOscura, imgCLAHE);
    clahe->setTilesGridSize(Size(24,24));

    imwrite("Ecualizada.png", imgEcualizada);
    imwrite("CLAHE.png", imgCLAHE);

    imshow("Oscura", imgOscura);
    imshow("Ecualizada", imgEcualizada);
    imshow("CLAHE", imgCLAHE);

    waitKey(0);
    destroyAllWindows();

    return 0;z
}