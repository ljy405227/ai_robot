# import onnxruntime
import cv2
import numpy as np
import os
import onnxruntime as ort
from PIL import Image
import json


def get_test_transform_opencv(image_path):
    # 使用 PIL 的 Image.open() 打开图像
    # image = Image.open(image_path)
    image = cv2.imread(image_path)
    image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
    # img_rgb = img_cv[:, :, ::-1]  # OpenCV 的通道顺序为 BGR, 转换成RGB
    # nparray
    image = Image.fromarray(np.uint8(image))

    # 使用 PIL 的 Image.resize() 调整图像大小,并指定 Image.BICUBIC 插值方法
    resized_image = image.resize((224, 224), resample=Image.BICUBIC)
    # resized_image = cv2.resize(image, (224, 224), interpolation=cv2.INTER_CUBIC)


    # 将 PIL 图像转换为 OpenCV 图像
    image = np.array(resized_image)

    # 转换图像数据类型为 float32
    image = image.astype(np.float32)

    # 归一化图像到 [0, 1] 范围
    normalized_image = image / 255.0

    # 归一化图像到 [-1, 1] 范围
    normalized_image = (normalized_image - 0.5) / 0.5

    # 转置图像维度为 (C, H, W)
    tensor_image = np.transpose(normalized_image, (2, 0, 1))

    input_data = np.expand_dims(tensor_image, axis=0)
    input_data = input_data.astype(np.float32)

    return input_data

try:
    json_file = open('./class_indices.json', 'r')
    class_indict = json.load(json_file)
except Exception as e:
    print(e)
    exit(-1)

def testset_acc_one_by_one(test_dir, onnx_dir):
    session = ort.InferenceSession(onnx_dir)
    input_name = session.get_inputs()[0].name

    total_num = 0
    total_correct = 0

    for class_name in os.listdir(test_dir):
        class_dir = os.path.join(test_dir, class_name)
        num_images = len(os.listdir(class_dir))
        correct_num = 0

        for img_name in os.listdir(class_dir):
            img_path = os.path.join(class_dir, img_name)
            opencv_img = get_test_transform_opencv(img_path)


            # 将图像数据转换为模型输入格式
            # input_data = np.expand_dims(opencv_img, axis=0)
            # input_data = input_data.astype(np.float32)

            # 运行ONNX模型进行预测
            outputs = session.run(None, {input_name: opencv_img})
            predicted_label = np.argmax(outputs[0])
            true_label = class_indict[str(predicted_label)]

            # 检查预测标签是否与真实标签匹配
            if true_label == class_name:
                correct_num += 1

        accuracy = correct_num / num_images
        print(f"Class: {class_name}, Number of Images: {num_images}, Accuracy: {accuracy:.4f}")

        total_num += num_images
        total_correct += correct_num

    overall_accuracy = total_correct / total_num
    print(f"Overall Accuracy: {overall_accuracy:.4f}")


# import time
session = ort.InferenceSession('./fer.onnx')

input_name = session.get_inputs()[0].name

def test_single(image_path,onnx_dir='./fer.onnx'):
    # import time
    session = ort.InferenceSession(onnx_dir)

    input_name = session.get_inputs()[0].name
    # t1 = time.perf_counter()
    opencv_img = get_test_transform_opencv(image_path)

    # 运行ONNX模型进行预测
    outputs = session.run(None, {input_name: opencv_img})
    predicted_label = np.argmax(outputs[0])
    true_label = class_indict[str(predicted_label)]
    return true_label


def get_frame(frame):
    # 使用 PIL 的 Image.open() 打开图像
    # image = Image.open(image_path)
    # image = cv2.imread(frame)
    image = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    # img_rgb = img_cv[:, :, ::-1]  # OpenCV 的通道顺序为 BGR, 转换成RGB
    # nparray
    image = Image.fromarray(np.uint8(image))

    # 使用 PIL 的 Image.resize() 调整图像大小,并指定 Image.BICUBIC 插值方法
    resized_image = image.resize((224, 224), resample=Image.BICUBIC)
    # resized_image = cv2.resize(image, (224, 224), interpolation=cv2.INTER_CUBIC)


    # 将 PIL 图像转换为 OpenCV 图像
    image = np.array(resized_image)

    # 转换图像数据类型为 float32
    image = image.astype(np.float32)

    # 归一化图像到 [0, 1] 范围
    normalized_image = image / 255.0

    # 归一化图像到 [-1, 1] 范围
    normalized_image = (normalized_image - 0.5) / 0.5

    # 转置图像维度为 (C, H, W)
    tensor_image = np.transpose(normalized_image, (2, 0, 1))

    input_data = np.expand_dims(tensor_image, axis=0)
    input_data = input_data.astype(np.float32)

    return input_data

def softmax(z):
    # 计算指数
    exp_z = np.exp(z)
    # 计算softmax
    softmax_output = exp_z / np.sum(exp_z)
    
    return softmax_output

def fer_frame(frame):
    opencv_img = get_frame(frame)
    # 运行ONNX模型进行预测
    outputs = session.run(None, {input_name: opencv_img})
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

# if __name__ == '__main__':

#     true_label = test_single('test_0037_aligned.jpg')
#     print(true_label)
#     # testset_acc_one_by_one('/data02/datasets/rafdb/test','./fer.onnx')