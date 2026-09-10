import torch
# from model import AlexNet
from PIL import Image
from torchvision import transforms
import json, os
import data_input
# from code import data_input
# import input_data
from model import ResNet18_children

# 标签，0:Surprise, 1:Fear, 2:Disgust, 3:Happiness, 4:Sadness, 5:Anger, 6:Neutral
# ferplus 

# read class_indict
try:
    json_file = open('class_indices.json', 'r')
    class_indict = json.load(json_file)
except Exception as e:
    print(e)
    exit(-1)

data_transform = transforms.Compose(
    [transforms.Resize((224, 224)),
     transforms.ToTensor(),
     transforms.Normalize((0.5, 0.5, 0.5), (0.5, 0.5, 0.5))])


def get_model():
    model = ResNet18_children()
    # load model weights
    model_weight_path = "../../Pro_log/res18_RAFDB/best_model.pth"
    model.load_state_dict(torch.load(model_weight_path))
    model.eval()
    return model


def get_inputs(img_path):
    image = Image.open(img_path)
    inputs = data_transform(image)
    inputs = torch.unsqueeze(inputs, dim=0)  # 单张图片，批次维度为1
    # print(type(input))
    return inputs


def predict(model, inputs):
    with torch.no_grad():
        # output = torch.squeeze(model(img))
        outputs = model(inputs)
        index = outputs.max(1).indices.item()  # 单样本预测类别索引
        # print(index)

    return index


# 预测一张图像
def main(model, img_path='./1.png'):
    inputs = get_inputs(img_path)
    index = predict(model, inputs)
    return index, class_indict[str(index)]  # 根据索引-类别的json文件得到类别


# test_dir = 'G:/dataset/RAFDB/basic/Image/test/'

def pre_write_txt(pred, file):
    for i in pred:
        f = open(file, 'a', encoding='utf-8_sig')
        f.write(str(i) + ',')
        f.close()
    print("-----------------预测结果已经写入文本文件--------------------")


# 测试整个测试集，通采用data loder测试
validate_loader, val_num = data_input.val_data(root_dir=r"D:\paycharm\engineering\res18_RAFDB\dataset\rafdb",
                                               batch_size=256)


def test_acc(model):
    acc = 0.0
    device = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")
    model.to(device)
    batch_index = 0
    all_pred = []
    all_label = []
    with torch.no_grad():
        for data_test in validate_loader:
            batch_index += 1
            test_images, test_labels = data_test
            test_labels_len = len(test_labels)
            # print(test_labels_len)    
            outputs = model(test_images.to(device))
            predict_y = torch.max(outputs, dim=1)[1]
            val_acc = (predict_y == test_labels.to(device)).sum().item()
            acc += val_acc
            print("第 {} 个test batch acc:{:.4f}".format(batch_index, val_acc / test_labels_len))

            # 保存当前 batch 的预测值和真实标签
            all_pred.extend(predict_y.cpu().numpy().tolist())
            all_label.extend(test_labels.numpy().tolist())

    accurate_test = acc / val_num
    print("\n测试集一共 {} 图像, 共测试了{}个 batch, predict acc : {}".format(val_num, batch_index, accurate_test))
    # 将所有的预测值和真实标签写入到文件中
    with open("pre_labels.txt", "w") as f:
        for pred, label in zip(all_pred, all_label):
            f.write(f"{pred} {label}\n")


# # 测试某一个类别
def class_acc(class_name):
    q = 0
    test_dir = os.path.join("../dataset/rafdb/test", class_name)
    imgs = os.listdir(test_dir)
    for img in imgs:
        img_path = test_dir + "/" + img
        index, true_label = main(model=model, img_path=img_path)
        if index == 3:
            q = q + 1

    print("\n predict acc : {}".format(q / len(imgs)))


def testset_acc_one_by_one(test_dir='D:/paycharm/engineering/res18_RAFDB/dataset/rafdb/test'):
    class_name = os.listdir(test_dir)  # test里的类别名

    # new_class_indict = dict((value,key) for key,value in class_indict.items())
    # print(class_indict)
    # print(new_class_indict)
    sum = 0
    model = get_model()
    for dir in class_name:
        right_num = 0
        # true_label = new_class_indict[dir]

        class_name = test_dir + dir + '/'  # test的类别名就绝对路径
        num_dir = len(os.listdir(class_name))  # test某类别的数据集大小
        print("class_name:", class_name)
        print("num_dir", num_dir)
        # print(dir,true_label)
        for img in os.listdir(class_name):
            lable_num, true_label = main(model, class_name + img)
            if true_label == dir:
                right_num += 1
        sum = sum + right_num
        print(class_name, num_dir, right_num)
    print(sum)


if __name__ == '__main__':
    # 测试单张图像
    model = get_model()
    # lable_num, true_label = main(model,img_path = "../tulips.jpg")
    # print(lable_num, true_label)
    class_acc('Happiness')
    # test_acc(model)
    # testset_acc_one_by_one()
