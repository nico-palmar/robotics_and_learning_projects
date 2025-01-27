#include <iostream>
#include <memory>
#include <NvInfer.h>
#include <NvOnnxParser.h>

class Logger : public nvinfer1::ILogger {
public:
    void log(nvinfer1::ILogger::Severity severity, const char* msg) noexcept override {
        if (severity != nvinfer1::ILogger::Severity::kINFO) {
            std::cout << msg << std::endl;
        }
    }
};

int main() {
    try {
        // Instantiate TensorRT's Logger and Runtime classes
        Logger logger;
        std::unique_ptr<nvinfer1::IRuntime> runtime { nvinfer1::createInferRuntime(logger) };

        if (runtime) {
            std::cout << "TensorRT is installed correctly!" << std::endl;
        } else {
            std::cerr << "TensorRT is not installed or not configured correctly." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
