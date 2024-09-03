
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    // Cargar las imágenes
    Mat imgCombinada1 = imread("/home/lubuntu/Descargas/practicas/CHARLES_BRONSON-Imagen-1.png");
    Mat imgCombinada2 = imread("/home/lubuntu/Descargas/practicas/CHARLES_BRONSON-Imagen-2.png");
    Mat lineaGuia = imread("/home/lubuntu/Descargas/practicas/CHARLES_BRONSON-Lienzo.png");

    if (imgCombinada1.empty() || imgCombinada2.empty() || lineaGuia.empty()) {
        cout << "Error al cargar las imágenes." << endl;
        return -1;
    }

    Mat imgResultado1 = imgCombinada1.clone();
    Mat imgResultado2 = imgCombinada2.clone();

    // Detectar la línea guía y realizar el intercambio
    for (int x = 0; x < lineaGuia.cols; x++) {
        int yStart = 0;
        while (yStart < lineaGuia.rows) {
            Vec3b color = lineaGuia.at<Vec3b>(yStart, x);
            // Detectar un color significativamente distinto del fondo para la guía
            if (color != Vec3b(0, 0, 0)) {  // Cambiar según el color exacto de la guía
                break;
            }
            yStart++;
        }

        // Intercambiar los píxeles por debajo del punto encontrado
        for (int y = yStart; y < imgCombinada1.rows; y++) {
            imgResultado1.at<Vec3b>(y, x) = imgCombinada2.at<Vec3b>(y, x);
            imgResultado2.at<Vec3b>(y, x) = imgCombinada1.at<Vec3b>(y, x);
        }
    }

    // Mostrar y guardar las imágenes resultantes
    imshow("Imagen Resultante 1", imgResultado1);
    imshow("Imagen Resultante 2", imgResultado2);
    waitKey(0);

    return 0;
}