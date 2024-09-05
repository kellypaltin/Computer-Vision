#include <opencv2/opencv.hpp>
#include "LBP.hpp"
#include <iostream>
#include <fstream>  // Asegúrate de incluir esta biblioteca

using namespace std;
using namespace cv;

void saveLBPHistogram(vector<int> histo, const string& filename) {
    ofstream file(filename);
    if (file.is_open()) {
        for (int val : histo) {
            file << val << " ";
        }
        file.close();
    }
}

vector<int> averageHistograms(vector<string> files) {
    vector<int> avgHistogram(256, 0);
    int numFiles = files.size();
    for (const string& file : files) {
        ifstream infile(file);
        int val;
        for (int i = 0; i < 256; i++) {
            infile >> val;
            avgHistogram[i] += val;
        }
        infile.close();
    }
    for (int i = 0; i < 256; i++) {
        avgHistogram[i] /= numFiles;
    }
    return avgHistogram;
}

void plotHistogram(Mat& image, const vector<int>& histo, const string& windowName) {
    int histSize = 256;
    int hist_w = 512; int hist_h = 400;
    int bin_w = cvRound((double)hist_w / histSize);
    Mat histImage(hist_h, hist_w, CV_8UC1, Scalar(0, 0, 0));

    for (int i = 1; i < histSize; i++) {
        line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(histo[i - 1])),
            Point(bin_w*(i), hist_h - cvRound(histo[i])),
            Scalar(255, 255, 255), 2, 8, 0);
    }

    namedWindow(windowName, WINDOW_AUTOSIZE);
    imshow(windowName, histImage);
    imshow("LBP Image", image);
}

int main(int argc, char* argv[]) {
    LBP* lbp = new LBP();

    // Cargar imágenes de entrenamiento y calcular LBP
    vector<string> class1Images = {"madera1.jpeg", "madera2.jpeg", "madera3.jpg", "madera4.jpg"};
    vector<string> class2Images = {"tela1.jpg", "tela2.jpg", "tela3.jpg", "tela4.jpg"};

    for (const string& imageFile : class1Images) {
        Mat img = imread(imageFile, IMREAD_GRAYSCALE);
        vector<int> histo = lbp->calcularLBP(img);
        saveLBPHistogram(histo, imageFile + "_histogram.txt");
    }

    for (const string& imageFile : class2Images) {
        Mat img = imread(imageFile, IMREAD_GRAYSCALE);
        vector<int> histo = lbp->calcularLBP(img);
        saveLBPHistogram(histo, imageFile + "_histogram.txt");
    }

    // Promediar histogramas de entrenamiento
    vector<int> class1Histogram = averageHistograms({"madera1.jpeg_histogram.txt", "madera2.jpeg_histogram.txt", "madera3.jpg_histogram.txt", "madera4.jpg_histogram.txt"});
    vector<int> class2Histogram = averageHistograms({"tela1.jpg_histogram.txt", "tela2.jpg_histogram.txt", "tela3.jpg_histogram.txt", "tela4.jpg_histogram.txt"});

    // Clasificar la imagen adjunta
    Mat textura = imread("textura.jpeg", IMREAD_GRAYSCALE);
    namedWindow("Textura", WINDOW_AUTOSIZE);
    imshow("Textura", textura);

    // Calcular imagen LBP y mostrar
    Mat lbpImage = lbp->calcularLBPImage(textura);
    vector<int> textureHistogram = lbp->calcularLBP(textura);

    plotHistogram(lbpImage, textureHistogram, "Histogram");

    // Comparar histogramas y clasificar
    double distanciaClass1 = norm(class1Histogram, textureHistogram, NORM_L2);
    double distanciaClass2 = norm(class2Histogram, textureHistogram, NORM_L2);

    if (distanciaClass1 > distanciaClass2) {
        cout << "La imagen de prueba pertenece a Clase 1 (Madera)" << endl;
    } else {
        cout << "La imagen de prueba pertenece a Clase 2 (Tela de ropa)" << endl;
    }

    waitKey(0);
    return 0;
}
