# -*- coding: utf-8 -*-
"""
Created on Thu Aug 17 16:31:35 2017

@note: 为了便于阅读，将模块的引用就近安置了
@author: lart
"""

# 用于生成短评页面网址的函数
def MakeUrl(start):
    """make the next page's url"""
    url = 'https://movie.douban.com/subject/26934346/comments?start=' + str(start) + '&limit=20&sort=new_score&status=P'
    return url


# 登录页面信息
main_url = 'https://accounts.douban.com/login?source=movie'
formdata = {
    "form_email":"your-email",
    "form_password":"your-password",
    "source":"movie",
    "redir":"https://movie.douban.com/subject/26934346/",
    "login":"登录"
}
user_agent = r'Mozilla/5.0 (Windows NT 6.2; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/27.0.1453.94 Safari/537.36'
headers = {'User-Agnet': user_agent, 'Connection': 'keep-alive'}


# 保存cookies便于后续页面的保持登陆
from urllib import request
from http import cookiejar

cookie = cookiejar.CookieJar()
cookie_support = request.HTTPCookieProcessor(cookie)
opener = request.build_opener(cookie_support)


# 编码信息，生成请求，打开页面获取内容
from urllib import parse

logingpostdata = parse.urlencode(formdata).encode('utf-8')
req_ligin = request.Request(url=main_url, data=logingpostdata, headers=headers)
response_login = opener.open(req_ligin).read().decode('utf-8')


# 获取验证码图片地址
from bs4 import BeautifulSoup

try:
    soup = BeautifulSoup(response_login, "html.parser")
    captchaAddr = soup.find('img', id='captcha_image')['src']

    # 匹配验证码id
    import re

    reCaptchaID = r'<input type="hidden" name="captcha-id" value="(.*?)"/'
    captchaID = re.findall(reCaptchaID, response_login)

    # 下载验证码图片
    request.urlretrieve(captchaAddr, "captcha.jpg")

    # 输入验证码并加入提交信息中，重新编码提交获得页面内容
    captcha = input('please input the captcha:')
    formdata['captcha-solution'] = captcha
    formdata['captcha-id'] = captchaID[0]
    logingpostdata = parse.urlencode(formdata).encode('utf-8')
    req_ligin = request.Request(url=main_url, data=logingpostdata, headers=headers)
    response_login = opener.open(req_ligin).read().decode('utf-8')

finally:
    # 获得页面评论文字
    soup = BeautifulSoup(response_login, "html.parser")
    totalnum = soup.select("div.mod-hd h2 span a")[0].get_text()[3:-2]

    # 计算出页数和最后一页的评论数
    pagenum = int(totalnum) // 20
    commentnum = int(totalnum) % 20
    print(pagenum, commentnum)

    # 设置等待时间，避免爬取太快
    import time
    # 用于在超时的时候抛出异常，便于捕获重连
    import socket

    timeout = 3
    socket.setdefaulttimeout(timeout)

    # 追加写文件的方式打开文件
    with open('秘密森林的短评.txt', 'w+', encoding='utf-8') as file:
        # 循环爬取内容
        for item in range(pagenum):
            print('第' + str(item) + '页')
            start = item * 20
            url = MakeUrl(start)

            # 超时重连
            state = False
            while not state:
                try:
                    html = opener.open(url).read().decode('utf-8')
                    state = True
                except socket.timeout:
                    state = False

            # 获得评论内容
            soup = BeautifulSoup(html, "html.parser")
            comments = soup.select("div.comment > p")
            for text in comments:
                file.write(text.get_text().split()[0] + '\n')
                print(text.get_text())
                limit_num = 0
                if item == pagenum - 1:
                    limit_num =+ 1
                    if limit_num == commentnum:
                        break
            time.sleep(3)

    print('采集写入完毕')
