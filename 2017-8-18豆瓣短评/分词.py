# -*- coding: utf-8 -*-
"""
Created on Thu Aug 17 16:31:35 2017

@note: 为了便于阅读，将模块的引用就近安置了
@author: lart
"""

# 读取事先爬取好的文件，由于文件较小，直接一次性读入。若文件较大，则最好分体积读入。
with open('秘密森林的短评.txt', 'r', encoding='utf-8') as file:
    comments = file.readlines()
    comment = ''.join(comments)


# 摘取中文字符，没有在下载时处理，正好保留原始数据。
import re

pattern = re.compile(r'[\u4e00-\u9fa5]+')
data = pattern.findall(comment)
filted_comment = ''.join(data)

# 分词
import jieba

word = jieba.lcut(filted_comment)


# 整理
import pandas as pd

words_df = pd.DataFrame({'words': word})

#停词相关设置。参数 quoting=3 全不引用
stopwords = pd.read_csv(
        "stopwords.txt",
        index_col=False,
        quoting=3,
        sep="\t",
        names=['stopword'],
        encoding='utf-8'
        )
words_df = words_df[~words_df.words.isin(stopwords.stopword)]

# 聚合
words_stat = words_df.groupby('words')['words'].agg({'size'})
words_stat = words_stat.reset_index().sort_values("size", ascending=False)


# 词云设置
from wordcloud import WordCloud
import matplotlib.pyplot as plt
import numpy as np

def rgb2gray(rgb):
    return np.dot(rgb[...,:3], [0.299, 0.587, 0.114])

def gray2bw(gray):
    for raw in range(len(gray)):
        for col in range(len(gray[raw])):
            gray[raw][col] = (0 if gray[raw][col]>50 else 255)
    return gray

img = plt.imread('4.jpg')
mask = rgb2gray(img)
bw = gray2bw(mask)

wordcloud = WordCloud(
                font_path="YaHei Consolas Hybrid.ttf",
                background_color="white",
                mask=bw,
                max_font_size=80
                )
# word_frequence 为字典类型，可以直接传入wordcloud.fit_words()
word_frequence = {
        x[0]:x[1] for x in words_stat.head(1000).values
        }
wordcloud = wordcloud.fit_words(word_frequence)

# 存储显示
plt.imsave('img.jpg', wordcloud)

plt.subplot(131)
plt.imshow(img)
plt.axis("off")
plt.subplot(132)
plt.imshow(bw)
plt.axis("off")
plt.subplot(133)
plt.imshow(wordcloud, interpolation='bilinear')
plt.axis("off")