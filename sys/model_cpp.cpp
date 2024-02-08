#include <opencv2/opencv.hpp>
#include <opencv2/ml.hpp>
#include <iostream>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

void loadImagesFromFolder(const std::string& folderPath, std::vector<cv::Mat>& images) {
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        cv::Mat img = cv::imread(entry.path().string(), cv::IMREAD_GRAYSCALE);
        if (!img.empty()) {
            images.push_back(img);
        }
    }
}

cv::Mat otsuThresholding(const cv::Mat& image) {
    cv::Mat img_bin;
    cv::threshold(image, img_bin, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    return img_bin;
}

cv::Mat extractHOGFeatures(const cv::Mat& image) {
    std::vector<float> descriptors;
    cv::HOGDescriptor hog;
    hog.compute(image, descriptors);

    // Convert to cv::Mat (optional, depending on further processing)
    cv::Mat hogFeatures(descriptors, true);
    return hogFeatures;
}

int main() {
    std::vector<cv::Mat> posImages, negImages;
    loadImagesFromFolder("path_to_positive_samples", posImages);
    loadImagesFromFolder("path_to_negative_samples", negImages);

    // Example: Extract HOG features from the first positive image
    if (!posImages.empty()) {
        cv::Mat img_bin = otsuThresholding(posImages[0]);
        cv::Mat hogFeatures = extractHOGFeatures(img_bin);

        std::cout << "HOG Features size: " << hogFeatures.size() << std::endl;
    }

    // SVM training and prediction (simplified example)
    // Prepare your data for SVM (labels and training data)
    // Train SVM
    cv::Ptr<cv::ml::SVM> svm = cv::ml::SVM::create();
    svm->setType(cv::ml::SVM::C_SVC);
    svm->setKernel(cv::ml::SVM::LINEAR);
    svm->train(/* trainingData */, cv::ml::ROW_SAMPLE, /* labels */);

    // Predict
    // svm->predict(/* testData */);

    return 0;
}
