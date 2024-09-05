#include <opencv2/opencv.hpp>
#include <opencv2/ml.hpp>
#include <filesystem>
#include <iostream>
#include <string>
#include <iomanip>
#include <map>
#include <algorithm>

using namespace cv;
using namespace std;
using namespace cv::ml;
namespace fs = std::filesystem;

vector<Mat> all_images;
vector<string> labels;
vector<float> descriptors;

HOGDescriptor hog(
    Size(32, 32),
    Size(16, 16),
    Size(8, 8), 
    Size(8, 8),   
    9             
);

pair<string, bool> containsKeyword(string filename) {
    if (filename.find("google") != std::string::npos) { return {"google", true};} 
    else if (filename.find("amazon") != std::string::npos) { return {"amazon", true};} 
    else if (filename.find("facebook") != std::string::npos) { return {"facebook", true};} 
    else if (filename.find("youtube") != std::string::npos) { return {"youtube", true};} 
    else { return {"", false}; }
}

vector<float> HOG(string filename){
    Mat img = imread(filename,IMREAD_GRAYSCALE);
    resize(img, img, Size(32, 32), 0, 0, INTER_LINEAR);

    Mat blurred;
    GaussianBlur(img, blurred, Size(5, 5), 0);                     

    hog.compute(blurred, descriptors);

    return descriptors;
}

void saveSVM(const Ptr<SVM>& svm, const string& filename) {
    svm->save(filename);
    cout << "Modelo SVM guardado con éxito" << endl;
}

void visualizeConfusionMatrix(int TP, int TN, int FP, int FN, const string& className) {
    Mat confusionMat(800, 800, CV_8UC3, Scalar(255, 255, 255));  
    rectangle(confusionMat, Point(100, 100), Point(700, 700), Scalar(0, 0, 0), 2);
    line(confusionMat, Point(100, 400), Point(700, 400), Scalar(0, 0, 0), 2);
    line(confusionMat, Point(400, 100), Point(400, 700), Scalar(0, 0, 0), 2);

    putText(confusionMat, "Actual Values", Point(300, 70), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2);
    putText(confusionMat, "Predicted     Values", Point(200, 450), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2, true);

    putText(confusionMat, "Negative:", Point(120, 150), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2);
    putText(confusionMat, "Positive:", Point(520, 150), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2);

    putText(confusionMat, "Negative:", Point(120, 500), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2, true);
    putText(confusionMat, "Positive:", Point(520, 500), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2, true);

    putText(confusionMat, "TN: " + to_string(TN), Point(150, 300), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2);
    putText(confusionMat, "FP: " + to_string(FP), Point(450, 300), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2);
    putText(confusionMat, "FN: " + to_string(FN), Point(150, 550), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2);
    putText(confusionMat, "TP: " + to_string(TP), Point(450, 550), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2);

    putText(confusionMat, "Confusion Matrix for: " + className, Point(150, 750), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 2);

    imshow("Confusion Matrix", confusionMat);
    waitKey(0);
    destroyAllWindows();
}

int main(int argc, char** argv) {

    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <test image path>" << endl;
        return -1;
    }

    string test_image_path = argv[1];
    string path = "/home/lubuntu/Descargas/Task3.2/Logos";
    vector<vector<float>> hog_features;  

    for (const auto& entry : fs::directory_iterator(path)) {
        std::string filename = entry.path().filename().string();
        auto result = containsKeyword(filename);
        if (result.second) {
            vector<float> descriptor = HOG(entry.path());
            labels.push_back(result.first);
            hog_features.push_back(descriptor);
        }
    }
    cout << "Cantidad de Labels: " << labels.size() << endl;
    cout << "Shape de los descriptores HOG: " << hog_features.size() << " x " << hog_features[0].size() << endl;

    map<string, int> labelMap;
    int labelCounter = 0;
    for (const auto& label : labels) {
        if (labelMap.find(label) == labelMap.end()) {
            labelMap[label] = labelCounter++;
        }
    }

    vector<int> numericLabels(labels.size());
    transform(labels.begin(), labels.end(), numericLabels.begin(),
        [&](const string& label) { return labelMap[label]; });

    Mat hogMat(hog_features.size(), hog_features[0].size(), CV_32FC1);
    for (size_t i = 0; i < hog_features.size(); ++i) {
        for (size_t j = 0; j < hog_features[i].size(); ++j) {
            hogMat.at<float>(i, j) = hog_features[i][j];
        }
    }

    Ptr<TrainData> data = TrainData::create(hogMat, ROW_SAMPLE, Mat(numericLabels));
    data->setTrainTestSplitRatio(0.8, true);

    Ptr<SVM> svm = SVM::create();
    svm->setKernel(SVM::POLY);
    svm->setDegree(3);
    svm->setGamma(0.1);
    svm->setC(1);
    svm->train(data->getTrainSamples(), ROW_SAMPLE, data->getTrainResponses());

    Mat predictions;
    svm->predict(data->getTestSamples(), predictions);

    int correct = 0;
    for (int i = 0; i < predictions.rows; ++i) {
        if (predictions.at<float>(i, 0) == data->getTestResponses().at<int>(i, 0)) {
            correct++;
        }
    }
    float accuracy = (float)correct / predictions.rows;
    cout << "Precisión del modelo SVM: " << accuracy << endl;

    saveSVM(svm, "svm_classifier.xml");

    Mat i = imread(test_image_path);
    vector<float> test_descriptor = HOG(test_image_path);

    Mat test_mat(1, test_descriptor.size(), CV_32FC1);
    memcpy(test_mat.data, test_descriptor.data(), test_descriptor.size() * sizeof(float));

    normalize(test_mat, test_mat, 0, 1, NORM_MINMAX);

    float prediction = svm->predict(test_mat);

    string predicted_label;
    for (const auto& pair : labelMap) {
        if (pair.second == (int)prediction) {
            predicted_label = pair.first;
            break;
        }
    }

    resize(i, i, Size(400, 400));
    putText(i, "Prediccion: " + predicted_label, Point(10, 30), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(255, 0, 255), 2);
    imshow("Imagen de prueba con prediccion", i);

    int TP = 0, TN = 0, FP = 0, FN = 0;
    int actual_label = labelMap[predicted_label];
    for (int j = 0; j < data->getTestResponses().rows; ++j) {
        int actual = data->getTestResponses().at<int>(j, 0);
        int predicted = predictions.at<float>(j, 0);
        if (actual == actual_label && predicted == actual_label) {
            TP++;
        } else if (actual != actual_label && predicted != actual_label) {
            TN++;
        } else if (actual != actual_label && predicted == actual_label) {
            FP++;
        } else if (actual == actual_label && predicted != actual_label) {
            FN++;
        }
    }

    visualizeConfusionMatrix(TP, TN, FP, FN, predicted_label);

    waitKey(0);
    destroyAllWindows();

    return 0;
}
