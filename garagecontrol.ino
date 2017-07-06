#define DEBUG               true     // Turns logging to serial on and off
#define relayActivationTime 250      // Time to leave garage door relay ON
#define doorOpenCloseTime   10000    // Time for door to open or close     

//Spark commands
int Open_Door(String command);

#define Door_SensorOpen_Pin    D0
#define Door_SensorClosed_Pin  D1
#define Door_Relay_Pin         D6

bool Door_SensorOpen        = false;
bool Door_SensorClosed      = false;

int Door_State             = 0;
int Door_State_Previous    = 0;

Timer Door_Timer(60000, publishDoor);


// ********************************************************************
// SETUP
//
// Initializes program and registers Particle functions
// ********************************************************************
void setup() 
{
    if(DEBUG)
    {
        Serial.begin(9600); 
    }
  
    //Register Particle functions
    Particle.function("getDoor", getDoor);
 
    //Register Particle variables (not currently used)
    Particle.variable("Door_State", "closed");

    pinMode(Door_Relay_Pin, OUTPUT);
    pinMode(Door_SensorOpen_Pin, INPUT);
    pinMode(Door_SensorClosed_Pin, INPUT);
}


// ********************************************************************
// LOOP
//
// Main program
// ********************************************************************
void loop()
{
    getDoorState();
    
    if(Door_State != Door_State_Previous) 
    {
        publishDoor();
    }

    Door_State_Previous = Door_State;

}


// ********************************************************************
// Read Door  State
//
// Door is the garage door. Sensor returns 0 when contact is active
// 0 = Closed
// 1 = Open
// 2 = Partially open/closed
// 3 = Error
// ********************************************************************
int getDoorState(){
    Door_SensorOpen    = digitalRead(Door_SensorOpen_Pin);
    Door_SensorClosed  = digitalRead(Door_SensorClosed_Pin);

    if(Door_SensorClosed && !Door_SensorOpen)
    {  
        Door_State = 0;
    }
    else if(!Door_SensorClosed && Door_SensorOpen)
    {
        Door_State = 1;
    }
    else if(!Door_SensorClosed && !Door_SensorOpen)
    {
        Door_State = 2;
    }
    else
    {
        Door_State = 3;
    }
    
    return Door_State;
}


// ********************************************************************
// Read Door State
// ********************************************************************
int getDoor(String args){
    getDoorState();
    return (int)Door_State;
}


// ********************************************************************
// Control Door
//
// Send the following to control the main garage door
// 0 = Close
// 1 = Open
// 3 = Error
// ********************************************************************
int putDoor(String command)
{
    int before = getDoorState();
    
    if (command == "0" && Door_State == 0){
        Door_State = 0;
    }    
    
    else if (command == "1" && Door_State == 1){
        Door_State = 1;
    }
    
    else if (Door_State == 3){
        return 3;
    }
    
    else if (command == "0" && Door_State != 0){    
        digitalWrite(Door_Relay_Pin, LOW);        
        delay(relayActivationTime);
        digitalWrite(Door_Relay_Pin, HIGH);             //Automatically turn off after a half second, need to test timing, but this is a first guess
    } 
    else if (command == "1" && Door_State != 1){
        digitalWrite(Door_Relay_Pin, LOW);        
        delay(relayActivationTime);
        digitalWrite(Door_Relay_Pin, HIGH);    
    }
    
    delay(doorOpenCloseTime);
    
    int after = getDoorState();
    
    if (before != after){
        if(command == "0"){
            Particle.variable("Door_State", "closed");
            Particle.publish("GarageDoor_Closed", NULL, 60, PRIVATE);
            Door_State = 0;
        }
        else if(command == "1"){
            Particle.variable("Door_State", "open");
            Particle.publish("GarageDoor_Opened", NULL, 60, PRIVATE);
            Door_State = 1;
        }
    }
    else{
        Door_State = 3;
    }
    
    return Door_State;
}



// ********************************************************************
// Publish Door
//
// Sends the door state to the Particle cloud
// 0 = Closed
// 1 = Open
// 2 = Partially open/closed
// 3 = Error
// ********************************************************************
void publishDoor()
{
    switch (getDoorState())
    {
        case 0:
            Particle.publish("Door_State","Closed",60,PRIVATE);
            break;
        
        case 1:
            Particle.publish("Door_State","Open",60,PRIVATE);
            break;
        
        case 2:
            Particle.publish("Door_State","Partial",60,PRIVATE);
            break;
        
        case 3:
            Particle.publish("Door_State","Error",60,PRIVATE);
            break;    
    }
}
