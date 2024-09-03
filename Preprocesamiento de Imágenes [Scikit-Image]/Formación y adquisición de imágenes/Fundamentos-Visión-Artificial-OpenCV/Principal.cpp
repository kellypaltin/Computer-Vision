
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

    // Cargamos una imagen guardada en el disco y la representamos como matriz
    Mat imagen = imread("San-Basilio.jpg"); // Carga la imagen a colores (formato BGR)

    imagen = imread("San-Basilio.jpg", IMREAD_GRAYSCALE); // Carga la imagen en escala de grises

    // Cómo obtener información de la imagen que se leyó
    // Tamaño de la imagen
    cout << "Alto de la imagen = " << imagen.rows << " Ancho de la imagen = " << imagen.cols << " Canales: " << imagen.channels() << endl;

    // Creamos la ventana y le asignamos un título a la misma
    namedWindow("Imagen", WINDOW_AUTOSIZE);

    // Mostramos la imagen en la ventana que se creó previamente
    imshow("Imagen", imagen);

    // Cómo obtener información de la versión de OpenCV instalada
    cout << getBuildInformation() << endl;

    // Ponemos una pausa para alcanzar a visualizar la imagen
    waitKey(0);

    return 0;
}