import torch
from torchvision import transforms, datasets, utils
import json, os
from PIL import Image
from torch.utils.data import DataLoader, Dataset
import numpy as np
import random
# import kaiwang_image_utils
import pandas as pd


# 为 DataLoader 的每个工作线程设置随机种子
def worker_init_fn(worker_id):
    worker_seed = torch.initial_seed() % 2 ** 32
    np.random.seed(worker_seed)
    random.seed(worker_seed)


# 使用 torch.Generator 确保数据加载顺序一致
g = torch.Generator()
g.manual_seed(3407)
torch.random.manual_seed(g.initial_seed())

# 数据预处理，定义data_transform这个字典
data_transform = {
    "train": transforms.Compose([transforms.Resize((224, 224), interpolation=Image.BICUBIC),  # 随机裁剪，裁剪到224*224
                                 transforms.RandomHorizontalFlip(),  # 水平方向随机翻转
                                 transforms.ToTensor(),  # [-0,1]
                                 transforms.Normalize((0.5, 0.5, 0.5), (0.5, 0.5, 0.5))]),  # -1, 1
    "test": transforms.Compose(
        [transforms.Resize((224, 224), interpolation=Image.BICUBIC),  # cannot 224, must (224, 224)
         transforms.ToTensor(),
         transforms.Normalize((0.5, 0.5, 0.5), (0.5, 0.5, 0.5))])}


def train_data(root_dir, batch_size):
    # train_dataset = datasets.ImageFolder(root=root_dir + "train",transform=data_transform["train"])
    train_dataset = FERDataset(dataset_path="/data/coding/dataset/rafdb", is_train=True)
    train_data_num = len(train_dataset)
    # print(train_dataset.classes)
    flower_list = train_dataset.class_to_index_dict  # 获取分类的名称所对应的索引，即{'daisy':0, 'dandelion':1, 'roses':2, 'sunflower':3, 'tulips':4}
    cla_dict = dict((val, key) for key, val in flower_list.items())  # 遍历获得的字典，将key和value反过来，即key变为0，val变为daisy
    # 将key和value反过来的目的是，预测之后返回的索引可以直接通过字典得到所属类别
    # write dict into json file
    json_str = json.dumps(cla_dict, indent=4)
    with open('class_indices.json', 'w') as json_file:  # 保存入json文件
        json_file.write(json_str)
    train_loader = torch.utils.data.DataLoader(train_dataset,
                                               batch_size=batch_size, shuffle=True,
                                               num_workers=8)  # 8
    return train_loader, train_data_num


def val_data(root_dir, batch_size):
    validate_dataset = datasets.ImageFolder(root=root_dir + "test", transform=data_transform["test"])
    val_data_num = len(validate_dataset)
    validate_loader = torch.utils.data.DataLoader(validate_dataset,
                                                  batch_size=batch_size, shuffle=False,
                                                  num_workers=8)  # 8
    return validate_loader, val_data_num


def ImageFolder_dataloader(root_dir, bs, is_train=True):
    # 根据是否训练模式来切换数据集路径和数据增强模式
    # root = root_dir + ("train" if is_train else "test")
    # transform = data_transform["train"] if is_train else data_transform["test"]
    # ImageFolder 获取数据
    ImageFolder_dataset = FERDataset(dataset_path=root_dir, is_train=is_train)
    data_len = len(ImageFolder_dataset)

    class_index = ImageFolder_dataset.class_to_index_dict  # 获取分类的名称所对应的索引，即{'daisy':0, 'dandelion':1, 'roses':2, 'sunflower':3, 'tulips':4}
    index_class = dict((val, key) for key, val in class_index.items())  # 遍历获得的字典，将key和value反过来，即key变为0，val变为daisy
    # 将key和value反过来的目的是，预测之后返回的索引可以直接通过字典得到所属类别
    # write dict into json file
    json_str = json.dumps(index_class, indent=4)
    with open('class_indices.json', 'w') as json_file:  # 保存入json文件
        json_file.write(json_str)
    data_loader = torch.utils.data.DataLoader(ImageFolder_dataset,
                                              batch_size=bs,
                                              shuffle=True if is_train else False,
                                              num_workers=0,
                                              worker_init_fn=worker_init_fn, generator=g
                                              )
    return data_loader, data_len


# 未分类数据集
# class FERDataset(Dataset):
#     def __init__(self, dataset_path, is_train):
#         super(FERDataset, self).__init__()
#         self.dataset_path = dataset_path
#         self.is_train = is_train

#         NAME_COLUMN = 0
#         LABEL_COLUMN = 1
#         df = pd.read_csv(os.path.join(self.dataset_path, 'list_patition_label.txt'), sep=' ', header=None)
#         if is_train:
#             dataset = df[df[NAME_COLUMN].str.startswith('train')]
#         else:
#             dataset = df[df[NAME_COLUMN].str.startswith('test')]
#         file_names = dataset.iloc[:, NAME_COLUMN].values
#         self.label = dataset.iloc[:,
#                      LABEL_COLUMN].values - 1  # 0:Surprise, 1:Fear, 2:Disgust, 3:Happiness, 4:Sadness, 5:Anger, 6:Neutral

#         # 类别索引映射关系
#         self.class_to_index_dict = { "Surprise":     0,
#                                 "Fear":   1,
#                                 "Disgust":      2,
#                                 "Happiness": 3,
#                                 "Sadness":   4,
#                                 "Anger":   5,
#                                 "Neutral":  6}
#         # 初始化数据集文件路径
#         self.images_list = []
#         self.labels_list = list(self.label)
#         for f in file_names:
#             f = f.split(".")[0]
#             f = f + "_aligned.jpg"
#             path = os.path.join(self.dataset_path, 'aligned', f)
#             self.images_list.append(path)

