//Include statements
#include "Adafruit_DHT/Adafruit_DHT.h"

#define DHTPIN A0     
#define DHTTYPE DHT11

bool DEBUG                  = true;     // Turns logging to serial on and off
int  relayActivationTime    = 250;      // Time to leave garage door relay ON
int  doorOpenCloseTime      = 10000;    // Time for door to open or close     

int loopTime;
int lastLoop;

float temp;
float humidity;
int light;

//Declare DHT
DHT dht(DHTPIN, DHTTYPE);

//Spark commands
int Open_Door1(String command);
int Open_Door2(String command);

int lightSensorPin          = A1;
int Door1_SensorTop_Pin     = D0;
int Door1_SensorBottom_Pin  = D1;
int Door2_SensorTop_Pin     = D2;
int Door2_SensorBottom_Pin  = D3;
int DoorH_Sensor_Pin        = D4;
int DoorS_Sensor_Pin        = D5;
int Door1_Relay_Pin         = D6;
int Door2_Relay_Pin         = D7;

bool Door1_SensorTop        = false;
bool Door1_SensorBottom     = false;
bool Door2_SensorTop        = false;
bool Door2_SensorBottom     = false;
bool DoorH_Sensor           = false;
bool DoorS_Sensor           = false;

int Door1_State             = 0;
int Door2_State             = 0;
int DoorH_State             = 0;
int DoorS_State             = 0;

int Door1_State_Previous    = 0;
int Door2_State_Previous    = 0;
int DoorH_State_Previous    = 0;
int DoorS_State_Previous    = 0;

Timer Door1_Timer(60000, publishDoor1);
Timer Door2_Timer(60000, publishDoor2);
Timer DHT_Timer(60000, updateDHT);
Timer Sensor_Timer(1000,updateSensors);

// ********************************************************************
// SETUP
//
// Initializes program and registers Particle functions
// ********************************************************************
void setup() 
{
    if(DEBUG){
        Serial.begin(9600); 
    }
  
    dht.begin();  
    
    //Register Particle functions
    Particle.function("getTemp", getDHTTemperature);
    Particle.function("getHum", getDHTHumidity);
    Particle.function("getDoor1", getDoor1);
    Particle.function("getDoor2", getDoor2);
    //Particle.function("getDoorH", getDoorHState);
    //Particle.function("getDoorS", getDoorSState);
    //Particle.function("putDoor1", putDoor1);
    //Particle.function("putDoor2", putDoor2);
    Particle.function("getLight", getLightSensor);
  

    //Register Particle variables (not currently used)
    Particle.variable("Door1_State", "closed");
    Particle.variable("Door2_State", "closed");
    Particle.variable("DoorH_State", "closed");
    Particle.variable("DoorS_State", "closed");
  
    pinMode(Door1_Relay_Pin, OUTPUT);
    pinMode(Door2_Relay_Pin, OUTPUT);
  
    pinMode(Door1_SensorTop_Pin, INPUT);
    pinMode(Door1_SensorBottom_Pin, INPUT);
    pinMode(Door2_SensorTop_Pin, INPUT);
    pinMode(Door2_SensorBottom_Pin, INPUT);
    pinMode(DoorH_Sensor_Pin, INPUT);
    pinMode(DoorS_Sensor_Pin, INPUT);
    
    //Door1_Timer.start();
    //Door2_Timer.start();
    DHT_Timer.start();
}


// ********************************************************************
// LOOP
//
// Main program
// ********************************************************************
void loop()
{
    getDoor1State();
    getDoor2State();

    updateSensors();
    
    if(Door1_State != Door1_State_Previous) publishDoor1();
    if(Door2_State != Door2_State_Previous) publishDoor2();
    if(DoorH_State != DoorH_State_Previous) publishDoorH();
    if(DoorS_State != DoorS_State_Previous) publishDoorS();
    
    
    Door1_State_Previous = Door1_State;
    Door2_State_Previous = Door2_State;
}

// ********************************************************************
// Read DHT Sensor
//
// Returns the temperature and humidity from the DHT
// ********************************************************************
void updateDHT(){
    temp        = dht.getTempFarenheit();
    humidity    = dht.getHumidity();
    light       = analogRead(lightSensorPin)*-.25 + 1000;;
}


