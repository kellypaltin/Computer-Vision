#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace std;
using namespace cv;

void applyMorphologicalOperations(const Mat& img, const Size& kernel_size) {
    Mat kernel = getStructuringElement(MORPH_RECT, kernel_size);

    Mat erosion;
    erode(img, erosion, kernel, Point(-1, -1), 1);

    Mat dilation;
    dilate(img, dilation, kernel, Point(-1, -1), 1);

    Mat tophat;
    morphologyEx(img, tophat, MORPH_TOPHAT, kernel);

    Mat blackhat;
    morphologyEx(img, blackhat, MORPH_BLACKHAT, kernel);

    Mat ecuacion;
    add(img, tophat - blackhat, ecuacion);

    imshow("Original (Kernel " + to_string(kernel_size.width) + "x" + to_string(kernel_size.height) + ")", img);
    imshow("Erosión (Kernel " + to_string(kernel_size.width) + "x" + to_string(kernel_size.height) + ")", erosion);
    imshow("Dilatación (Kernel " + to_string(kernel_size.width) + "x" + to_string(kernel_size.height) + ")", dilation);
    imshow("Top Hat (Kernel " + to_string(kernel_size.width) + "x" + to_string(kernel_size.height) + ")", tophat);
    imshow("Black Hat (Kernel " + to_string(kernel_size.width) + "x" + to_string(kernel_size.height) + ")", blackhat);
    imshow("Ecuacion (Kernel " + to_string(kernel_size.width) + "x" + to_string(kernel_size.height) + ")", ecuacion);

    waitKey(0);
    destroyAllWindows();
}

int main() {
    vector<string> image_files = {"/home/kelly/Descargas/imagen1.jpg", "/home/kelly/Descargas/imagen2.jpg", "/home/kelly/Descargas/imagen3.jpg"};

    vector<Size> kernel_sizes = {Size(35, 35), Size(37, 37), Size(39, 39)};

    for (const string& file : image_files) {
        Mat img = imread(file, IMREAD_GRAYSCALE);
        if (img.empty()) {
            cout << "El archivo " << file << " no pudo ser leído, verifica la ruta" << endl;
            continue;
        }

        for (const Size& kernel_size : kernel_sizes) {
            applyMorphologicalOperations(img, kernel_size);
        }
    }

    return 0;
}
