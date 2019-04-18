#________________THE TAB-SAVEr__________________
#Real-Time Interfaces & Interaction miniproject
#Rasmus Kruuse          Student nr.2016-3929        AAU CPH 

#the computer side program
import webbrowser as wb
import pyautogui as py
from win32gui import GetWindowText, GetForegroundWindow
import win32clipboard
import serial


def openTab(input = ""):
    """Try to open a new browser window for each string in the list passed to the function"""
    for s in input:
        wb.open_new(s)

def getListOfTabs():
    """alt + tab's until google chrome is open, then ctrl + tab though tabs until every tab has been visited once. 
    Returns the list of open tabs """

    screenWidth, screenHeight = py.size()
    a = "start"
    first_opened = GetWindowText(GetForegroundWindow()) #save the first window that is open, so we know when we have checked all
    i = 1
    while first_opened != a: #until we return to the first opened window
        py.keyDown("alt")   #alt then tab i number of tmies
        for n in range(0,i):
            py.press("tab")
        py.keyUp("alt")
        a = GetWindowText(GetForegroundWindow())    #save the title of the foreground window
        if "Google Chrome" in a:    #if it is google chrome break the loop
            break
        py.keyDown("alt")   #otherwise alt tab back to the first window
        py.press("tab")     #this is done because alt tabing from here will mess up the order of the windows
        py.keyUp("alt")
        i += 1

    
    #now that google chrome is in focus time to get the tabs
    tabs = []   #list that gonna hold the url of all open tabs
    py.moveTo(screenWidth*0.5,68) #place cursor on address bar

    for a in range(100): #the 100 is an arbirary max, and would allow upto 50 tabs(much more than the arduino can store)
        #the approach is this -- itterate through two tabs and add them to the list. then check if the first half of the list
        #is equal to the second half. This is to see that we have visited all tabs. This is also why we make two step itterations
        # if you have 5 tabs open it will at first be discovered on after adding 10 tabs
        for _ in range(2):
            py.click()  #click address bar
            py.keyDown("ctrl",_pause=False) #copy content
            py.press(["a","c","tab"])
            py.keyUp("ctrl",pause=False)
            win32clipboard.OpenClipboard()  #open the clip board and add the data from there to the list of tabs
            tabs.append(win32clipboard.GetClipboardData())
            win32clipboard.CloseClipboard()
        
        if(tabs[:len(tabs)//2] == tabs[len(tabs)//2:]): #explained above
            break
    return tabs[:len(tabs)//2]

inputTest = "https://docs.python.org/3/tutorial/datastructures.html", "https://www.youtube.com", "https://hotmail.com"


#listening test
ser = serial.Serial()   #connect to the arduino, with correct baudrate and all the other stuff
ser.baudrate = 9600
ser.port = 'COM240'
ser.open()

msgToBeSent = [] # a queue of mesages to be sent
#the program waits for the arduino to send back return mesages to confirm that they have been sent correctly

while 1:
    a = ser.read_until(b'\n').decode()  #read until we receive a newline, then convert that into ascii
    if(a != ""):    #print what we received
        print("Received : " + a)
    
    if "gettabs:" in str(a):    #gettabs asks us to send the current google chrome tabs to the arduino
        listOfTabs = getListOfTabs()
        msgToBeSent.append('sending')   #"sending" tells the arduino to be prepared to save the mesages it receives
        ser.write(bytes(str(msgToBeSent[0]).encode("ascii")))   #send first mesages
        print("begining to send")
        for s in listOfTabs:    #then add all the other tabs the sending stacj
            msgToBeSent.append(s)
        msgToBeSent.append('done')  #finally add "done" to the sending queue, 
        #this tell the arduino to save the mesages to eeprom memory

    if "opentabs:" in str(a): #opentab tell the arduino want to send a list of tabs to open
        ser.write(bytes(str('read').encode("ascii")))   #send the message read to confirm we are ready

    if "inMemory:" in str(a):   #in memory will be followed by a list of tab
        print('Got the list')
        b = str(str(a).partition('inMemory:')[2])
        print(b)
        b = str(b.partition('||')[0]) #cut away the fat to end up with a list of space separated items
        print(b)
        x = openTab([n for n in b.split(' ') if n != ''])   #try to open a tab for each item

    if "close:" in str(a):  #close means the device has been "knocked" and is done
        ser.close()
        break

    if msgToBeSent == []:   # if there is no new messages to be sent do nothing
        continue

    if msgToBeSent[0] in str(a):    #if the mesage we got back is the same as the message we just sent, send the next one
        msgToBeSent.pop(0)
        
        if(msgToBeSent == []):
            continue

        ser.write(bytes(str(msgToBeSent[0]).encode("ascii")))
        print("sent the message" + str(msgToBeSent[0]))
    else :
        print("did not find " + str(msgToBeSent[0]) + " in " + str(a))

#once done, just do a simple clean up
del(ser)
print("closing connection")
