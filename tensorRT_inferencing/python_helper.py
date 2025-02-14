import onnx

# # Load the model
model = onnx.load("best.onnx")

# print(model)

# Print inputs and outputs
print("Inputs:")
for input in model.graph.input:
    print(input)

print("\nOutputs:")
for output in model.graph.output:
    print(output)

# Verify model integrity
onnx.checker.check_model(model)
print("ONNX model is valid!")

# import torchvision.models as models
# import torch

# resnet50 = models.resnet50(pretrained=True, progress=False).eval()

# BATCH_SIZE = 32
# dummy_input=torch.randn(BATCH_SIZE, 3, 224, 224)

# torch.onnx.export(resnet50, dummy_input, "resnet50_pytorch.onnx", verbose=False)