#         self.img_trans = self.img_transformer()

#     def __len__(self):
#         return len(self.images_list)

#     def img_transformer(self):
#             return {
#             "train": transforms.Compose([
#                 #transforms.Resize((100, 100), interpolation=Image.BICUBIC),  # InterpolationMode.BICUBIC
#                 transforms.Resize((224, 224), interpolation=Image.BICUBIC),
#                 transforms.ToTensor(),
#                 transforms.Normalize((0.5, 0.5, 0.5), (0.5, 0.5, 0.5))
#             ]),
#             "test": transforms.Compose([
#                 # transforms.Resize((100//8, 100//8)),
#                 transforms.Resize((224, 224), interpolation=Image.BICUBIC),
#                 # transforms.RandomApply([
#                 # transforms.ColorJitter(0.4, 0.4, 0.4, 0.1)
#                 # ], p=0.8),
#                 # transforms.RandomGrayscale(p=0.2),
#                 transforms.ToTensor(),
#                 transforms.Normalize((0.5, 0.5, 0.5), (0.5, 0.5, 0.5))
#             ]),
#         }

#     def __getitem__(self, index):
#         # 读取原图像
#         ori_img = Image.open(self.images_list[index]).convert('RGB')
#         trans_img = self.img_trans["train"](ori_img) if self.is_train else self.img_trans["test"](ori_img)

#         return trans_img, self.labels_list[index]


# 已分类数据集
class FERDataset(Dataset):
    def __init__(self, dataset_path, is_train):
        super(FERDataset, self).__init__()
        self.dataset_path = dataset_path
        self.is_train = is_train
        self.root_dir = os.path.join(self.dataset_path, "train" if self.is_train else "test")
        # 类别索引映射关系
        self.class_to_index_dict = {"Anger": 0,
                                    "Disgust": 1,
                                    "Fear": 2,
                                    "Happiness": 3,
                                    "Neutral": 4,
                                    "Sadness": 5,
                                    "Surprise": 6}
        # 初始化数据集文件路径
        self.images_list = []
        self.labels_list = []
        for class_name in os.listdir(self.root_dir):
            class_dir = os.path.join(self.root_dir, class_name)
            # 获取该类别的索引
            class_id = self.class_to_index_dict[class_name]
            # print(type(class_id))
            # 遍历各类别
            for image_name in os.listdir(class_dir):
                # 获取图像路径
                image_path = os.path.join(class_dir, image_name)
                # 添加到列表中
                self.images_list.append(image_path)
                # 同时添加类别
                self.labels_list.append(class_id)

        self.img_trans = self.img_transformer()

    def __len__(self):
        return len(self.images_list)

    def img_transformer(self):
        return {
            "train": transforms.Compose([
                # transforms.Resize((100, 100), interpolation=Image.BICUBIC),  # InterpolationMode.BICUBIC
                transforms.Resize((224, 224), interpolation=Image.BICUBIC),
                transforms.ToTensor(),
                transforms.Normalize((0.5, 0.5, 0.5), (0.5, 0.5, 0.5))
            ]),
            "test": transforms.Compose([
                # transforms.Resize((100//8, 100//8)),
                transforms.Resize((224, 224), interpolation=Image.BICUBIC),
                # transforms.RandomApply([
                # transforms.ColorJitter(0.4, 0.4, 0.4, 0.1)
                # ], p=0.8),
                # transforms.RandomGrayscale(p=0.2),
                transforms.ToTensor(),
                transforms.Normalize((0.5, 0.5, 0.5), (0.5, 0.5, 0.5))
            ]),
        }

    def __getitem__(self, index):
        # 读取原图像
        ori_img = Image.open(self.images_list[index]).convert('RGB')
        trans_img = self.img_trans["train"](ori_img) if self.is_train else self.img_trans["test"](ori_img)

        return trans_img, self.labels_list[index]


def Gen_DataLoader(data_path, is_train, bs=8):
    gen_dataset = FERDataset(dataset_path=data_path, is_train=is_train)
    data_len = gen_dataset.__len__()
    # print(gen_dataset.__len__())
    data_loader = torch.utils.data.DataLoader(
        gen_dataset,
        batch_size=bs,
        shuffle=True if is_train else False,
        num_workers=0,
        drop_last=False)
    return data_loader, data_len


if __name__ == "__main__":

    # data_dir = "/datasets/rafdb/"
    data_dir = r'/data/coding/rafdb/'
    batch_size = 2

    # Old ImageFolder 方式
    # validate_loader,val_data_num = val_data(data_dir,batch_size)
    # print("Total data:", val_data_num)
    # for data_batch in validate_loader:
    #          datas, labels = data_batch
    #          labels_len = len(labels)
    #          print(datas.size(), labels_len, type(labels))

    # ImageFolder 方式
    # data_loader, data_len = ImageFolder_dataloader(root_dir = data_dir, bs = batch_size, is_train=False)
    # print("Total data:", data_len)
    # for data_batch in data_loader:
    #          datas, labels = data_batch
    #          labels_len = len(labels)
    #          # print(datas.size(), labels_len, type(labels))

    # # Dataset
    data_loader, data_len = Gen_DataLoader(data_path=data_dir, is_train=False, bs=batch_size)
    print("Total data:", data_len)
    for step, data_batch in enumerate(data_loader, start=0):
        datas, labels = data_batch
        labels_len = len(labels)
        print(datas.size(), labels_len, type(labels))
