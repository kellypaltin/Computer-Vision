#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main() {
    // Leer las imágenes original y modificada
    Mat imagenOriginal = imread("/home/lubuntu/Documentos/practicar/Simulacro-Prueba-Manipulación-Pixeles-Imágenes-Triángulos/san-petesburgo.jpg");
    Mat imagenModificada = imread("/home/lubuntu/Documentos/practicar/Simulacro-Prueba-Manipulación-Pixeles-Imágenes-Triángulos/imagentriangulo.png");

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

    // Crear la imagen resultante, inicialmente blanca
    Mat imagenResultante(imagenOriginal.size(), imagenOriginal.type(), Scalar(255, 255, 255));

    // Recorrer la máscara de izquierda a derecha
    for (int i = 0; i < imagenOriginal.rows; i++) {
        bool dentroTriangulo = false;
        for (int j = 0; j < imagenOriginal.cols; j++) {
            if (mascara.at<uchar>(i, j) > 0) {
                dentroTriangulo = true;
                break;
            }
            imagenResultante.at<Vec3b>(i, j) = imagenOriginal.at<Vec3b>(i, j);
        }
    }

    // Recorrer la máscara de derecha a izquierda
    for (int i = 0; i < imagenOriginal.rows; i++) {
        bool dentroTriangulo = false;
        for (int j = imagenOriginal.cols - 1; j >= 0; j--) {
            if (mascara.at<uchar>(i, j) > 0) {
                dentroTriangulo = true;
                break;
            }
            imagenResultante.at<Vec3b>(i, j) = imagenOriginal.at<Vec3b>(i, j);
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
