#include <NvInfer.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <cassert>
#include <opencv2/opencv.hpp>   // Main OpenCV header
#include <opencv2/core.hpp>     // Core functionalities (Mat, etc.)
#include <opencv2/imgcodecs.hpp> // Image reading/writing (imread, imwrite)
#include <opencv2/imgproc.hpp>   // Image processing (resize, convertTo, etc.)


class Logger : public nvinfer1::ILogger {
public:
    void log(nvinfer1::ILogger::Severity severity, const char* msg) noexcept override {
        if (severity != nvinfer1::ILogger::Severity::kVERBOSE) {
            std::cout << msg << std::endl;
        }
    }
};

std::vector<char> readModelFromFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);  
    if (!file) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    std::streamsize size = file.tellg();  
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        throw std::runtime_error("Failed to read file: " + filename);
    }

    return buffer;
}

void printTensorNames(nvinfer1::ICudaEngine* engine) {
    int numTensors = engine->getNbIOTensors();

    std::cout << "Number of I/O Tensors: " << numTensors << std::endl;
    
    for (int i = 0; i < numTensors; ++i) {
        const char* tensorName = engine->getIOTensorName(i);
        std::cout << "Tensor " << i << ": " << tensorName << std::endl;
    }
}


void printDimInfo(nvinfer1::Dims dims)
{
    auto nDims = dims.nbDims;
    std::cout << "nDims: " << nDims << std::endl;

    for (auto i = 0; i < nDims; i++)
    {
        std::cout << "Dim " << i << " has size " << dims.d[i] << std::endl;
    }
}

std::vector<float> preprocessForTensorRT(const std::string& imagePath, int inputHeight, int inputWidth) {
    cv::Mat img = cv::imread(imagePath);

    // std::cout << "Resize to " << inputHeight << " and " << inputWidth << std::endl;

    cv::resize(img, img, cv::Size(inputWidth, inputHeight), cv::INTER_CUBIC);
    img.convertTo(img, CV_32FC3, 1.0 / 255.0);

    std::vector<float> chw(img.total() * img.channels());
    std::vector<cv::Mat> channels(3);
    cv::split(img, channels);

    for (int i = 0; i < 3; i++) {
        memcpy(chw.data() + i * img.total(), channels[i].data, img.total() * sizeof(float));
    }

    return chw;
}

int main()
{

    Logger logger;
    nvinfer1::IRuntime* runtime = nvinfer1::createInferRuntime(logger);
    cudaStream_t stream;
    cudaStreamCreate(&stream);

    std::vector<char> modelData = readModelFromFile("../best.engine");
    nvinfer1::ICudaEngine* engine = runtime->deserializeCudaEngine(modelData.data(), modelData.size());

    nvinfer1::IExecutionContext *context = engine->createExecutionContext();

    printTensorNames(engine);

    const char* INPUT_NAME = "images";   // Update with your input tensor name
    const char* OUTPUT_NAME = "output0"; // Update with your output tensor name

    // // Query tensor sizes
    nvinfer1::Dims inputDims = engine->getTensorShape(INPUT_NAME);
    nvinfer1::Dims outputDims = engine->getTensorShape(OUTPUT_NAME);

    printDimInfo(inputDims);
    printDimInfo(outputDims);
    // nvinfer1::Dims outputDims = engine->getTensorShape(OUTPUT_NAME);

    size_t inputSize = 1;   // Compute input size in bytes
    size_t outputSize = 1;  // Compute output size in bytes
    for (int i = 0; i < inputDims.nbDims; i++) inputSize *= inputDims.d[i];
    for (int i = 0; i < outputDims.nbDims; i++) outputSize *= outputDims.d[i];

    inputSize *= sizeof(float);
    outputSize *= sizeof(float);

    std::vector<float> preprocessed_img = preprocessForTensorRT("../0000069_01878_d_0000005.jpg", inputDims.d[2], inputDims.d[3]);
    size_t input_img_size = preprocessed_img.size() * sizeof(float);
    assert(inputSize == input_img_size);

    // Allocate device memory for input/output
    // this is for the GPU
    void* d_input;
    void* d_output;
    cudaMalloc(&d_input, inputSize);
    cudaMalloc(&d_output, outputSize);

    context->setTensorAddress(INPUT_NAME, d_input);
    context->setTensorAddress(OUTPUT_NAME, d_output);

    // allocate host (CPU) memory
    // float* h_input = new float[inputSize / sizeof(float)];  // Allocate host memory
    cudaMemcpyAsync(d_input, preprocessed_img.data(), inputSize, cudaMemcpyHostToDevice, stream);

    context->enqueueV3(stream);

    float* h_output = new float[outputSize / sizeof(float)];
    cudaMemcpyAsync(h_output, d_output, outputSize, cudaMemcpyDeviceToHost, stream);
    cudaStreamSynchronize(stream);

    delete runtime;
    delete context;
    delete engine;
    // delete[] h_input;
    delete[] h_output;
    cudaFree(d_input);
    cudaFree(d_output);

    cudaStreamDestroy(stream);

    std::cout << "Inference completed successfully!" << std::endl;
    return 0;
}