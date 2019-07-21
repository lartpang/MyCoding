# -*- coding: utf-8 -*-
# @Time    : 2019/7/18 上午9:54
# @Author  : Lart Pang
# @FileName: cal_fps.py
# @Project : Paper_Code
# @GitHub  : https://github.com/lartpang

import os
import time

import torch
from PIL import Image
from torchvision import transforms
from tqdm import tqdm

from BaseNet.ResNet import (Backbone_ResNet18 as Backbone_ResNet_18)
from Networks.WeakNet import (FastSmallMFCNV5_B, SmallMFCNV5_B)


def check_mkdir(dir_name):
    if not os.path.exists(dir_name):
        os.makedirs(dir_name)


class FPSer():
    def __init__(self, proj_name, args, pth_path):
        super(FPSer, self).__init__()
        self.args = args
        self.to_pil = transforms.ToPILImage()
        self.proj_name = proj_name
        self.dev = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")
        backbone = self.args[proj_name]['backbone']
        self.net = self.args[proj_name]['net'](backbone).to(self.dev)
        self.net.eval()

        self.test_img_transform = transforms.Compose([
            transforms.Resize((self.args['crop_size'], self.args['crop_size'])),
            transforms.ToTensor(),
            transforms.Normalize([0.485, 0.456, 0.406], [0.229, 0.224, 0.225])
        ])

        if pth_path != None:
            print(f'导入模型...{pth_path}')
            checkpoint = torch.load(pth_path)
            model_dict = self.net.state_dict()
            # 1. filter out unnecessary keys
            pretrained_dict = {k: v for k, v in checkpoint.items()
                               if k in model_dict}
            # 2. overwrite entries in the existing state dict
            model_dict.update(pretrained_dict)
            # 3. load the new state dict
            self.net.load_state_dict(model_dict)
            print("初始化完毕...")
        else:
            print("不加载权重")

    def test(self, data_path, save_path):
        assert save_path != None

        print(f'保存路径为{save_path}')
        check_mkdir(save_path)

        print(f'开始测试...{data_path}')
        img_path = os.path.join(data_path, 'Image')
        img_list = os.listdir(img_path)

        start_time = time.time()

        tqdm_iter = tqdm(enumerate(img_list), total=len(img_list), leave=False)
        for idx, img_name in tqdm_iter:
            tqdm_iter.set_description(f"{self.proj_name}:te=>{idx + 1}")

            img_fullpath = os.path.join(img_path, img_name)
            test_data = Image.open(img_fullpath).convert('RGB')
            img_size = test_data.size
            test_data = self.test_img_transform(test_data)
            test_data = test_data.unsqueeze(0)

            inputs = test_data.to(self.dev)
            with torch.no_grad():
                outputs = self.net(inputs)  # 按照实际情况改写
            outputs_np = outputs.squeeze(0).cpu().detach()

            out_img = self.to_pil(outputs_np).resize(img_size)
            oimg_path = os.path.join(save_path, img_name[:-4] + '.png')
            out_img.save(oimg_path)

        total_time = time.time() - start_time
        fps = len(img_list) / total_time
        return fps


if __name__ == '__main__':
    proj_list = ['FastSmallMFCNV5_B', 'SmallMFCNV5_B']

    arg_dicts = {
        'FastSmallMFCNV5_B': {
            'net'      : FastSmallMFCNV5_B,
            'backbone' : Backbone_ResNet_18,
            'pth_path' : None,  # '/home/lart/coding/Paper_Code/output/CPD/pth/final.pth'
            'save_root': './output'  # 必须有
        },
        'SmallMFCNV5_B'    : {
            'net'      : SmallMFCNV5_B,
            'backbone' : Backbone_ResNet_18,
            'pth_path' : None,  # '/home/lart/coding/Paper_Code/output/CPD/pth/final.pth'
            'save_root': './output'  # 必须有
        },

        'crop_size'        : 320,
    }

    data_dicts = {
        # 'pascal-s': '/home/lart/Datasets/RGBSaliency/Pascal-S',
        'duts': '/home/lart/Datasets/RGBSaliency/DUTS/Test',
        # 'ecssd': '/home/lart/Datasets/RGBSaliency/ECSSD',
        # 'hku-is': '/home/lart/Datasets/RGBSaliency/HKU-IS',
        # 'dutomron': '/home/lart/Datasets/RGBSaliency/DUT-OMRON',
    }

    for proj_name in proj_list:
        pth_path = arg_dicts[proj_name]['pth_path']
        for data_name, data_path in data_dicts.items():
            save_path = os.path.join(arg_dicts[proj_name]['save_root'], data_name)
            fpser = FPSer(proj_name, arg_dicts, pth_path)
            fps = fpser.test(data_path, save_path)
            print(f"FPS:{fps}")
            del fpser

    print('所有模型速度测试完毕')
