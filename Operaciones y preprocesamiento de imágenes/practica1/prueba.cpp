//2023_7_8_0_5_71
//2023+7+8=2038
//canal=0 = blue azul
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

void kernel_vecindario(Mat& input, Mat& output, int x, int y, int vecindario_size) {
    int mitad = vecindario_size / 2;
    int x_inicio = max(0, x - mitad);
    int x_end = min(input.rows, x + mitad + 1);
    int y_inicio = max(0, y - mitad);
    int y_end = min(input.cols, y + mitad + 1);

    for (int i = x_inicio; i < x_end; ++i) {
        for (int j = y_inicio; j < y_end; ++j) {
            output.at<Vec3b>(i, j) = input.at<Vec3b>(i, j);
        }
    }
}

int main() {
    int anio = 2023;
    int mes = 7;
    int dia = 8;
    int canal = 0; 
    int vecindario = 5; 
    int minimo = 71; 

    int suma_valor = anio + mes + dia;

    Mat image = imread("/home/lubuntu/assignments/PALTIN_GUZMAN_KELLY_DENISE_2023_7_8_0_5_71.png");

    Mat output_image(image.size(), image.type(), Scalar(0, 0, 0));

    for (int i = 0; i < image.rows; ++i) {
        for (int j = 0; j < image.cols; ++j) {
            int pixel_valor = image.at<Vec3b>(i, j)[canal]; 
            if ((pixel_valor != 0 && suma_valor % pixel_valor == 0) || pixel_valor <= minimo) {
                kernel_vecindario(image, output_image, i, j, vecindario);
            }
        }
    }

    imwrite("output_image.png", output_image);

    imshow("Original Image", image);
    imshow("Output Image", output_image);
    waitKey(0);
    destroyAllWindows();

    return 0;
}
