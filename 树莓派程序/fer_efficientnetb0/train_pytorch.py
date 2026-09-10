import torch
import torch.nn as nn
import numpy as np
import torch.optim as optim
from torch.nn import functional as F
import os
import time
import random


# sys.path.append('./')

# 控制seed
def seed_torch(seed=3407):
    random.seed(seed)
    np.random.seed(seed)
    torch.manual_seed(seed)
    torch.cuda.manual_seed(seed)
    torch.cuda.manual_seed_all(seed)
    torch.backends.cudnn.benchmark = False  # 关闭自动选择最快算法的选项
    torch.backends.cudnn.deterministic = True  # 让 CuDNN 以确定性的方式执行


seed_torch()

# 执行控制seed种子之后再倒入代码包
# from code import data_input
# from model import ResNet18
# from code.model import ResNet18_children

import data_input
from model import EfficientNetB0


def cre_prolog():
    # 获取当前脚本的绝对路径  
    current_script_path = os.path.abspath(__file__)  # train.py
    # 获取当前脚本的所在目录 ,即上一级目录
    parent_dir = os.path.dirname(current_script_path)  # resnet18_RAFDB
    # 获取更上一级目录（即父目录的父目录）  
    grandparent_dir = os.path.dirname(parent_dir)  # engineering

    # 定义要创建的Pro_log目录的路径（在更上一级目录中）  
    pro_log_dir_path = os.path.join(grandparent_dir, 'Pro_log')  # engineering\\Pro_log

    # 检查Pro_log目录是否存在，如果不存在则创建  
    if not os.path.exists(pro_log_dir_path):
        os.makedirs(pro_log_dir_path)
        print(f"Directory '{pro_log_dir_path}' created.")

        # 获取当前脚本的上一级目录的名称（用于新子目录的名称）
    parent_dir_name = os.path.basename(parent_dir)
    # 在Pro_log目录中创建以当前脚本的上一级目录名为名字的目录  
    new_subdir_path = os.path.join(pro_log_dir_path, parent_dir_name)  # D:\paycharm\engineering\Pro_log\res18_RAFDB
    # 检查这个新目录是否存在，如果不存在则创建  
    if not os.path.exists(new_subdir_path):
        os.makedirs(new_subdir_path)
        print(f"Directory '{new_subdir_path}' created.")
    else:
        print(f"Directory '{new_subdir_path}' already exists.")

    return new_subdir_path


# 整个训练脚本产生的结果都将存入这个脚本中，模型，log等等
exp_dir = cre_prolog()
print(f"实验目录 {exp_dir}")

device = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")
print(device)


class LSR(nn.Module):
    def __init__(self, n_classes=7, eps=0.1):
        super(LSR, self).__init__()
        self.n_classes = n_classes
        self.eps = eps

    def forward(self, outputs, labels):
        # labels.shape: [b,]
        assert outputs.size(0) == labels.size(0)
        n_classes = self.n_classes
        one_hot = F.one_hot(labels, n_classes).float()
        mask = ~(one_hot > 0)
        smooth_labels = torch.masked_fill(one_hot, mask, self.eps / (n_classes - 1))
        smooth_labels = torch.masked_fill(smooth_labels, ~mask, 1 - self.eps)
        # ce_loss = torch.sum(-smooth_labels * F.log_softmax(outputs, 1), dim=1).mean() # 标准是用这个
        ce_loss = torch.sum(-smooth_labels * F.log_softmax(outputs, 1), dim=1)
        # print(ce_loss/ce_loss.mean())
        # ce_loss = F.nll_loss(F.log_softmax(outputs, 1), labels, reduction='mean')
        return ce_loss  # 返回一维[batch_size,]向量


LSR_loss = LSR(n_classes=7, eps=0.1)

# data_dir = "/datasets/rafdb/"
data_dir = r"D:\rafdb"
# batch_size = 64
batch_size = 8
# trainset loader
train_loader, tra_num = data_input.ImageFolder_dataloader(data_dir, bs=batch_size, is_train=True)
# print(train_loader)
# validation loader
validate_loader, val_num = data_input.ImageFolder_dataloader(data_dir, bs=256, is_train=False)

# sequential = ResNet18_children(num_classes=7, device=device)
net = EfficientNetB0(num_classes=7)

# 预训练权重，用于恢复训练
pre_train_model = 'AlexNet3_vit.pth'
# save_path = './AlexNet3_vit2.pth'

