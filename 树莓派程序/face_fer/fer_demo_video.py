

# import os

# import torch
import cv2
import numpy as np
from PIL import Image, ImageDraw, ImageFont

# 导入人脸表情识别函数库
import fer_onnx
# 导入人脸检测库
import face_demo



def generate_priors(feature_map_list, shrinkage_list, image_size, min_boxes):
    priors = []
    for index in range(0, len(feature_map_list[0])):
        scale_w = image_size[0] / shrinkage_list[0][index]
        scale_h = image_size[1] / shrinkage_list[1][index]
        for j in range(0, feature_map_list[1][index]):
            for i in range(0, feature_map_list[0][index]):
                x_center = (i + 0.5) / scale_w
                y_center = (j + 0.5) / scale_h

                for min_box in min_boxes[index]:
                    w = min_box / image_size[0]
                    h = min_box / image_size[1]
                    priors.append([
                        x_center,
                        y_center,
                        w,
                        h
                    ])
    print("priors nums:{}".format(len(priors)))
    return np.clip(priors, 0.0, 1.0)


def hard_nms(box_scores, iou_threshold, top_k=-1, candidate_size=200):
    scores = box_scores[:, -1]
    boxes = box_scores[:, :-1]
    picked = []
    indexes = np.argsort(scores)
    indexes = indexes[-candidate_size:]
    while len(indexes) > 0:
        current = indexes[-1]
        picked.append(current)
        if 0 < top_k == len(picked) or len(indexes) == 1:
            break
        current_box = boxes[current, :]
        indexes = indexes[:-1]
        rest_boxes = boxes[indexes, :]
        iou = iou_of(
            rest_boxes,
            np.expand_dims(current_box, axis=0),
        )
        indexes = indexes[iou <= iou_threshold]
    return box_scores[picked, :]


# def area_of(left_top, right_bottom):
#     hw = np.clip(right_bottom - left_top, 0.0, None)
#     return hw[..., 0] * hw[..., 1]



# def normalize(image,mean=[0.5,0.5,0.5],std=[0.5,0.5,0.5]):
#     image=image.transpose(2,0,1)/255 #由（H,W,C）变为（C,H,W）,并进行归一化
#     image=torch.from_numpy(image)
#     mean = torch.as_tensor(mean)
#     std = torch.as_tensor(std)
#     if mean.ndim == 1:
#         mean = mean.view(-1, 1, 1)
#     if std.ndim == 1:
#         std = std.view(-1, 1, 1)
#     image=(image-mean)/std #根据image=(x-mean)/std进行标准化计算

#     return image


# net = dnn.readNetFromONNX(onnx_path)  # onnx version
# # net = dnn.readNetFromCaffe(caffe_prototxt_path, caffe_model_path)  # caffe model converted from onnx
# input_size = [320,240]
# witdh = input_size[0]
# height = input_size[1]
# priors = define_img_size(input_size)

landmark_color = [
        (255,   0,   0), # right eye
        (  0,   0, 255), # left eye
        (  0, 255,   0), # nose tip
        (255,   0, 255), # right mouth corner
        (  0, 255, 255)  # left mouth corner
    ]

# 定义英文到中文表情名称的映射字典
emotion_mapping = {
    "Anger": "愤怒",
    "Disgust": "厌恶",
    "Fear": "恐惧",
    "Happiness": "开心",
    "Neutral": "平和",
    "Sadness": "悲伤",
    "Surprise": "惊讶"
}

# 是否显示中文
show_chinese = True
# 是否显示关键点
is_mark = True
#视频读取定义
camera = cv2.VideoCapture(0)
# cv2.namedWindow("frame", cv2.WINDOW_NORMAL)
# cv2.setWindowProperty("frame", cv2.WND_PROP_FULLSCREEN, cv2.WND_PROP_FULLSCREEN)
camera.set(cv2.CAP_PROP_FRAME_WIDTH, 1376)
camera.set(cv2.CAP_PROP_FRAME_HEIGHT, 960)



