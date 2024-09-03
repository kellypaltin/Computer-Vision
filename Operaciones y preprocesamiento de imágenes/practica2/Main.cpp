#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main() {
    // Lectura de las imágenes original y modificada
    Mat imagenOriginal = imread("/home/lubuntu/Descargas/VISION/IMÁGENES-Evaluación-Práctica-UNIDAD-2/imagenes_originales/CHARLES_BRONSON.png");
    Mat imagenModificada = imread("/home/lubuntu/Descargas/VISION/IMÁGENES-Evaluación-Práctica-UNIDAD-2/assigments/CHARLES_BRONSON.png");

    // Si las imágenes no se pudieron leer, se muestra un mensaje de error y se finaliza el programa
    if (imagenOriginal.empty() || imagenModificada.empty()) {
        cout << "Error al leer las imágenes" << endl;
        return -1;
    }

    // Crear la máscara restando las imágenes
    Mat mascara;
    absdiff(imagenModificada, imagenOriginal, mascara);

    // Convertir la máscara a escala de grises
    cvtColor(mascara, mascara, COLOR_BGR2GRAY);

    // Crear la imagen resultante, inicialmente una copia de la imagen original en color
    Mat imagenResultante = imagenOriginal.clone();

    // Recorrer la máscara de izquierda a derecha
    for (int i = 0; i < imagenOriginal.rows; i++) {
        for (int j = 0; j < imagenOriginal.cols; j++) {
            if (mascara.at<uchar>(i, j) == 0) {
                Vec3b pixelOriginal = imagenOriginal.at<Vec3b>(i, j);
                int promedio = (pixelOriginal[0] + pixelOriginal[1] + pixelOriginal[2]) / 3;
                Vec3b pixelGris(promedio, promedio, promedio);
                imagenResultante.at<Vec3b>(i, j) = pixelGris;
            } else {
                break;
            }
        }
    }

    // Recorrer la máscara de derecha a izquierda
    for (int i = 0; i < imagenOriginal.rows; i++) {
        for (int j = imagenOriginal.cols - 1; j >= 0; j--) {
            if (mascara.at<uchar>(i, j) == 0) {
                Vec3b pixelOriginal = imagenOriginal.at<Vec3b>(i, j);
                int promedio = (pixelOriginal[0] + pixelOriginal[1] + pixelOriginal[2]) / 3;
                Vec3b pixelGris(promedio, promedio, promedio);
                imagenResultante.at<Vec3b>(i, j) = pixelGris;
            } else {
                break;
            }
        }
    }

    // Visualización de las imágenes
    imshow("Imagen original", imagenOriginal);
    imshow("Imagen modificada", imagenModificada);
    imshow("Imagen resultante", imagenResultante);

    // Espera a que se presione una tecla para cerrar las ventanas
    waitKey(0);

    // Liberación de recursos
    destroyAllWindows();

    return 0;
}
