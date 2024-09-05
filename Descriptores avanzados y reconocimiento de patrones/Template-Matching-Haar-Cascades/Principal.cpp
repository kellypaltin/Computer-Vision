

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
#include <opencv2/objdetect/objdetect.hpp>

//#include <opencv2/xfeatures2d.hpp>

#include <opencv2/features2d/features2d.hpp>

using namespace std;
using namespace cv;



int main(int argc, char* args[]){

    // Detección de Rostros

    Mat personas = imread("Personas2.jpg");
    Mat imgDetecciones = personas.clone();

    Mat gris;
    cvtColor(personas, gris, COLOR_BGR2GRAY);

    string archivoCascadas = "/home/vlarobbyk/aplicaciones/Librerias/opencv/opencvi/share/opencv4/haarcascades/haarcascade_frontalface_default.xml";
    CascadeClassifier clasificador(archivoCascadas);

    vector<Rect> detecciones;

    clasificador.detectMultiScale(gris, detecciones);

    cout << "Detecciones: " << detecciones.size() << endl;

    for(int i=0;i<detecciones.size();i++){
        rectangle(imgDetecciones, detecciones[i], Scalar(3,3,233), 2);
    }

    namedWindow("Personas", WINDOW_AUTOSIZE);
    namedWindow("Detecciones", WINDOW_AUTOSIZE);

    imshow("Personas", personas);
    imshow("Detecciones", imgDetecciones);

    waitKey(0);
    destroyAllWindows();

    // Template Matching

    Mat imagen = imread("Mario-Bros.jpg");
    Mat imgTemplate = imread("Template.png", IMREAD_GRAYSCALE);
    cvtColor(imagen, gris, COLOR_BGR2GRAY);

    Mat result;
    matchTemplate(gris, imgTemplate, result, TM_SQDIFF);
    normalize(result, result, 0, 1, NORM_MINMAX);

    double valorMinimo, valorMaximo;
    Point puntoMinimo, puntoMaximo;

    minMaxLoc(result, &valorMinimo, &valorMaximo, &puntoMinimo, &puntoMaximo);

    circle(imagen, puntoMinimo, 7, Scalar(3,3,233), 2);
    rectangle(imagen, Rect(puntoMinimo.x, puntoMinimo.y, imgTemplate.cols, imgTemplate.rows), Scalar(3,233,233), 2);

    namedWindow("Original", WINDOW_AUTOSIZE);
    namedWindow("TemplateB", WINDOW_AUTOSIZE);
    namedWindow("Result", WINDOW_AUTOSIZE);

    imshow("Original", imagen);
    imshow("TemplateB", imgTemplate);
    imshow("Result", result);

    waitKey(0);
    destroyAllWindows();

    return 0;
}