

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
//#include <opencv2/xfeatures2d.hpp>

#include <opencv2/features2d/features2d.hpp>

using namespace std;
using namespace cv;



int main(int argc, char* args[]){

    Mat catedral = imread("San-Basilio-Logo.jpg");
    Mat logo = imread("logoCatedraTransparencia.jpg");

    Mat catedralKP = catedral.clone();
    Mat logoKP = logo.clone();

    // Detector SIFT
    Ptr<SIFT> sDetector = SIFT::create();

    vector<KeyPoint> keypointsCat, keypointsLogo;
    Mat descriptorCat, descriptorLogo;

    sDetector->detect(catedral, keypointsCat);
    sDetector->detect(logo, keypointsLogo);

    sDetector->compute(catedral, keypointsCat, descriptorCat);
    sDetector->compute(logo, keypointsLogo, descriptorLogo);

    drawKeypoints(catedralKP, keypointsCat, catedralKP);
    drawKeypoints(logoKP, keypointsLogo, logoKP);

    BFMatcher matcher;
    Mat img_matches;

    vector<vector <DMatch> > matches;
    vector<DMatch> matchesFiltrados;

    matcher.knnMatch(descriptorLogo, descriptorCat,matches,2);

    float ratio = 0.67;

    for(int i=0;i<matches.size();i++){
        if(matches[i][0].distance < ratio*matches[i][1].distance){
            matchesFiltrados.push_back(matches[i][0]);
        }
    }

    cout << "Matches totales = " << matches.size() << " Filtrados = " << matchesFiltrados.size() << endl;

    Mat resultado;
    drawMatches(logo, keypointsLogo, catedral, keypointsCat, matchesFiltrados, resultado);

    namedWindow("San-Basilio", WINDOW_AUTOSIZE);
    namedWindow("Logo", WINDOW_AUTOSIZE);
    namedWindow("San-Basilio-KP", WINDOW_AUTOSIZE);
    namedWindow("Logo-KP", WINDOW_AUTOSIZE);


    imshow("San-Basilio", catedral);
    imshow("San-Basilio-KP", catedralKP);
    imshow("Logo", logo);
    imshow("Logo-KP", logoKP);
    imshow("Resultado", resultado);


    waitKey(0);
    destroyAllWindows();

    return 0;
}