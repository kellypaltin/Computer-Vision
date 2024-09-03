
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
    VideoCapture video("http://pendelcam.kip.uni-heidelberg.de/mjpg/video.mjpg");

    // Verificamos si la cámara se pudo abrir
    if (video.isOpened()){
        namedWindow("Video",WINDOW_FREERATIO);

        Mat frame;

        while(3==3){
            video >> frame;

            // El método resize permite cambiar el tamaño de una imagen
            resize(frame, frame, Size(), 0.8, 0.8);

            imshow("Video", frame);

            if(waitKey(23)==27)
                break;
        }
        video.release();
        destroyAllWindows();
    }else{
        cout << "No se puede abrir ..." << endl;
    }

    return 0;
}