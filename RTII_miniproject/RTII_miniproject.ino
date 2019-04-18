//________________THE TAB-SAVEr__________________
//Real-Time Interfaces & Interaction miniproject
//Rasmus Kruuse          Student nr.2016-3929        AAU CPH 
//
//libraries 
//#include <Adafruit_ESP8266.h>
#include <CapacitiveSensor.h> // to use pins as capacitative touch sensors
#include <EEPROM.h>
#include <string.h>


//general
unsigned long timeOfLastIr = 0;     //  used for debuging
char data[500];             //hold mesages until they are writen to memory
int currentIndex = 0;       //where to in data to write the new msg

//variabels for cap sensor
CapacitiveSensor cs = CapacitiveSensor(2,4);
#define CAP_THRESHOLD 50            //threshold above which the capacitiveSensor is considered held down
unsigned long timeOfPress = 0;      //used to check how long the capacitiveSensor was held
unsigned long timeOfLastPress = 0;  //used to check for double clicks
bool wasPressed = false;   //if the button cap sensor was pressed at the last reading (used to detect the finger lifting)

//variables for serial msaging  
bool isReceiving = false;   //controls if the received msg should be saved in memory (set true when reciving a "sending"msg)
bool sensorWorking = true;  //controls if device should read from sensor or look/send msg on serial

//variables for knock detection with peizo
#define KNOCK_THRESHOLD 65
#define piezoPin A0
unsigned long timeOfLastKnock = 0;


//led variables
int ledState = 0;   // the led starts in no state
int ledPin = 5;     //pin the led is attached to


void setup(){
    Serial.begin(9600);
    pinMode(ledPin, OUTPUT);
    pinMode(8, OUTPUT);
    pinMode(9, OUTPUT);
    pinMode(piezoPin, INPUT);
}

void loop(){
    //reset variabels
    ledState = 0;
    bool isPressed = false;

    unsigned long time = millis();


    if(sensorWorking){
        long capReading = cs.capacitiveSensor(10);
        //Serial.println("    Reading : " + String(capReading));
        if(capReading > CAP_THRESHOLD){// if being pressed, turn on the light
            ledState = 1;
            isPressed = true;
        }

        if(isPressed == true && wasPressed == false){//if being pressed for the first time
            timeOfPress = time;
        }

        if(wasPressed == true && isPressed == false){ //if user let go
            performAction(time - timeOfPress);
        }

        led(ledState); //set the led to active pattern

        wasPressed = isPressed;

        if(analogRead(piezoPin)> KNOCK_THRESHOLD){
            if((timeOfLastKnock - time)> 200){
                closeConnection();
            }
            timeOfLastKnock = time;
            while(analogRead(piezoPin)> KNOCK_THRESHOLD){
               // Serial.println(" : " + String(millis()-timeOfLastIr));
            }
        }

    }else{

        String msg = Serial.readString();
        if(msg != ""){
            sendMsg(msg); //if a message was received echo it back
            
            if( msg == "done"){ //if we receive "done" stop saving mesages and save list of msg in EEPROM memory
                isReceiving = false;
                write_data(0, data, currentIndex);
                sendMsg(String(data));
                currentIndex = 0;
                memset(data,0,sizeof data);
                sensorWorking = true;
            }

            //use an LED to indicate if it is receiving 
            if(isReceiving){
                digitalWrite(8,HIGH);
                int msgSize = msg.length();//to avoid the ending char 
                //read the data into the preassigned char array
                for(int i = 0; i < msgSize; i++){
                    data[i+currentIndex] = msg[i];
                }
                currentIndex += msgSize; // get new index postion
                data[currentIndex++] = ' ';
            }else{
                digitalWrite(8,LOW); 
            }

            if( msg == "sending"){  //sending indicates that we should save the next msg's in memory
                isReceiving = true;
            }

            if( msg == "read"){ //read means that we should send the saved list of open tabs to python
                read_data(0);
                sendMsgNoEnd("inMemory:");
                sendMsgNoEnd(String(data));
                sendMsg("||");
                sensorWorking = true;
                //closeConnection();
            }
        }
    }

    timeOfLastIr = time;
}

void led(int state){
    digitalWrite(ledPin,LOW);
    if(state == 1){
        digitalWrite(ledPin,HIGH);
    }
}

void performAction(unsigned long duration){ //perform action based on duration of press
    if(duration> 100 && duration < 1000){
        if(millis() - timeOfLastPress < 1000){
            openTabs();
        }

    }else if(duration > 1500){
        digitalWrite(9,HIGH);
        receiveList();
    }
    timeOfLastPress = millis();
}

void receiveList(){
    sendMsg("gettabs:");
    sensorWorking = false;
}

void openTabs(){
    sendMsg("opentabs:");
    sensorWorking = false;
}

void write_data(char add, char data[500], int size)
{
    int i;
    for(i=0;i < size;i++)
    {
        EEPROM.write(add+i,data[i]);
    }
    EEPROM.write(add+size,'\0');   //Add termination null character for String Data
}


void read_data(char add)
{
    int len=0;
    unsigned char k;
    k=EEPROM.read(add);
    while(len<500)   //Read until null character
    {    
        k=EEPROM.read(add+len);
        if(k == '\0'){
            break;
        }
        data[len]=k;
        len++;
    }
    currentIndex = len;
}

void sendMsg(String msgToSend){
    Serial.print(msgToSend + "\n");
}

void sendMsgNoEnd(String msgToSend){
    Serial.print(msgToSend);
}

void closeConnection(){
    sendMsg("close:");
}
