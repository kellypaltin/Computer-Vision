#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

void cutImageWithSineFunction(const string& imagePath, const string& outputPath) {
    // Load image
    Mat image = imread(imagePath, IMREAD_UNCHANGED);

    if (image.empty()) {
        cerr << "Could not open the image!" << endl;
        return;
    }

    int width = image.cols;
    int height = image.rows;

    Mat output = image.clone();

    // Parameters
    float scaleX = 30.0;
    float scaleY = 30.0;
    float step = 0.08;

    for (float x = 0; x < width; x += step) {
        int y = static_cast<int>((height / 2) - scaleY * sin(x / scaleX)); // Modificado para la parte inferior
        for (int i = y; i < height; i++) { // Cambiado el bucle para empezar desde y
            output.at<Vec4b>(i, static_cast<int>(x)) = Vec4b(0, 0, 0, 0); // Transparent
        }
    }

    imwrite(outputPath, output);
}

int main() {
    cutImageWithSineFunction("/home/lubuntu/Documentos/practicar/Simulacro-Prueba-Manipulación-Pixeles-Imágenes-Triángulos/san-petesburgo.png", "output_cut_image.png");
    return 0;
}
