//________________THE TAB-SAVEr__________________
//Real-Time Interfaces & Interaction miniproject
//Rasmus Kruuse          Student nr.2016-3929        AAU CPH 
//

//libraries 
#include <CapacitiveSensor.h> // to use pins as capacitative touch sensors
#include <EEPROM.h>


//pin
CapacitiveSensor cs = CapacitiveSensor(2,4);
int i = 0;
String memo = "";
//declaration of variables
unsigned long timeOfPress = 0;
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
    i++;
    //Serial.print(String(i) + " - ");
    ledState = 0;
    bool isPressed = false;


    long capReading = cs.capacitiveSensor(30);
    unsigned long time = millis();

    

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
    Serial.print(msg +  "over");


    digitalWrite(8,LOW);

    if(isReceiving){
        digitalWrite(8,HIGH);
        memo += msg;
    }

    if( msg == "done"){
        isReceiving = false;
    }

    if( msg == "sending"){
        isReceiving = true;
    }

    if( msg == "read"){
        Serial.print("inMemory:" + memo + "over");
    }
    
}

void led(int state){
    digitalWrite(ledPin,LOW);
    if(state == 1){
        digitalWrite(ledPin,HIGH);
    }
}

void performAction(unsigned long duration){ //perform action based on duration of press
    if(duration> 200 && duration < 600){
        digitalWrite(8, HIGH);
    }else if(duration > 1500){
        digitalWrite(9,HIGH);
        receiveList();
    }
}

void receiveList(){
    Serial.print("gettabsover");
}