// ********************************************************************
// Read Sensors
//
// Updates all digital sensors
// ********************************************************************
void updateSensors(){
    DoorH_Sensor     = digitalRead(DoorH_Sensor_Pin);
    DoorS_Sensor     = digitalRead(DoorS_Sensor_Pin);
}


// ********************************************************************
// Send Temperature
//
// Returns the temperature from the DHT
// ********************************************************************
int getDHTTemperature(String args){
    return (int)temp;
}


// ********************************************************************
// Send Humidity
//
// Returns the humidity from the DHT
// ********************************************************************
int getDHTHumidity(String args){
    return (int)humidity;
}

// ********************************************************************
// Send Light Sensor
// ********************************************************************
int getLightSensor(String args){
    return light;
}

// ********************************************************************
// Read Door 1 State
//
// Door 1 is the main garage door. Sensor returns 0 when contact is active
// 0 = Closed
// 1 = Open
// 2 = Partially open/closed
// 3 = Error
// ********************************************************************
int getDoor1State(){
    Door1_SensorTop    = digitalRead(Door1_SensorTop_Pin);
    Door1_SensorBottom = digitalRead(Door1_SensorBottom_Pin);

    if(!Door1_SensorBottom && Door1_SensorTop){
        Door1_State = 0;
    }
    
    else if(Door1_SensorBottom && !Door1_SensorTop){  
        Door1_State = 1;
    }
    
    else if(!Door1_SensorBottom || Door1_SensorTop){
        Door1_State = 2;
    }
    
    else{
        Door1_State = 3;
    }
    
    return Door1_State;
}


// ********************************************************************
// Read Door 1 State
// ********************************************************************
int getDoor1(String args){
    getDoor1State();
    return (int)Door1_State;
}


// ********************************************************************
// Read Door 2 State
//
// Door 2 is the small garage door
// 0 = Closed
// 1 = Open
// 2 = Partially open/closed
// 3 = Error
// ********************************************************************
int getDoor2State(){
    Door2_SensorTop    = digitalRead(Door2_SensorTop_Pin);
    Door2_SensorBottom = digitalRead(Door2_SensorBottom_Pin);

    if(!Door2_SensorBottom && Door2_SensorTop){
        Door2_State = 0;
    }
    
    else if(Door2_SensorBottom && !Door2_SensorTop){
        Door2_State = 1;
    }
    
    else if(!Door2_SensorBottom || Door2_SensorTop){
        Door2_State = 3;
    }
    
    else{
        Door2_State = 4;
    }
    
    return Door2_State;
}


// ********************************************************************
// Read Door 2 State
// ********************************************************************
int getDoor2(String args){
    getDoor2State();
    return (int)Door2_State;
}


// ********************************************************************
// Read House Door State
//
// Door H is the door from the garage into the house
// 0 = Closed
// 1 = Open
// ********************************************************************
int getDoorHState(void){
    DoorH_Sensor = digitalRead(DoorH_Sensor_Pin);

    if(DoorH_Sensor == false){
        DoorH_State = 0;
    }
    
    else if(DoorH_Sensor == true){
        DoorH_State = 1;
    }
    
    else{
        DoorH_State = 2;
    }
    
    return DoorH_State;
}


// ********************************************************************
// Read Service Door State
//
// Door S is the service door from the garage to outside
// 0 = Closed
// 1 = Open
// ********************************************************************
int getDoorSState(void){
    DoorS_Sensor = digitalRead(DoorS_Sensor_Pin);

    if(DoorS_Sensor == false){
        DoorS_State = 0;
    }
    
    else if(DoorS_Sensor == true){
        DoorS_State = 1;
    }
    
    else{
        DoorS_State = 2;
    }
    
    return DoorS_State;
}


// ********************************************************************
// Control Door 1
//
// Send the following to control the main garage door
// 0 = Close
// 1 = Open
// 3 = Error
// ********************************************************************
int putDoor1(String command)
{
    int before = getDoor1State();
    
    if (command == "0" && Door1_State == 0){
        Door1_State = 0;
    }    
    
    else if (command == "1" && Door1_State == 1){
        Door1_State = 1;
    }
    
    else if (Door1_State == 3){
        return 3;
    }
    
    else if (command == "0" && Door1_State != 0){    
        digitalWrite(Door1_Relay_Pin, LOW);        
        delay(relayActivationTime);
        digitalWrite(Door1_Relay_Pin, HIGH);             //Automatically turn off after a half second, need to test timing, but this is a first guess
    } 
    else if (command == "1" && Door1_State != 1){
        digitalWrite(Door1_Relay_Pin, LOW);        
        delay(relayActivationTime);
        digitalWrite(Door1_Relay_Pin, HIGH);    
    }
    
    delay(doorOpenCloseTime);
    
    int after = getDoor1State();
    
    if (before != after){
        if(command == "0"){
            Particle.variable("Door1_State", "closed");
            Particle.publish("GarageDoor1_Closed", NULL, 60, PRIVATE);
            Door1_State = 0;
        }
        else if(command == "1"){
            Particle.variable("Door1_State", "open");
            Particle.publish("GarageDoor1_Opened", NULL, 60, PRIVATE);
            Door1_State = 1;
        }
    }
    else{
        Door1_State = 3;
    }
    
    return Door1_State;
}


