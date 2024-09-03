#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main() {
    // Lectura de las imágenes original y modificada
    Mat imagenOriginal = imread("/home/lubuntu/Documentos/practicar/screen3.jpg");
    Mat imagenModificada = imread("/home/lubuntu/Documentos/practicar/screen4.jpg");

    // Si las imágenes no se pudieron leer, se muestra un mensaje de error y se finaliza el programa
    if (imagenOriginal.empty() || imagenModificada.empty()) {
        cout << "Error al leer las imágenes" << endl;
        return -1;
    }

    // Convertir la imagen modificada a escala de grises
    Mat imagenModificadaGrayscale;
    cvtColor(imagenModificada, imagenModificadaGrayscale, COLOR_BGR2GRAY);

    // Aplicar un umbral para obtener una imagen binaria que resalte el triángulo
    Mat imagenUmbral;
    threshold(imagenModificadaGrayscale, imagenUmbral, 240, 255, THRESH_BINARY);

    // Encontrar contornos en la imagen binaria
    vector<vector<Point>> contornos;
    findContours(imagenUmbral, contornos, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    // Crear una máscara para el triángulo
    Mat mascaraTriangulo = Mat::zeros(imagenModificada.size(), CV_8UC1);
    drawContours(mascaraTriangulo, contornos, 0, Scalar(255), FILLED);

    // Invertir la máscara para que los píxeles dentro del triángulo sean negros
    mascaraTriangulo = 255 - mascaraTriangulo;

    // Crear la imagen resultante, inicialmente una copia de la imagen original en color
    Mat imagenResultante = imagenOriginal.clone();

    // Aplicar la máscara para eliminar los píxeles dentro del triángulo
    imagenOriginal.copyTo(imagenResultante, mascaraTriangulo);

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
