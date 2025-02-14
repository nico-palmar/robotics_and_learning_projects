#include <NvInfer.h>
#include <NvOnnxParser.h>
#include <iostream>
#include <fstream>
#include <filesystem>

class Logger : public nvinfer1::ILogger {
public:
    void log(nvinfer1::ILogger::Severity severity, const char* msg) noexcept override {
        if (severity != nvinfer1::ILogger::Severity::kVERBOSE) {
            std::cout << msg << std::endl;
        }
    }
};

int main()
{
    Logger logger;
    nvinfer1::IBuilder* builder = nvinfer1::createInferBuilder(logger);
    // Explicit batch network
    // const auto explicitBatch = 1U << static_cast<uint32_t>(nvinfer1::NetworkDefinitionCreationFlag::kEXPLICIT_BATCH);
    // nvinfer1::INetworkDefinition* network = builder->createNetworkV2(explicitBatch);
    nvinfer1::INetworkDefinition* network = builder->createNetworkV2(0);
    nvonnxparser::IParser* parser = nvonnxparser::createParser(*network, logger);

    // Load the ONNX model
    std::string model_file = "../best.onnx";
    // std::cout << "Test building 2" << std::endl;

    std::cout << "File is: " << model_file.c_str() << std::endl;

    if (std::filesystem::exists(model_file)) {
        std::cout << "File exists: " << model_file << std::endl;
    } else {
        std::cout << "File not found: " << model_file << std::endl;
    }

    if (!parser->parseFromFile(model_file.c_str(), static_cast<int>(nvinfer1::ILogger::Severity::kVERBOSE))) {
        for (int i = 0; i < parser->getNbErrors(); ++i)
        {
            std::cout << parser->getError(i)->desc() << std::endl;
        }
        std::cerr << "Failed to parse ONNX model!" << std::endl;
        return 0;
    }

    std::cout << "Successfully loaded onnx model" << std::endl;

    // Set up the builder configuration and build the engine
    nvinfer1::IBuilderConfig* config = builder->createBuilderConfig();
    // TODO: FIGURE OUT WHAT VALUES TO SET THESE MEMORY POOLS TO
    // DICATE WHETHER OR NOT FAILURES HAPPEN WHEN SERIALIZING THE MODEL
    config->setMemoryPoolLimit(nvinfer1::MemoryPoolType::kWORKSPACE, 1U << 30);
    config->setMemoryPoolLimit(nvinfer1::MemoryPoolType::kTACTIC_SHARED_MEMORY, 48 << 10);
    config->setFlag(nvinfer1::BuilderFlag::kFP16);  // Use FP16 precision for faster inference

    // Optimize for batch size 1
    // nvinfer1::IOptimizationProfile* profile = builder->createOptimizationProfile();
    // profile->setDimensions("input_tensor", nvinfer1::OptProfileSelector::kMIN, nvinfer1::Dims4(1, 3, 224, 224));
    // profile->setDimensions("input_tensor", nvinfer1::OptProfileSelector::kOPT, nvinfer1::Dims4(1, 3, 224, 224));
    // profile->setDimensions("input_tensor", nvinfer1::OptProfileSelector::kMAX, nvinfer1::Dims4(1, 3, 224, 224));
    // config->addOptimizationProfile(profile);

    // nvinfer1::ICudaEngine* engine = builder->buildEngineWithConfig(*network, *config);

    // std::cout << "Successfully created engine file" << std::endl;

    // // Serialize the engine to a file
    // nvinfer1::IHostMemory* serializedEngine = engine->serialize();
    nvinfer1::IHostMemory* serializedModel = builder->buildSerializedNetwork(*network, *config);

    // TODO: make smart pointers to avoid this
    delete parser;
    delete network;
    delete config;
    delete builder;

    std::cout << "Engine serialized" << std::endl;
    std::ofstream engineFile("../best.engine", std::ios::binary);
    std::cout << "Creating engine file to write to" << std::endl;
    engineFile.write(reinterpret_cast<char*>(serializedModel->data()), serializedModel->size());

    // TODO: make smart pointers to avoid this
    delete serializedModel;

    return 0;
}
