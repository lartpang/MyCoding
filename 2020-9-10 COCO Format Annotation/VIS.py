import glob
import os
from PIL import Image

import json
import argparse

my_parser = argparse.ArgumentParser(allow_abbrev=False)
my_parser.add_argument("--jpeg_dir", required=True, type=str, help="JPEGImages文件夹")
my_parser.add_argument("--save_dir", required=True, type=str, help="保存输出的json文件的文件夹")


def construct_info_dict(dirname):
    videos = os.listdir(dirname)
    print(f"there is {len(videos)} videos in the folder {dirname}")

    videos_files = []
    for idx, video in enumerate(videos, start=1):
        file_names = []
        imagefiles = sorted(glob.glob(os.path.join(dirname, video, "*.jpg")))
        length = len(imagefiles)

        if length > 0:
            img = Image.open(os.path.join(imagefiles[0])).convert("RGB")
            w, h = img.size
            # print(length)
            for img_path in imagefiles:
                split_path = img_path.split("/")
                img_name = split_path[-1]
                img_path = video + "/" + img_name
                file_names.append(str(img_path))
            cont = {
                "width": w,
                "length": length,
                "date_captured": "2019-04-11 00:18:45.652544",
                "license": 1,
                "flickr_url": "",
                "file_names": file_names,
                "id": idx,
                "coco_url": "",
                "height": h,
            }
            videos_files.append(cont)
        else:
            print("the folder ", video, " has not jpeg images.")
    print("construct_info_dict end.")
    return videos_files


def generate_and_write_json(videos_info_dict, save_dir):
    data = {
        "info": {
            "description": "YouTube-VOS",
            "url": "https://youtube-vos.org/home",
            "version": "1.0",
            "year": 2018,
            "contributor": "ychfan",
            "date_created": "2019-04-11 00:18:45.652429",
        },
        "licenses": [
            {
                "url": "https://creativecommons.org/licenses/by/4.0/",
                "id": 1,
                "name": "Creative Commons Attribution 4.0 License",
            }
        ],
        "videos": videos_info_dict,
        "categories": [{"supercategory": "object", "id": 1, "name": "person"}],
    }
    json_obj = json.dumps(data, sort_keys=False, indent=2)
    with open(os.path.join(save_dir, "test_data.json"), "w") as json_file:
        json_file.write(json_obj)
    print("generate_and_write_json end.")


if __name__ == "__main__":
    my_args = my_parser.parse_args()
    videos_info_dict = construct_info_dict(my_args.jpeg_dir)
    generate_and_write_json(videos_info_dict, my_args.save_dir)
