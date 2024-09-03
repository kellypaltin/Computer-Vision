
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
    Mat imagen = imread("San-Basilio.jpg", IMREAD_UNCHANGED); // Carga la imagen a colores (formato BGR)

    // Manipulación de pixeles
    // En una imagen a colores, los pixeles están representados
    // por el número de canales que tiene la imagen, es decir,
    // es un arreglo de N elementos, donde N = #canales

    // Obtenemos el pixel del centro de la imagen
    int fila = imagen.rows/2;
    int columna = imagen.cols/2;

    Vec3b pixel = imagen.at<Vec3b>(fila, columna);

    cout << "Fila: " << fila << " Columna: " << columna << " R: " << (int)pixel[2] << " G: " << (int)pixel[1] << " B: " << (int)pixel[0] << endl;

    for (int i=0;i<130;i++){
        for(int j=0;j<130;j++){
            if(i==j)
                imagen.at<Vec3b>(i,j) = Vec3b(0,0,0);
        }
    }

    // Ejemplo del método usado para crear imágenes vacías
    Mat gris = Mat::zeros(Size(imagen.cols, imagen.rows), CV_8UC1);

    // Cómo clonar imágenes
    Mat grisPonderada = gris.clone();

    Mat grisOpenCV;

    // Rellenamos la matriz de grises con el promedio de los canales 
    // de la imagen a color
    int pixelGris = 0;

    for (int i=0;i<gris.rows;i++){
        for (int j=0;j<gris.cols;j++){
            pixel = imagen.at<Vec3b>(i,j);
            gris.at<uchar>(i,j) = (pixel[0]+pixel[1]+pixel[2])/3;

            grisPonderada.at<uchar>(i,j) = 0.114*pixel[0]+0.587*pixel[1]+0.299*pixel[2];
        }
    }

    cvtColor(imagen, grisOpenCV, COLOR_BGR2GRAY);

    // Uso de primitivas gráficas
    rectangle(imagen, Point(123, 177), Point(200,70), Scalar(0,255,255),-1);
    putText(imagen, "OpenCV!!!", Point(233,33), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(3,3,233));
    line(imagen, Point(0,0), Point(100,100), Scalar(3,200,3), 3);

    // Creamos la ventana y le asignamos un título a la misma
    namedWindow("Imagen", WINDOW_AUTOSIZE);
    namedWindow("Gris", WINDOW_AUTOSIZE);
    namedWindow("Gris-Ponderada", WINDOW_AUTOSIZE);
    namedWindow("Gris-OpenCV", WINDOW_AUTOSIZE);

    // Mostramos la imagen en la ventana que se creó previamente
    imshow("Imagen", imagen);
    imshow("Gris", gris);
    imshow("Gris-Ponderada", grisPonderada);
    imshow("Gris-OpenCV", grisOpenCV);

    // Ponemos una pausa para alcanzar a visualizar la imagen
    waitKey(0);

    // Eliminamos las ventanas que ya no se utilizan
    destroyAllWindows();

    // Manejo de Video
    VideoCapture video(0);

    // Verificamos si la cámara se pudo abrir
    if (video.isOpened()){
        namedWindow("Video", WINDOW_AUTOSIZE);
        Mat frame;

        while(3==3){
            video >> frame;
            flip(frame, frame, 1);
            
            imshow("Video", frame);

            if(waitKey(23)==27)
                break;
        }
        video.release();
        destroyAllWindows();
    }

    return 0;
}