tm = cv2.TickMeter()

def cv2AddChineseText(img, text, position, textColor=(0, 255, 0), textSize=30):
    if (isinstance(img, np.ndarray)):  # 判断是否OpenCV图片类型
        img = Image.fromarray(cv2.cvtColor(img, cv2.COLOR_BGR2RGB))
    # 创建一个可以在给定图像上绘图的对象
    draw = ImageDraw.Draw(img)
    # 字体的格式
    fontStyle = ImageFont.truetype(
        "simsun.ttc", textSize, encoding="utf-8")
    # 绘制文本
    draw.text(position, text, textColor, font=fontStyle)
    # 转换回OpenCV格式
    return cv2.cvtColor(np.asarray(img), cv2.COLOR_RGB2BGR)
n = 0
while True:
# 读取当前帧
    ret, frame = camera.read()
    n = n+1
    # 人脸检测计时器
    tm.start()
    face_results = face_demo.detect_frame(frame)
    tm.stop()
    # 计算人脸检测器的帧率
    fps = tm.getFPS()
    # 显示帧率
    if fps is not None:
        cv2.putText(frame, 'FPS: {:.2f}'.format(fps), (0, 15), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255))

    # 每一个人脸循环识别表情
    for det in face_results:
        # 获得人脸坐标框
        bbox = det[0:4].astype(np.int32)
        # 参数表示依次为:(图片，长方形框左上角坐标(x0,y0),长方形框右下角坐标(x0+w, y0+h)，字体颜色，字体粗细)
        # 绘制人脸框
        cv2.rectangle(frame, (bbox[0], bbox[1]), (bbox[0]+bbox[2], bbox[1]+bbox[3]), (0, 255, 0), 3)

        # 人脸检测的置信度
        conf = det[-1]
        # cv.putText(frame, '{:.4f}'.format(conf), (bbox[0], bbox[1]+12), cv.FONT_HERSHEY_DUPLEX, 0.5, text_color)

        # 是否显示关键点
        # print(is_mark)
        if is_mark:
            landmarks = det[4:14].astype(np.int32).reshape((5,2))
            for idx, landmark in enumerate(landmarks):
                cv2.circle(frame, landmark, 2, landmark_color[idx], 2)

        # 取出人脸图像
        face = frame[bbox[1]-20:bbox[1]+bbox[3]-10,bbox[0]-10:bbox[0]+bbox[2]+10]
        if face.shape[0] == 0 or face.shape[1] == 0:
            continue
        # 人脸表情识别
        fer_result, prob = fer_onnx.fer_frame(face)
        # print(fer_result)
        # Facial expression recognition 
        tl = round(0.002 * (frame.shape[0] + frame.shape[1]) / 2) + 1
        tf = max(tl - 1, 1)
        # 显示中文
        if show_chinese:
            # frame=cv2AddChineseText(frame,emotion_mapping[fer_result], (bbox[0], bbox[1]-26),(255, 0, 0), 25)
            frame=cv2AddChineseText(frame,'{} {}'.format(emotion_mapping[fer_result], str(prob)), (bbox[0], bbox[1]-26),(255, 0, 0), 25)

        else:
            cv2.putText(frame, '{} {}'.format(fer_result, str(prob)), (bbox[0]-5, bbox[1]-12), 0, tl / 3, [0, 0, 255], thickness=tf,
                    lineType=cv2.LINE_AA)
        # cv2.rectangle(frame, (bbox[0]-25, bbox[1]-20), (bbox[2]+20, bbox[3]-10), (0, 255, 0), 2)
        
    cv2.imshow("Facial Expression Recognition", frame)
    # cv2.imshow("face", face)
    tm.reset()
    if cv2.waitKey(1) == ord('s'): # 按's'键保存
        cv2.imwrite("{}.png".format(n),frame)
        print("已保存当前图像")
            
    elif cv2.waitKey(1) == ord('q'): # 按'q'键推出
            break

camera.release()
cv2.destroyAllWindows()

