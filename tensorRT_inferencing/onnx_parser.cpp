#include <NvInfer.h>
#include <NvOnnxParser.h>
#include <iostream>
#include <fstream>

class Logger : public nvinfer1::ILogger {
public:
    void log(nvinfer1::ILogger::Severity severity, const char* msg) noexcept override {
        if (severity != nvinfer1::ILogger::Severity::kINFO) {
            std::cout << msg << std::endl;
        }
    }
};

int main()
{
    Logger logger;
    nvinfer1::IBuilder* builder = nvinfer1::createInferBuilder(logger);
    nvinfer1::INetworkDefinition* network = builder->createNetworkV2(0U);
    nvonnxparser::IParser* parser = nvonnxparser::createParser(*network, logger);

    // Load the ONNX model
    if (!parser->parseFromFile("best.onnx", static_cast<int>(nvinfer1::ILogger::Severity::kINFO))) {
        std::cerr << "Failed to parse ONNX model!" << std::endl;
        return 0;
    }

    // Set up the builder configuration and build the engine
    nvinfer1::IBuilderConfig* config = builder->createBuilderConfig();
    config->setMemoryPoolLimit(nvinfer1::MemoryPoolType::kWORKSPACE, 1U << 30); // Set a 1GB workspace
    config->setFlag(nvinfer1::BuilderFlag::kFP16);  // Use FP16 precision for faster inference

    nvinfer1::ICudaEngine* engine = builder->buildEngineWithConfig(*network, *config);

    // Serialize the engine to a file
    nvinfer1::IHostMemory* serializedEngine = engine->serialize();
    std::ofstream engineFile("best.engine", std::ios::binary);
    engineFile.write(reinterpret_cast<char*>(serializedEngine->data()), serializedEngine->size());

    return 0;
}
