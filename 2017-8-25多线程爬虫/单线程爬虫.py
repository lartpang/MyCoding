import time
import socket
import re
# import threading
from urllib import parse
from urllib import request
from http import cookiejar
from bs4 import BeautifulSoup
from matplotlib import pyplot
from datetime import datetime


# 用于生成短评页面网址的函数
def MakeUrl(start):
    """make the next page's url"""
    url = 'https://movie.douban.com/subject/26934346/comments?start=' \
        + str(start) + '&limit=20&sort=new_score&status=P'
    return url


def MakeOpener():
    """make the opener of requset"""
    # 保存cookies便于后续页面的保持登陆
    cookie = cookiejar.CookieJar()
    cookie_support = request.HTTPCookieProcessor(cookie)
    opener = request.build_opener(cookie_support)
    return opener


def MakeRes(url, opener, formdata, headers):
    """make the response of http"""
    # 编码信息，生成请求，打开页面获取内容
    data = parse.urlencode(formdata).encode('utf-8')
    req = request.Request(
                    url=url,
                    data=data,
                    headers=headers
                )
    response = opener.open(req).read().decode('utf-8')
    return response


def GetNum(soup):
    """get the number of pages"""
    # 获得页面评论文字
    totalnum = soup.select("div.mod-hd h2 span a")[0].get_text()[3:-2]
    # 计算出页数
    pagenum = int(totalnum) // 20
    print("the number of comments is:" + totalnum,
          "the number of pages is: " + str(pagenum))
    return pagenum


def IOFile(soup, opener, file, pagestart, step):
    """the IO operation of file"""
    # 循环爬取内容
    for item in range(step):
        start = (pagestart + item) * 20
        print('第' + str(pagestart + item) + '页评论开始爬取')
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
        # 延时1s
        time.sleep(3)

    print('线程采集写入完毕')


def GetSoup():
    """get the soup and the opener of url"""
    main_url = 'https://accounts.douban.com/login?source=movie'
    formdata = {
        "form_email": "your-email",
        "form_password": "your-password",
        "source": "movie",
        "redir": "https://movie.douban.com/subject/26934346/",
        "login": "登录"
            }
    headers = {
        "User-Agent": "Mozilla/5.0 (Windows NT 5.1; U; en; rv:1.8.1)\
            Gecko/20061208 Firefox/2.0.0 Opera 9.50",
        'Connection': 'keep-alive'
            }
    opener = MakeOpener()

    response_login = MakeRes(main_url, opener, formdata, headers)
    soup = BeautifulSoup(response_login, "html.parser")

    if soup.find('img', id='captcha_image'):
        print("有验证码")
        # 获取验证码图片地址
        soup = BeautifulSoup(response_login, "html.parser")
        captchaAddr = soup.find('img', id='captcha_image')['src']
        # 匹配验证码id
        reCaptchaID = r'<input type="hidden" name="captcha-id" value="(.*?)"/'
        captchaID = re.findall(reCaptchaID, response_login)
        # 下载验证码图片
        request.urlretrieve(captchaAddr, "captcha.jpg")
        img = pyplot.imread("captcha.jpg")
        pyplot.imshow(img)
        pyplot.axis('off')
        pyplot.show()
        # 输入验证码并加入提交信息中，重新编码提交获得页面内容
        captcha = input('please input the captcha:')
        formdata['captcha-solution'] = captcha
        formdata['captcha-id'] = captchaID[0]
        response_login = MakeRes(main_url, opener, formdata, headers)
        soup = BeautifulSoup(response_login, "html.parser")

    return soup, opener


def main():
    """main function"""
    timeout = 2
    socket.setdefaulttimeout(timeout)
    now = datetime.now()
    soup, opener = GetSoup()

    pagenum = GetNum(soup)
#    step = pagenum // 9
#    pagelist = [x for x in range(0, pagenum, step)]
#    print('pageurl`s list={}, step={}'.format(pagelist, step))

    # 追加写文件的方式打开文件
    with open('秘密森林的短评.txt', 'w+', encoding='utf-8') as file:
        pagestart = 0
        IOFile(soup, opener, file, pagestart, pagenum)

#        thread = []
#        for i in range(0, 10):
#            t = threading.Thread(
#                    target=IOFile,
#                    args=(soup, opener, file, pagelist[i], step)
#                )
#            thread.append(t)
#
#        # 建立线程
#        for i in range(0, 10):
#            thread[i].start()
#
#        for i in range(0, 10):
#            thread[i].join()

    end = datetime.now()
    print("程序耗时： " + str(end-now))


if __name__ == "__main__":
    main()
