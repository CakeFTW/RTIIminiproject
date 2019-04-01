#________________THE TAB-SAVEr__________________
#Real-Time Interfaces & Interaction miniproject
#Rasmus Kruuse          Student nr.2016-3929        AAU CPH 

#the computer side program
from selenium import webdriver
from selenium.webdriver.common.keys import Keys
import time
import pyautogui as py
import math
from win32gui import GetWindowText, GetForegroundWindow
import win32clipboard



def openTab(input = ""): #open all tabs passed to the function
    driver = webdriver.Chrome() #could be made faster i think

    for s in input: 
        driver.execute_script("window.open('"+s+"');")

    driver.switch_to_window(driver.window_handles[0])
    driver.close()
    return driver


def getListOfTabs():
    screenWidth, screenHeight = py.size()
    print(GetWindowText(GetForegroundWindow()))
    tabs = []
    a = "start"
    b = GetWindowText(GetForegroundWindow())
    i = 1
    while "Google Chrome" not in a and b != a:
        py.keyDown("alt")
        for n in range(0,i):
            py.press("tab")
        py.keyUp("alt")
        a = GetWindowText(GetForegroundWindow())
        py.keyDown("alt")
        py.press("tab")
        py.keyUp("alt")
        i += 1
        print(a)

    py.keyDown("alt")
    py.press("tab")
    py.keyUp("alt")

    py.moveTo(screenWidth*0.5,68) #place cursor on 
    py.click()
    py.keyDown("ctrl")
    py.press(["a","c"])
    py.keyUp("ctrl")


    win32clipboard.OpenClipboard()
    tabs.append(win32clipboard.GetClipboardData())


    return tabs

inputTest = "https://docs.python.org/3/tutorial/datastructures.html", "https://www.youtube.com", "https://hotmail.com"





print(getListOfTabs())






