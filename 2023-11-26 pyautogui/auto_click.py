import time

import pyautogui as pag

tab_xy = pag.Point(x=1436, y=21)
star_xy = pag.Point(x=503, y=833)
comm_xy = pag.Point(x=268, y=1854)
input_xy = pag.Point(x=508, y=484)
input_xy2 = pag.Point(x=349, y=608)
next_xy = pag.Point(x=1470, y=294)

# 为所有的PyAutoGUI函数增加延迟（停顿）,默认延迟时间是0.1秒
pag.PAUSE = 1

for i in range(30):
    print(f"Processing {i}th page.")
    time.sleep(2)
    star_button_ltwh = pag.locateOnScreen('blankstar.png')
    if star_button_ltwh is not None:
        star_button_xy: pag.Point = pag.center(star_button_ltwh)
        print("Start XY: ", star_button_xy)
        pag.click(x=520, y=star_button_xy.y)
        pag.scroll(-500)
        pag.click(x=comm_xy.x, y=comm_xy.y)
        pag.click(x=input_xy.x, y=input_xy.y)
        pag.click(x=input_xy2.x, y=input_xy2.y)
        pag.hotkey("ctrl", "v")
        pag.hotkey("ctrl", "enter")
        pag.click(x=next_xy.x, y=next_xy.y)
    time.sleep(3)
