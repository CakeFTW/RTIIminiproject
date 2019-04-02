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
import serial



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


    #open google chrome
    py.keyDown("alt")
    py.press("tab")
    py.keyUp("alt")

    py.moveTo(screenWidth*0.5,68) #place cursor on adress bar



    for a in range(15):
        for _ in range(2):
            py.click()
            py.keyDown("ctrl")
            py.press(["a","c","tab"])
            py.keyUp("ctrl")
            win32clipboard.OpenClipboard()
            tabs.append(win32clipboard.GetClipboardData())
            win32clipboard.CloseClipboard()
        
        if(tabs[:len(tabs)//2] == tabs[len(tabs)//2:]):
            print(tabs[:len(tabs)//2])
            print(tabs[len(tabs)//2:])
            break
    return tabs[:len(tabs)//2]

inputTest = "https://docs.python.org/3/tutorial/datastructures.html", "https://www.youtube.com", "https://hotmail.com"


#listening test
ser = serial.Serial()
ser.baudrate = 9600
ser.port = 'COM240'
ser.open()

doneSending = False
msgToBeSent = []

while 1:
    a = ser.read_all().decode("utf-8")
    time.sleep(3)
    print(a)
    time.sleep(0.05)
    if '1' in a and not doneSending:
        msgToBeSent.append('save')
        msgToBeSent.append("oit mate")
        msgToBeSent.append('done sending')
        ser.write(bytes(str(msgToBeSent[0]).encode("ascii")))
        doneSending = True
    
    
    if "gettabs" in str(a):
        listOfTabs = getListOfTabs()

    if msgToBeSent == []:
        continue

    if msgToBeSent[0] in str(a):
        msgToBeSent.pop(0)
        
    if(msgToBeSent != []):
            continue

        ser.write(bytes(str(msgToBeSent[0]).encode("ascii")))
        print("sent the message" + str(msgToBeSent[0]))
    else :
        print("did not find " + str(msgToBeSent[0]) + " in " + str(a))