// ********************************************************************
// Control Door 2
//
// Send the following to control the main garage door
// 0 = Close
// 1 = Open
// 4 = Error
// ********************************************************************
int putDoor2(String command)
{
    int before = getDoor2State();
    
    if (command == "0" && Door2_State == 0){
        Door2_State = 0;
    }    
    
    else if (command == "1" && Door2_State == 1){
        Door2_State = 1;
    }
    
    else if (Door2_State == 3){
        return 3;
    }
    
    else if (command == "0" && Door2_State != 0){    
        digitalWrite(Door2_Relay_Pin, LOW);        
        delay(relayActivationTime);
        digitalWrite(Door2_Relay_Pin, HIGH);             //Automatically turn off after a half second, need to test timing, but this is a first guess
    } 
    else if (command == "1" && Door1_State != 1){
        digitalWrite(Door2_Relay_Pin, LOW);        
        delay(relayActivationTime);
        digitalWrite(Door2_Relay_Pin, HIGH);    
    }
    
    delay(doorOpenCloseTime);
    
    int after = getDoor2State();
    
    if (before != after){
        if(command == "0"){
            Particle.variable("Door2_State", "closed");
            Particle.publish("GarageDoor2_Closed", NULL, 60, PRIVATE);
            Door2_State = 0;
        }
        else if(command == "1"){
            Particle.variable("Door2_State", "open");
            Particle.publish("GarageDoor2_Opened", NULL, 60, PRIVATE);
            Door2_State = 1;
        }
    }
    else{
        Door2_State = 3;
    }
    
    return Door2_State;
}


// ********************************************************************
// Publish Door 1
//
// Sends the door state to the Particle cloud
// 0 = Closed
// 1 = Open
// 2 = Partially open/closed
// 3 = Error
// ********************************************************************
void publishDoor1()
{
    switch (getDoor1State())
    {
        case 0:
            Particle.publish("Door1_State","Closed",60,PRIVATE);
            break;
        
        case 1:
            Particle.publish("Door1_State","Open",60,PRIVATE);
            break;
        
        case 2:
            Particle.publish("Door1_State","Partial",60,PRIVATE);
            break;
        
        case 3:
            Particle.publish("Door1_State","Error",60,PRIVATE);
            break;    
    }
}


// ********************************************************************
// Publish Door 2
//
// Sends the door state to the Particle cloud
// 0 = Closed
// 1 = Open
// 2 = Partially open/closed
// 3 = Error
// ********************************************************************
void publishDoor2()
{
    switch (getDoor2State())
    {
        case 0:
            Particle.publish("Door2_State","Closed",60,PRIVATE);
            break;
        
        case 1:
            Particle.publish("Door2_State","Open",60,PRIVATE);
            break;
        
        case 2:
            Particle.publish("Door2_State","Partial",60,PRIVATE);
            break;
        
        case 3:
            Particle.publish("Door2_State","Error",60,PRIVATE);
            break;    
    }
}


// ********************************************************************
// Publish House Door State
// ********************************************************************
void publishDoorH()
{
    switch (getDoorHState())
    {
        case 0:
            Particle.publish("DoorH_State","Closed",60,PRIVATE);
            break;
        
        case 1:
            Particle.publish("DoorH_State","Open",60,PRIVATE);
            break;
    }
}


// ********************************************************************
// Publish Service Door State
// ********************************************************************
void publishDoorS()
{
    switch (getDoorSState())
    {
        case 0:
            Particle.publish("DoorS_State","Closed",60,PRIVATE);
            break;
        
        case 1:
            Particle.publish("DoorS_State","Open",60,PRIVATE);
            break;
    }
}