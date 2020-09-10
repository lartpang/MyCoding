import os
import numpy as np
import cv2
from pycocotools import mask
from skimage import measure
from pycococreatortools import pycococreatortools

# def print_key_tree_from_dict(data, level: int):
#     if isinstance(data, dict):
#         for k, v in data.items():
#             print(" " * level, k)
#             print_key_tree_from_dict(v, level + 1)
#     elif isinstance(data, (tuple, list)):
#         if len(data) == 1:
#             print_key_tree_from_dict(data[0], level + 1)
#     return


def maskToanno(ground_truth_binary_mask, height, width, video_id, ann_id):
    fortran_ground_truth_binary_mask = np.asfortranarray(ground_truth_binary_mask)
    encoded_ground_truth = mask.encode(fortran_ground_truth_binary_mask)
    ground_truth_area = mask.area(encoded_ground_truth)
    ground_truth_bounding_box = mask.toBbox(encoded_ground_truth)
    contours = measure.find_contours(ground_truth_binary_mask, 0.5)

    annotations_template_dict = {
        "height": height,
        "width": width,
        "length": 1,
        "category_id": 1,
        "video_id": video_id,
        "iscrowd": 0,
        "id": ann_id,
        "segmentations": [],
        "bboxes": ground_truth_bounding_box.tolist(),
        "areas": ground_truth_area.tolist(),
    }

    for contour in contours:
        contour = np.flip(contour, axis=1)
        segmentation = contour.ravel().tolist()
        annotations_template_dict["segmentations"].append(segmentation)
    return annotations_template_dict


result_json = dict(
    info={
        "description": "YouTube-VOS",
        "url": "https://youtube-vos.org/home",
        "version": "1.0",
        "year": 2018,
        "contributor": "lartpang",
        "date_created": "2019-04-11 00:55:41.903634",
    },
    licenses=[
        {
            "url": "https://creativecommons.org/licenses/by/4.0/",
            "id": 1,
            "name": "Creative Commons Attribution 4.0 License",
        }
    ],
    videos=[],  # 存放所有视频的信息
    categories=[dict(supercategory="object", id=1, name="person")],
    annotations=[],
)

video_info_template_dict = {
    "width": None,
    "length": None,
    "date_captured": "2019-04-11 00:55:41.903902",
    "license": 1,
    "flickr_url": "",
    "file_names": None,
    "id": None,
    "coco_url": "",
    "height": None,
}

alivos_root = "/home/lart/Datasets/VideoSeg/AliTianChiPersonSeg/FuSai/Train/JPEGImages"
video_name_list = sorted(os.listdir(alivos_root))
all_video_info = []
all_annos_info = []
for video_id, video_name in enumerate(video_name_list, start=1):
    video_path = os.path.join(alivos_root, video_name)
    frame_name_list_per_video = sorted(os.listdir(video_path))

    first_frame_path = os.path.join(video_path, frame_name_list_per_video[0])
    image = cv2.imread(filename=first_frame_path, flags=1)
    height, width, _ = image.shape
    video_info_template_dict["height"], video_info_template_dict["width"] = height, width

    video_info_template_dict["file_names"] = {
        frame_id: "/".join([video_name, frame_name])
        for frame_id, frame_name in enumerate(frame_name_list_per_video)
    }

    video_info_template_dict["length"] = len(frame_name_list_per_video)

    video_info_template_dict["id"] = video_id

    all_video_info.append(video_info_template_dict)
    all_annos_info.append(maskToanno(ground_truth_binary_mask, height, width, video_id, ann_id))
