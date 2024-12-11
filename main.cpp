#include <Windows.h>
#include <shlwapi.h>
#include <iostream>
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"

using namespace std;

static float features[EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT];

template<typename ... Args>  
std::string str_format(const char* fmt, Args ... args) {
    std::string ret;
    int length = std::snprintf(nullptr, 0, fmt, args...);
    if(length > 0) {
        char* buf = new char[length + 1];
        std::snprintf(buf, length + 1, fmt, args...);  
        ret = buf;
        delete[] buf;
    }
    return ret;
}

void logIt(string msg) {
    string s = str_format("[EI]: %s", msg.c_str());
    OutputDebugString(s.c_str());
}
// void logIt(char* msg) {
//     string s = str_format("[EI]: %s", msg);
//     OutputDebugString(s.c_str());
// }

void resize_and_crop(cv::Mat *in_frame, cv::Mat *out_frame)
{
    // to resize... we first need to know the factor
    float factor_w = static_cast<float>(EI_CLASSIFIER_INPUT_WIDTH) / static_cast<float>(in_frame->cols);
    float factor_h = static_cast<float>(EI_CLASSIFIER_INPUT_HEIGHT) / static_cast<float>(in_frame->rows);

    float largest_factor = factor_w > factor_h ? factor_w : factor_h;

    cv::Size resize_size(static_cast<int>(largest_factor * static_cast<float>(in_frame->cols)),
        static_cast<int>(largest_factor * static_cast<float>(in_frame->rows)));

    cv::Mat resized;
    cv::resize(*in_frame, resized, resize_size);

    int crop_x = resize_size.width > resize_size.height ?
        (resize_size.width - resize_size.height) / 2 :
        0;
    int crop_y = resize_size.height > resize_size.width ?
        (resize_size.height - resize_size.width) / 2 :
        0;

    cv::Rect crop_region(crop_x, crop_y, EI_CLASSIFIER_INPUT_WIDTH, EI_CLASSIFIER_INPUT_HEIGHT);

    //if (use_debug) {
    //    printf("crop_region x=%d y=%d width=%d height=%d\n", crop_x, crop_y, EI_CLASSIFIER_INPUT_WIDTH, EI_CLASSIFIER_INPUT_HEIGHT);
    //}

    *out_frame = resized(crop_region);
}

int main(int argc, char *argv[]) {
    
}

int __main(int argc, char *argv[]) {
    int ret = ERROR_INVALID_PARAMETER;
    char buf[MAX_PATH];
    ZeroMemory(buf, sizeof(buf));
    GetCurrentDirectory(MAX_PATH, buf);
    string s = str_format("Current Dir: %s", buf);
    logIt(s);
    // cout << s << endl;
    string fn;
    if (argc > 1) {
        fn = argv[1];
        logIt(str_format("From command line image filename: %s.", argv[1]));
    }
    else {
        logIt(str_format("Missing image filename."));
        return ERROR_INVALID_PARAMETER;
    }
    if (PathFileExists(fn.c_str())) {
        cv::Mat img = cv::imread(fn.c_str());
        if (img.empty()) {
            logIt(str_format("Fail to read image filename (%s).", fn.c_str()));
            return ERROR_INVALID_PARAMETER;
        }
        else {
            logIt(str_format("Loaded image filename (%s). (%dx%d). resize to (%dx%d)", 
            fn.c_str(), img.cols, img.rows, EI_CLASSIFIER_INPUT_WIDTH, EI_CLASSIFIER_INPUT_HEIGHT));
            // resize and crop image
            cv::Mat cropped;
            resize_and_crop(&img, &cropped);
            // calc feature
            if (!cropped.empty()) {
                size_t feature_ix = 0;
                for (int rx = 0; rx < (int)input.rows; rx++) {
                    for (int cx = 0; cx < (int)input.cols; cx++) {
                        cv::Vec3b pixel = input.at<cv::Vec3b>(rx, cx);
                        uint8_t b = pixel.val[0];
                        uint8_t g = pixel.val[1];
                        uint8_t r = pixel.val[2];
                        features[feature_ix++] = (r << 16) + (g << 8) + b;
                    }
                }
            }
            // construct a signal from the features buffer 
            signal_t signal;
            numpy::signal_from_buffer(features, EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT, &signal);
            // run the classifier
            ei_impulse_result_t result;
            EI_IMPULSE_ERROR res = run_classifier(&signal, &result, false);
            if (res != 0) {
                printf("ERR: Failed to run classifier (%d)\n", res);
                return -1;
            }
            logIt(str_format("Predictions (DSP: %d ms., Classification: %d ms., Anomaly: %d ms.): \n",
                result.timing.dsp, result.timing.classification, result.timing.anomaly)) 
            printf("#Classification results:\n");
            for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
                printf("%s: %.05f\n", result.classification[ix].label, result.classification[ix].value);
            }                
        }
    }
    else {
        ret = ERROR_FILE_NOT_FOUND;
    }
    return ret;
}