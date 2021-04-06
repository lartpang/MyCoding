# -*- coding: utf-8 -*-
# @Time    : 2021/2/20
# @Author  : Lart Pang
# @GitHub  : https://github.com/lartpang

import os

from PIL import Image


def image_to_pdf(image_root, pdf_dir):
    chapter_names = sorted(os.listdir(image_root))
    for chapter_name in chapter_names:
        pdf_path = os.path.join(pdf_dir, chapter_name + ".pdf")
        chapter_path = os.path.join(image_root, chapter_name)
        image_names = sorted(os.listdir(chapter_path))

        images = []
        for image_name in image_names:
            image_path = os.path.join(chapter_path, image_name)
            image = Image.open(image_path).convert("RGB")
            images.append(image)

        head_image = images[0]
        other_image = images[1:]
        head_image.save(pdf_path, save_all=True, append_images=other_image)
        print(f"save {chapter_name}")


def image_to_pdf_v2(image_root, pdf_dir):
    chapter_names = sorted(os.listdir(image_root))
    for chapter_name in chapter_names:
        pdf_path = os.path.join(pdf_dir, chapter_name + ".pdf")
        chapter_path = os.path.join(image_root, chapter_name)
        image_names = sorted(os.listdir(chapter_path))

        hights = []
        images = []
        for image_name in image_names:
            image_path = os.path.join(chapter_path, image_name)
            image = Image.open(image_path).convert("RGB")
            w, h = image.size
            hights.append(w)
            images.append(image)

        canvas = Image.new("RGB", size=(730, sum(hights) + 10), color=(255, 255, 255))

        y = 5
        for image, image_h in zip(images, hights):
            canvas.paste(images, box=(5, y))
            image.close()
            y += image_h

        canvas.save(pdf_path, save_all=True)
        print(f"save {chapter_name}")


if __name__ == "__main__":
    image_root = "picture_path"  # 相对路径
    pdf_dir = "pdf_patch"
    if not os.path.exists(pdf_dir):
        os.mkdir(pdf_dir)
    image_to_pdf(image_root, pdf_dir)
