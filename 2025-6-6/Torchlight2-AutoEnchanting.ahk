#Requires AutoHotkey v2.0
#SingleInstance Force

; ===== 封装函数 =====
HardwareClick(targetX, targetY, delayBeforeClick := 100, delayBetween := 100) {
    DllCall("SetCursorPos", "int", targetX, "int", targetY)
    Sleep(delayBeforeClick)
    DllCall("mouse_event", "uint", 0x02, "uint", 0, "uint", 0, "uint", 0, "ptr", 0)  ; MOUSEEVENTF_LEFTDOWN
    Sleep(delayBetween)
    DllCall("mouse_event", "uint", 0x04, "uint", 0, "uint", 0, "uint", 0, "ptr", 0)  ; MOUSEEVENTF_LEFTUP
}

EnchantLoop(winTitle := "Torchlight II v.1.25.9.5", numLoops := 2000, actionDelay := 500) {
    if !WinExist(winTitle) {
        MsgBox("游戏窗口未找到！请启动" winTitle "后再运行脚本。", "错误", "IconX")
        return
    }

    WinActivate(winTitle)
    WinWaitActive(winTitle, , 2)

    loop numLoops {
        ; 取消附魔
        HardwareClick(794, 849)
        Sleep(actionDelay)

        ; 点击附魔按钮
        HardwareClick(814, 703)
        Sleep(actionDelay)

        ; 确认弹窗
        HardwareClick(1846, 1274)
        Sleep(actionDelay)

        ToolTip("附魔进度: " A_Index "/" numLoops)
    }

    ; === 新增：操作结束提示 ===
    ToolTip("附魔完成！")          ; 显示最终提示
    SetTimer(() => ToolTip(), -2000) ; 2秒后自动关闭提示
}

F1:: EnchantLoop()
F2:: ExitApp()
