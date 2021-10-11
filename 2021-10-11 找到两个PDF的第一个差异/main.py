# -*- coding: utf-8 -*-
# @Time    : 2021/1/25
# @Author  : Lart Pang
# @GitHub  : https://github.com/lartpang

import argparse
import difflib
import re
import sys

from pdfminer.high_level import extract_text


def find_first_diff_for_string(str1, str2, win_length=50):
    max_length = max(len(str1), len(str2))
    str1 += " " * (max_length - len(str1))
    str2 += " " * (max_length - len(str2))

    # start to find
    for win_start in range(0, max_length, win_length):
        clip1 = str1[win_start : win_start + win_length]
        clip2 = str2[win_start : win_start + win_length]
        if clip1 != clip2:
            print(
                f"{win_start}:{win_start + win_length}[{max_length}]"
                f"\n{clip1.encode('utf-8')}\n{clip2.encode('utf-8')}"
            )
            break


def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("pdf1", type=str)
    parser.add_argument("pdf2", type=str)
    parser.add_argument("--tool", type=str, choices=["firstline", "total"])
    args = parser.parse_args()
    return args


def main():
    args = get_args()

    SUB_MAPPING = {
        # r"\s": "",
        r"-": "",
    }

    pdf1 = extract_text(args.pdf1)
    pdf2 = extract_text(args.pdf2)

    for pattern, repl in SUB_MAPPING.items():
        pdf1 = re.sub(pattern=pattern, repl=repl, string=pdf1)
        pdf2 = re.sub(pattern=pattern, repl=repl, string=pdf2)

    if args.tool == "firstline":
        find_first_diff_for_string(pdf1, pdf2)
    else:
        result = difflib.ndiff(pdf1.splitlines(keepends=True), pdf2.splitlines(keepends=True))
        sys.stdout.writelines(result)


if __name__ == '__main__':
    main()