is_recovery = False
if is_recovery:
    checkpoint = torch.load(pre_train_model)
    net.load_state_dict(checkpoint)
    print("-------- 模型恢复训练成功-----------")
    save_path = './pretrain_{}_.pth'.format(pre_train_model.split('.')[0])

net.to(device)
# weght_ce = torch.tensor([0.0939, 0.4310, 0.1689, 0.0254, 0.0611, 0.1718, 0.0480], dtype=torch.float32).to(device)
# weight=weght_ce
loss_function = nn.CrossEntropyLoss()
pata = list(net.parameters())  # 查看net内的参数 lr 0.0001（不是要通过优化器的参数查看吗？），按层的先后顺序，权重和偏置
optimizer = optim.Adam(net.parameters(), lr=0.0005)

# scheduler= torch.optim.lr_scheduler.StepLR(optimizer, step_size=10, gamma=0.99)
scheduler = torch.optim.lr_scheduler.CosineAnnealingLR(optimizer, T_max=20, eta_min=0, last_epoch=-1)

best_acc = 0.0

# 在代码开始运行时删除旧的日志文件
log_file = f"{exp_dir}/log.txt"
if os.path.exists(log_file):
    os.remove(log_file)
for epoch in range(60):
    # train
    net.train()  # 在训练过程中调用dropout方法
    tra_loss = 0.0
    t1 = time.perf_counter()  # 统计训练一个epoch所需时间
    # print('star')
    tra_acc = 0.0
    # torch.autograd.set_detect_anomaly(True)
    for step, data in enumerate(train_loader, start=0):
        # 这个时候的标签还是数字 不是onehot
        # print(step)
        images, labels = data
        images = images.to(device)
        labels = labels.to(device)

        optimizer.zero_grad()  # 梯度清零
        outputs = net(images, True)
        loss_list = LSR_loss(outputs, labels)
        loss = loss_list.mean()
        # 累加每个步骤的损失  
        tra_loss += loss_list.sum()
        loss.backward()
        optimizer.step()
        tra_predict_y = torch.max(outputs, dim=1)[1]  # torch.max()返回指定维度的最大值和它的索引，所以这里得到分类概率最大的种类的索引
        step_acc = (tra_predict_y == labels.to(device)).sum().item()
        tra_acc += step_acc
        # each 10 step(or batch) print once
        if (step + 1) % 10 == 0:
            print("step:{} train acc:{:.5f} train loss:{:.5f}".format(step, step_acc / len(labels), loss))

    scheduler.step()  # 调整优化器的学习率
    one_epoch_time = time.perf_counter() - t1

    # 计算并打印整个epoch的平均损失和准确率  
    tra_loss = tra_loss / tra_num
    tra_acc = tra_acc / tra_num

    # validate
    # ema.apply_shadow()
    net.eval()  # 在测试过程中关掉dropout方法，不希望在测试过程中使用dropout
    # ema.restore()
    val_acc = 0.0  # accumulate accurate number / epoch
    with torch.no_grad():

        for data_test in validate_loader:
            test_images, test_labels = data_test
            test_labels_len = len(test_labels)
            outputs = net(test_images.to(device), False)
            # out= outputs
            predict_y = torch.max(outputs, dim=1)[1]
            val_acc += (predict_y == test_labels.to(device)).sum().item()

        val_acc = val_acc / val_num
        torch.save(net.state_dict(), f"{exp_dir}/current_model.pth")
        if val_acc > best_acc:
            best_acc = val_acc
            torch.save(net.state_dict(), f"{exp_dir}/best_model.pth")
            if 0.91 < val_acc:
                torch.save(net.state_dict(), f"{exp_dir}/{val_acc:.5f}_model.pth")

        print(
            f'Epoch {epoch + 1} tra_acc:{tra_acc:.5f} tra_loss: {tra_loss:.5f} val_acc:{val_acc:.5f} best_acc: {best_acc:.5f}s time: {one_epoch_time}')
        with open(log_file, 'a') as file:
            line_to_write = f'Epoch {epoch} tra_acc {tra_acc:.5f} tra_loss {tra_loss:.5f} val_acc {val_acc:.5f} best_acc {best_acc:.5f} time {one_epoch_time:.5f}\n'
            file.write(line_to_write)

print('Finished Training')
