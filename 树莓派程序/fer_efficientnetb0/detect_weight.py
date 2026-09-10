import cv2
import numpy as np
from typing import Union
# from EfficientNetB0.test_onnx import get_frame, onnx_session, input_name, softmax, class_indict
from test_onnx import get_frame, onnx_session, input_name, softmax, class_indict


def detect(frame: Union[cv2.typing.MatLike, cv2.UMat, str]):
    opencv_img = get_frame(frame)
    # 运行ONNX模型进行预测
    outputs = onnx_session.run(None, {input_name: opencv_img})
    probabilities = softmax(outputs)
    max_probability = np.max(probabilities)
    predicted_label = np.argmax(outputs[0])
    # print(class_indict)
    true_label = class_indict[str(predicted_label)]
    # print(probabilities, max_probability)
    # 格式化max_probability为四位小数
    formatted_max_probability = round(max_probability, 4)
    # print(formatted_max_probability)
    return true_label, formatted_max_probability
