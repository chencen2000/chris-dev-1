#include <Windows.h>
#include <shlwapi.h>
#include <iostream>
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"

using namespace std;

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

int main(int argc, char *argv[]) {
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
            // TODO: resize and crop image
            
        }
    }
    else {
        ret = ERROR_FILE_NOT_FOUND;
    }
    return ret;
}