#ifndef TAREA_ZERNIKE_H
#define TAREA_ZERNIKE_H


#include <complex>
#include <vector>
#include <opencv2/opencv.hpp>

void mb_Znl(const std::vector<double>& X, const std::vector<double>& Y, const std::vector<double>& P, double D, double m10_m00, double m01_m00,
            double R, double psum, std::vector<double>& zvalues);


#endif //TAREA_ZERNIKE_H
