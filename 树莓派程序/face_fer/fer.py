import cv2
import numpy as np
from PIL import Image, ImageDraw, ImageFont
import fer_onnx
import face_demo

# 其他函数定义（如 generate_priors 和 hard_nms）

landmark_color = [
    (255, 0, 0),   # right eye
    (0, 0, 255),   # left eye
    (0, 255, 0),   # nose tip
    (255, 0, 255), # right mouth corner
    (0, 255, 255)  # left mouth corner
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

def cv2AddChineseText(img, text, position, textColor=(0, 255, 0), textSize=30):
    if isinstance(img, np.ndarray):
        img = Image.fromarray(cv2.cvtColor(img, cv2.COLOR_BGR2RGB))
    draw = ImageDraw.Draw(img)
    fontStyle = ImageFont.truetype("simsun.ttc", textSize, encoding="utf-8")
    draw.text(position, text, textColor, font=fontStyle)
    return cv2.cvtColor(np.asarray(img), cv2.COLOR_RGB2BGR)

def main():
    # 视频读取定义
    URL = "http://192.168.137.128:8081"
    # camera = cv2.VideoCapture(URL)
    camera = cv2.VideoCapture(0)
    camera.set(cv2.CAP_PROP_FRAME_WIDTH, 1920)
    camera.set(cv2.CAP_PROP_FRAME_HEIGHT, 1920)

    n = 0
    tm = cv2.TickMeter()

    while True:
        ret, frame = camera.read()
        n += 1
        tm.start()
        face_results = face_demo.detect_frame(frame)
        tm.stop()
        fps = tm.getFPS()

        if fps is not None:
            cv2.putText(frame, 'FPS: {:.2f}'.format(fps), (0, 15), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255))

        for det in face_results:
            bbox = det[0:4].astype(np.int32)
            cv2.rectangle(frame, (bbox[0], bbox[1]), (bbox[0] + bbox[2], bbox[1] + bbox[3]), (0, 255, 0), 3)

            if is_mark:
                landmarks = det[4:14].astype(np.int32).reshape((5, 2))
                for idx, landmark in enumerate(landmarks):
                    cv2.circle(frame, landmark, 2, landmark_color[idx], 2)

            face = frame[bbox[1]-20:bbox[1]+bbox[3]-10, bbox[0]-10:bbox[0]+bbox[2]+10]
            if face.shape[0] == 0 or face.shape[1] == 0:
                continue

            fer_result, prob = fer_onnx.fer_frame(face)

            tl = round(0.002 * (frame.shape[0] + frame.shape[1]) / 2) + 1
            tf = max(tl - 1, 1)

            if show_chinese:
                frame = cv2AddChineseText(frame, '{} {}'.format(emotion_mapping[fer_result], str(prob)), (bbox[0], bbox[1]-26), (255, 0, 0), 25)
                
            else:
                cv2.putText(frame, '{} {}'.format(fer_result, str(prob)), (bbox[0]-5, bbox[1]-12), 0, tl / 3, [0, 0, 255], thickness=tf, lineType=cv2.LINE_AA)

        cv2.imshow("Facial Expression Recognition", frame)
        tm.reset()

        if cv2.waitKey(1) == ord('s'):
            cv2.imwrite("{}.png".format(n), frame)
            print("已保存当前图像")
        elif cv2.waitKey(1) == ord('q'):
            break

    camera.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
