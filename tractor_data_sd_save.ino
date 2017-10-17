//Include libraries
#include <SPI.h>
#include <defaults.h>
#include <global.h>
#include <mcp2515.h>
#include <mcp2515_defs.h>
#include <Canbus.h>
#include <SD.h>


//Initialize uSD pins
const int chipSelect = 9;


//Declare SD File
File dataFile;


// Define Joystick connection pins
#define UP     A1
#define DOWN   A3
#define LEFT   A2
#define RIGHT  A5
#define CLICK  A4

//Define LED pins
#define LED2 8
#define LED3 7



//Declare CAN variables for communication
char *EngineRPM;
char *Speed;
char *Cool_Temp;
char *Throttle;
char *MAF;

char buffer1[64];  //Data will be temporarily stored to this buffer before being written to the file
char buffer2[64];
char buffer3[64];
char buffer4[64];
char buffer5[64];


unsigned long time1;
unsigned long time2;
unsigned long time3;
unsigned long time4;
unsigned long time5;
unsigned long curtime;
int i1,i2,i3,i4,i;
//********************************Setup Loop*********************************//
void setup() {
  //Initialize Serial communication for debugging
  Serial.begin(115200);
  Serial.println("ECU Demo");
  
  //Initialize pins as necessary
  pinMode(chipSelect, OUTPUT);
  pinMode(CLICK,INPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
 
  //Pull analog pins high to enable reading of joystick movements
  digitalWrite(CLICK, HIGH);
 
  //Write LED pins low to turn them off by default
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
 
  //Initialize CAN Controller
  if(Canbus.init(CANSPEED_250))  /* Initialize MCP2515 CAN controller at the specified speed */
  {
    Serial.println("CAN Init Ok");
    delay(1500);
  }
  else
  {
    Serial.println("Can't init CAN");
    return;
  }

//Check if uSD card initialized
  if (!SD.begin(chipSelect)) {
    Serial.println("uSD card failed to initialize, or is not present");
    //return;
  }
  else{
      Serial.println("uSD card initialized.");
      delay(1500);
  }
 
  //Print menu to PC screen
  Serial.println("Click to begin");
  Serial.println("Logging Data");
 
  while(digitalRead(CLICK)==HIGH)
  {
     //Wait for user to click joystick to begin logging
  }

  delay(1000);
  //setup first time
 
  time1=millis();
  time2=millis();
  time3=millis();
  time4=millis();
  time5=millis();
  curtime=millis();
 
 

}

//********************************Main Loop*********************************//
void loop(){

 //========> Open SD card <========
                            
        File  dataFile = SD.open("datatr1.txt", FILE_WRITE); //Open uSD file to log data
    
      //If data file can't be opened, throw error.
      if (!dataFile){
        Serial.println("Error opening");
        Serial.println("datatr1.txt");}
 
  int j=1;
 long attempts=0;
 long timeout=0;  //timeout=7-8 millisecs
  while(digitalRead(CLICK)==HIGH){
    
      digitalWrite(LED3, HIGH); //Turn on LED to indicate CAN Bus traffic
     
     // Serial.println("Start requesting...");
      
                                //=======> Requests <=============
           tCAN message;
if (mcp2515_check_message())
  {
    if (mcp2515_get_message(&message))
  {
        //if(message.id == 0x620 and message.data[2] == 0xFF)  //uncomment when you want to filter
             //{
              
               Serial.print("ID: ");
               Serial.print(message.id,HEX);
               Serial.print(", ");
               Serial.print("Data: ");
               Serial.print(message.header.length,DEC);

               dataFile.print("ID: ");
               dataFile.print(message.id,HEX);
               dataFile.print(", ");
               dataFile.print("Data: ");
               dataFile.print(message.header.length,DEC);
              
               for(int i=0;i<message.header.length;i++)
                {
                  Serial.print(message.data[i],HEX);
                  Serial.print(" ");

                  dataFile.print(message.data[i],HEX);
                  dataFile.print(" ");
                }
               Serial.println("");
               dataFile.println("");
               dataFile.flush(); 
       
  }
  }
  }
       
        
   
     
     
      //======> close SDcard <=======
       //// dataFile.print("attempts: ");
        //dataFile.println(attempts);
        //dataFile.print("timeouts: ");
       // dataFile.println(timeout);
       
       
        dataFile.flush();
        dataFile.close();   //Close data logging file
       
  Serial.print("Logging stopped.");
  while(1); //Stop logging if joystick pressed
 
}
