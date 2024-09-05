#ifndef PROJECT_LBP_H
#define PROJECT_LBP_H

#include <opencv2/opencv.hpp>
#include <vector>

class LBP {
public:
    LBP();
    int* LBP8(const int* data, int rows, int columns);
    std::vector<int> calcularLBP(cv::Mat imagen);
    cv::Mat calcularLBPImage(cv::Mat imagen);
private:
    inline void compab_mask_inc(const int* p, int bit, unsigned int &value, int &cntr);
};


#endif // LBP_HPP
