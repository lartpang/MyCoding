# -*- coding: utf-8 -*-
# @Time    : 2021/7/14
# @Author  : Lart Pang
# @GitHub  : https://github.com/lartpang

import hashlib
import hmac
import base64
import argparse


def shansing(args):
    """
    A python implement of the core encryption process of the great tool: <https://shansing.com/passwords/>

    The core js code:

    ::

        function countV2(password, key) { // By: Shansing
            var symbol = "!@#$%!@#$%!@#$%!@#$%!@#$%!@#$%";
            var head, hash, code40, code16;
            hash = sha256.hmac(key, password);
            hash = sha256.hmac.update("ShansingPv2", hash).array();
            code40 = hash.slice(0, 30);
            code40 = btoa(String.fromCharCode.apply(null, code40));
            head = code40.charCodeAt(0);
            if (head >= 65 && head <= 90) head = symbol[head-65];
            else if (head >= 97 && head <= 122) head = symbol[head-97];
            else if (head >= 48 && head <= 57) head = symbol[head-47];  //-48+1, history issue
            else if (head === 43) head = symbol[3]; // + -> $
            else if (head === 47) head = symbol[4]; // / -> %
            code40 = head + code40.slice(1).replace(/\+/g, "/").replace(/\//g, "\\");
            code16 = code40.slice(0, 16);

            setCopyVar(code16, code40);
        }

    Some refrences:
    - [🌟🌟] https://shansing.com/read/477/
    - https://zhuanlan.zhihu.com/p/148364711
    """
    base_pw: str = args.basepassword
    code: str = args.code
    symbol: str = "!@#$%!@#$%!@#$%!@#$%!@#$%!@#$%"

    hex_hash_str = hmac.new(key=code.encode(), msg=base_pw.encode(), digestmod=hashlib.sha256).hexdigest()
    hex_hash_str = hmac.new(
        key="ShansingPv2".encode(), msg=hex_hash_str.encode(), digestmod=hashlib.sha256
    ).hexdigest()

    bytes_list = []
    for i in range(0, len(hex_hash_str), 2):
        bytes_list.append(int("0x" + hex_hash_str[i] + hex_hash_str[i + 1], 16))
    code40 = bytes_list[:30]
    code40 = "".join([chr(x) for x in code40])

    # https://zhuanlan.zhihu.com/p/148364711
    # js btoa == base64.b64encode("待编码的字符".encode("latin1")).decode("utf8")
    code40 = base64.b64encode(code40.encode("latin1")).decode()
    head = ord(code40[0])
    if 64 <= head <= 90:
        head = symbol[head - 65]
    elif 97 <= head <= 122:
        head = symbol[head - 97]
    elif 48 <= head <= 57:
        head = symbol[head - 47]
    elif head == 43:
        head = symbol[3]
    elif head == 47:
        head = symbol[4]
    code40 = head + code40[1:].replace("+", "/").replace("/", "\\")
    code16 = code40[:16]
    print(code16)


def flowerpassword(args):
    """
    A python implement of the core encryption process of the great tool: <https://flowerpassword.com>

    The core js code:

    ::

        getLastPassword = function() {
            for (var t = document.querySelector(".content-box .password-in-mind").value,
                     e = document.querySelector(".content-box .key").value,
                     n = i(t, e),
                     r = i(n, "snow"),
                     o = i(n, "kise").split(""),
                     u = r.split(""),
                     c = 0
                ; c <= 31
                ; c++)
                if (isNaN(u[c])) {
                    "sunlovesnow1990090127xykab".search(o[c]) > -1 && (u[c] = u[c].toUpperCase())
                }
            var a = u.join("")
            , s = a.slice(0, 1);
            return isNaN(s) ? a.slice(0, 16) : "K" + a.slice(1, 16)
        }

    Some refrences:
    - [🌟🌟] https://shansing.com/read/477/
    - https://gitee.com/batcom/flower/blob/master/pw.py
    - https://xlsdg.org/p/68f1da70.html
    """
    base_pw: str = args.basepassword
    code: str = args.code
    upper_code: str = "sunlovesnow1990090127xykab"

    md5_str1 = hmac.new(key=code.encode(), msg=base_pw.encode(), digestmod=hashlib.md5).hexdigest().encode("utf-8")
    source = [x for x in hmac.new(key="snow".encode(), msg=md5_str1, digestmod=hashlib.md5).hexdigest()]
    rule = [x for x in hmac.new(key="kise".encode(), msg=md5_str1, digestmod=hashlib.md5).hexdigest()]

    for c in range(32):
        if not source[c].isdigit():
            if upper_code.find(rule[c]) > -1:
                source[c] = source[c].upper()

    code32 = "".join(source)

    if not code32[0].isdigit():
        code16 = code32[:16]
    else:
        code16 = "K" + code32[1:16]
    print(code16)


def get_args():
    parser = argparse.ArgumentParser(description="A python tool for encrypting your password.")
    parser.add_argument("basepassword", type=str, help="A single string used as the base password.")
    parser.add_argument("code", type=str, help="A mnemonic string used to distinguish passwords.")

    subparsers = parser.add_subparsers()
    flower_parser = subparsers.add_parser(
        name="flower",
        help="A argument group for the flower mode. This mode is the same as <https://flowerpassword.com>.",
    )
    flower_parser.set_defaults(func=flowerpassword)

    shansing_parser = subparsers.add_parser(
        name="shansing",
        help="A argument group for the shansing mode. This mode is the same as <https://shansing.com/passwords/> .",
    )
    shansing_parser.set_defaults(func=shansing)

    args = parser.parse_args()
    args.func(args)


if __name__ == "__main__":
    get_args()
