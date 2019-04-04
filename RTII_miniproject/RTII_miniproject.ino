#include <Adafruit_ESP8266.h>

//________________THE TAB-SAVEr__________________
//Real-Time Interfaces & Interaction miniproject
//Rasmus Kruuse          Student nr.2016-3929        AAU CPH 
//

//libraries 
#include <CapacitiveSensor.h> // to use pins as capacitative touch sensors
#include <EEPROM.h>


//pin
CapacitiveSensor cs = CapacitiveSensor(2,4);
String memo = "";
//declaration of variables
unsigned long timeOfPress = 0;

unsigned long timeOfLastIr = 0;

bool wasPressed = false;

bool isReceiving = false;

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


    long capReading = cs.capacitiveSensor(15);
    unsigned long time = millis();
    Serial.println(time - timeOfLastIr);
    timeOfLastIr = time;

 
    if(capReading > 1000){// if being pressed, turn on the light
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

    String msg = Serial.readString();

    // if(msg != ""){
    //     sendMsg(msg); //if a message was received echo it back
        
    //     //use an LED to indicate if it is receiving 
    //     if(isReceiving){
    //         digitalWrite(8,HIGH);
    //         memo += msg + ' ';
    //     }else{
    //         digitalWrite(8,LOW); 
    //     }
    // }

    // if( msg == "done"){ //if we receive "done" stop saving mesages and save list of msg in EEPROM memory
    //     isReceiving = false;
    //     writeString(10,memo + ' ');
    //     memo = "";
    // }

    // if( msg == "sending"){  //sending indicates that we should save the next msg's in memory
    //     isReceiving = true;
    // }

    // if( msg == "read"){ //read means that we should send the saved list of open tabs to python
    //     String stringToSend;
    //     stringToSend = read_String(10);
    //     sendMsg("inMemory:" + stringToSend + "||");
    // }
    
}

void led(int state){
    digitalWrite(ledPin,LOW);
    if(state == 1){
        digitalWrite(ledPin,HIGH);
    }
}

void performAction(unsigned long duration){ //perform action based on duration of press
    if(duration> 200 && duration < 1000){
        digitalWrite(8, HIGH);
        openTabs();
    }else if(duration > 1500){
        digitalWrite(9,HIGH);
        openTabs();
        //receiveList();
    }
}

void receiveList(){
    sendMsg("gettabs");
}

void openTabs(){
    sendMsg("opentabs");
}

void writeString(char add,String data)
{
    int _size = data.length();
    int i;
    for(i=0;i<_size;i++)
    {
        EEPROM.write(add+i,data[i]);
    }
    EEPROM.write(add+_size,'\0');   //Add termination null character for String Data
}
 
 
String read_String(char add)
{
  int i;
  char data[100]; //Max 100 Bytes
  int len=0;
  unsigned char k;
  k=EEPROM.read(add);
  while(k != '\0' && len<500)   //Read until null character
  {    
    k=EEPROM.read(add+len);
    data[len]=k;
    len++;
  }
  data[len]='\0';
  return String(data);
}

void sendMsg(String msgToSend){
    Serial.print(msgToSend + "over");
}