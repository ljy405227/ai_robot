from PIL import Image
import os

# GIF文件路径
gif_file = 'xuanyun.gif'

# 输出文件夹
output_folder = 'out'

# 创建输出文件夹（如果不存在）
if not os.path.exists(output_folder):
    os.makedirs(output_folder)

# 打开GIF文件
gif = Image.open(gif_file)

# 获取GIF的帧数
frame_count = gif.n_frames

# 帧编号
frame_number = 1

# 读取每一帧并保存
for i in range(frame_count):
    gif.seek(i)  # 跳转到第i帧
    frame = gif.copy()  # 获取当前帧的副本
    
    # 保存帧为图片
    frame_filename = os.path.join(output_folder, f"{frame_number}.png")
    frame.save(frame_filename)
    
    frame_number += 1

print(f"截屏完成，图片已保存至 '{output_folder}' 文件夹。")
