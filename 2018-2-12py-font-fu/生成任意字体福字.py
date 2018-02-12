# 调用cmd
import os
# 处理字符串
import re
# 文字转化为图片
from PIL import Image, ImageFont, ImageDraw

# 列出所有的字体文件，主要是用ttf（TrueType字体)文件
return_cmd = os.popen("C: & cd / & cd C:\Windows\Fonts\ & tree /f")
fonts = return_cmd.read()
print(fonts)

# 只保留ttf部分
pattern = re.compile('\S.*?ttf', re.I)
result_list = pattern.findall(fonts)
# 去掉中文名称的字体
result_list = result_list[1:-9]
print(result_list)

# 文字处理
text = "福"

# 指定图片保存文件夹
path_img = 'D:\\编程\\py-font-fu\\img\\'
if os.path.isdir(path_img):
    pass
else:
    os.mkdir(path_img)

# 绘制
for index in range(len(result_list)):
    # 新建一个300*300的空白图像
    img = Image.new("RGB", (300, 300), (255, 255, 255))
    # 创建画笔
    drawer = ImageDraw.Draw(img)
    # 字体路径
    path_font = 'C:\\Windows\\Fonts\\' + result_list[index]
    font = ImageFont.truetype(path_font, 200)
    drawer.text((10, 10), text=text, font=font, fill=(0, 0, 0))
    img.save(path_img + result_list[index][0:-3] + 'png')
