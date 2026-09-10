import torch
from model import EfficientNetB0

# 设置设备
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")

# 创建模型实例
model = EfficientNetB0(num_classes=7)

# 加载训练好的模型权重
model.load_state_dict(torch.load("fer.pth", map_location=device))
model.to(device)
model.eval()

# 定义输入形状 (batch_size, channels, height, width)
batch_size = 1
input_shape = (3, 224, 224)

# 创建示例输入张量
dummy_input = torch.randn(batch_size, *input_shape, device=device)

# 导出ONNX模型
onnx_file_path = "fer.onnx"

torch.onnx.export(
    model,
    dummy_input,
    onnx_file_path,
    export_params=True,
    opset_version=11,
    do_constant_folding=True,
    input_names=['input'],
    output_names=['output'],
    dynamic_axes={
        'input': {0: 'batch_size'},
        'output': {0: 'batch_size'}
    }
)

print(f"模型已成功导出为ONNX格式: {onnx_file_path}")
