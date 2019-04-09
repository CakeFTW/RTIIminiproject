//________________THE TAB-SAVEr__________________
//Real-Time Interfaces & Interaction miniproject
//Rasmus Kruuse          Student nr.2016-3929        AAU CPH 
//
//libraries 
//#include <Adafruit_ESP8266.h>
#include <CapacitiveSensor.h> // to use pins as capacitative touch sensors
#include <EEPROM.h>
#include <string.h>
char data[500];
int currentIndex = 0;

//pin
CapacitiveSensor cs = CapacitiveSensor(2,4);
String memo = "";
//declaration of variables
unsigned long timeOfPress = 0;
unsigned long timeOfLastIr = 0;
unsigned long timeOfLastPress = 0;

bool wasPressed = false;

bool isReceiving = false;

bool sensorWorking = true;

//led variables
int ledState = 0;   // the led starts in no state
int ledPin = 5;     //pin the led is attached to

void setup(){
    Serial.begin(9600);
    pinMode(ledPin, OUTPUT);
    pinMode(8, OUTPUT);
    pinMode(9, OUTPUT);
}

void loop(){
    //reset variabels
    ledState = 0;
    bool isPressed = false;

    unsigned long time = millis();
    timeOfLastIr = time;

    if(sensorWorking){
        long capReading = cs.capacitiveSensor(10);
        //Serial.println("    Reading : " + String(capReading));
        if(capReading > 50){// if being pressed, turn on the light
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
            }
        }
    }
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
    sendMsg("gettabs");
    sensorWorking = false;
}

void openTabs(){
    sendMsg("opentabs");
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
