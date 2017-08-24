# -*- coding: utf-8 -*-
"""
Created on Thu Aug 24 13:31:26 2017

@title: xpath练手
@author: lart
"""

from lxml import etree
from urllib import request

url = 'http://www.baidu.com'
req = request.Request(url)
res = request.urlopen(req)
text = res.read().decode('utf-8')
print(type(text)) # <class 'str'>
html = etree.HTML(text)
print(type(html)) # <class 'lxml.etree._Element'>
result_1 = html.xpath('//img/@src')
print(result_1[0]+ ' ; ' +result_1[1]) # //www.baidu.com/img/bd_logo1.png ; //www.baidu.com/img/baidu_jgylogo3.gif
result_2 = html.xpath('//div[@id="mCon"]//text()')
print(result_2) # ['输入法']
result_3 = html.xpath('//div')
print(result_3) # 'div','/div'返回空，'//div'返回非空列表
result_4 = html.xpath('//@src')
print(result_4) # ['//www.baidu.com/img/bd_logo1.png', '//www.baidu.com/img/baidu_jgylogo3.gif', 'http://s1.bdstatic.com/r/www/cache/static/jquery/jquery-1.10.2.min_65682a2.js']
result_5 = html.xpath('//ul[@id="mMenu"]//@name')
print(result_5) # ['ime_hw', 'ime_py', 'ime_cl']
# 多种方法打印html文件中的“更多产品”
# 纯路径选择
f_1 = html.xpath('//div[@id="u1"]/a[last()]/text()')
print(f_1[0])
f_2 = html.xpath('//a[@href="http://www.baidu.com/more/"]/text()')
print(f_2[0])
f_3 = html.xpath('//div[@id="u1"]//text()')
print(f_3[-1])
f_4 = html.xpath('//a[@href="http://www.baidu.com/more/"]/node()')
print(f_4[0])
f_5 = html.xpath('//div[@id="u1"]/a[last()]/node()') # 可见，文本也是节点的一种。
# 有七种类型的节点：元素、属性、文本、命名空间、处理指令、注释以及文档节点（或称为根节点）
print(f_5[0])
# 路径配合step选择
f_6 = html.xpath('//div[@id="u1"]/child::node()[last()]/node()')
print(f_6[0])
f_7 = html.xpath('//div[@id="u1"]//child::text()')
print(f_7[-1])
f_8 = html.xpath('//div[@id="u1"]/child::*/child::text()')
print(f_8[-1])
f_9 = html.xpath('//div[@id="u1"]/child::*/child::node()')
print(f_9[-1])

