
//Include libraries 
#include <SPI.h>
#include <SoftwareSerial.h>
#include <Canbus.h>


//SIM800 TX is connected to Arduino D8
#define SIM800_TX_PIN 8
 
//SIM800 RX is connected to Arduino D7
#define SIM800_RX_PIN 7
 
//Create software serial object to communicate with SIM800
SoftwareSerial serialSIM800(SIM800_TX_PIN,SIM800_RX_PIN);


// Define Joystick connection pins 
#define UP     A1
#define DOWN   A3
#define LEFT   A2
#define RIGHT  A5
#define CLICK  A4

//Define LED pins
//#define LED2 8
//#define LED3 7



//Declare CAN variables for communication
char *EngineRPM;
char *Speed;
char *Cool_Temp;
char *Throttle;
char *MAF;
char* r_id= (char*) malloc(sizeof(char)*5);
char* v_id= (char*) malloc(sizeof(char)*7);
char* lat= (char*) malloc(sizeof(char)*8);
char* lon= (char*) malloc(sizeof(char)*8);

char buffer1[64];  //Data will be temporarily stored to this buffer before being written to the file
char buffer2[64];
char buffer3[64];
char buffer4[64];
char buffer5[64];
char buffer6[64];
char buffer7[64];
char buffer8[64];
char buffer9[64];
char buffer10[64];

int i,i1,i2,i3,i4;

//********************************Setup Loop*********************************//
void setup() {
  //initialize vehicle identifiers
  strcpy(r_id,"123"); // route id
  strcpy(v_id,"ATB1837"); // vehicle id
  
  strcpy(lat,"12.333"); // example lat
  strcpy(lon,"11.444"); // example lon
  
  //Initialize Serial communication for debugging
  
  Serial.begin(115200);
  Serial.println("ECU Demo");

  //Initialize pins as necessary
  pinMode(CLICK,INPUT);
  
  //Pull analog pins high to enable reading of joystick movements
  digitalWrite(CLICK, HIGH);
  
  
  //Initialize CAN Controller 
  if(Canbus.init(CANSPEED_500))  /* Initialize MCP2515 CAN controller at the specified speed */
  {
    Serial.println("CAN Init Ok");
    delay(1500);
  } 
  else
  {
    Serial.println("Can't init CAN");
    return;
  } 

  
  //Being serial communication with Arduino and SIM800
  serialSIM800.begin(9600);
  delay(1000);
  Serial.println("Serial communication with SIM800 begins");

  //gprs opens
  open_gprs();
  
  //http_initalization
  init_http();

  
  //Print menu to PC screen
  Serial.println("Click to begin");
  Serial.println("Logging Data");
 
  while(digitalRead(CLICK)==HIGH)
  {
     //Wait for user to click joystick to begin logging
  }

  Serial.println("Start requesting..."); 
  
  delay(1000); 

}

//********************************Main Loop*********************************//
void loop(){
  int timeout;
  while(digitalRead(CLICK)==HIGH){
     
     // digitalWrite(LED3, HIGH); //Turn on LED to indicate CAN Bus traffic
      
       
                                //=======> Requests <=============
                                
        for(i=0;i<100;i++)   {             
        if(Canbus.ecu_req(VEHICLE_SPEED, buffer1)==1){
        Speed=buffer1;
        Serial.print("Speed ok");
        Serial.println(i);
        i1=i;    
         break;
        }   
        else{timeout++;
        }
        delay(10);
        } 
     

     for(i=0;i<100;i++){
      if(Canbus.ecu_req(ENGINE_COOLANT_TEMP, buffer3)==1){
           Cool_Temp=buffer3;
           Serial.print("Temp ok");
           Serial.println(i);
           i3=i;
           break;
                }      
        else{timeout++;
                }
           delay(10);     
        }
   

       
     for(i=0;i<100;i++){
   if(Canbus.ecu_req(ENGINE_RPM, buffer2)==1){ //timeout 7 ms
        EngineRPM=buffer2;
        Serial.print("RPM ok");
        Serial.println(i);
        i2=i;
        break;
        }     
        else{timeout++;
        
        }
        delay(10);
          }    
     
     for(i=0;i<100;i++){
      if(Canbus.ecu_req(THROTTLE, buffer5)==1){
             Throttle=buffer5;
             Serial.print("THROTTLE ok ");
              Serial.println(i);
              i4=i;
             break;
        }      
         else{timeout++;
         }
         delay(10);
        }
    
  
     

      //=========> Parse values to check <==============

    /*  sprintf(buffer2,"2005");// -------> na allaxw apo sprintf stis libs
      EngineRPM = buffer2;
      sprintf(buffer1,"10");
      Speed=buffer1;
      sprintf(buffer5,"15");
      Throttle=buffer5;
      sprintf(buffer3,"99");
      Cool_Temp=buffer3;*/
      sprintf(buffer4,"81");
      MAF=buffer4;
      
      delay(10);
     
                                   

      Serial.print("Engine RPM: "); //Serial Print
      Serial.println(EngineRPM);
      
      Serial.print("Speed: "); 
      Serial.println(Speed);

      Serial.print("Throttle: "); 
      Serial.println(Throttle);  

      Serial.print("Cool_Temp: "); 
      Serial.println(Cool_Temp);

      Serial.print("MAF_Pressure: "); 
      Serial.println(MAF);
      Serial.println();
                                
      //char r_id[],char v_id[],char cool_temp[],char maf_pre[],char rpm[],char sp[],char thr[],char lat[],char lon[]
      upload(r_id,v_id,Cool_Temp,MAF,EngineRPM,Speed,Throttle,lat,lon);       
        
     // digitalWrite(LED3, LOW); //Turn off LED3
      delay(500); 
      
 
      }
  Serial.print("Logging stopped.");
  while(1); //Stop logging if joystick pressed
  
}

/////open-gprs function////////

void open_gprs(){
 serialSIM800.write("at+cgatt=0\r\n");
delay(1000);
serialSIM800.write("at+sapbr=3,1,\"CONTYPE\",\"GPRS\"\r\n");
delay(1000);
serialSIM800.write("at+sapbr=3,1,\"APN\",\"INTERNET\"\r\n");
delay(1000);
serialSIM800.write("at+sapbr=1,1\r\n");
delay(1000);
serialSIM800.write("at+sapbr=2,1\r\n");
delay(5000);
Serial.println("GPRS OK");
  }

////init_http function/////
void init_http(){
  serialSIM800.write("at+httpinit\r\n");
  delay(1000);
  serialSIM800.write("at+httppara=\"url\",\"http://billioserver.dvrdns.org:500/postdata\"\r\n");
  delay(1000);
  serialSIM800.write("at+httppara=\"CID\",1\r\n");
  delay(1000);
  serialSIM800.write("AT+HTTPPARA=\"CONTENT\",\"application/json\"\r\n");
  delay(2000);
  Serial.println("HTTP_INIT OK");
  }  

////upload function/////
void upload(char r_id[],char v_id[],char cool_temp[],char maf_pre[],char rpm[],char sp[],char thr[],char lat[],char lon[]){
  serialSIM800.write("AT+HTTPDATA=150,1000\r\n");//2000
  delay(500); 
  
  serialSIM800.write("{\"r_id\":\"");
  serialSIM800.write(r_id);
  serialSIM800.write("\",\"v_id\":\"");
  serialSIM800.write(v_id);
  serialSIM800.write("\",\"cool_temp\":\"");
  serialSIM800.write(cool_temp);
  serialSIM800.write("\",\"maf_pre\":\"");
  serialSIM800.write(maf_pre);
  serialSIM800.write("\",\"rpm\":\"");
  serialSIM800.write(rpm);
  serialSIM800.write("\",\"speed\":\"");
  serialSIM800.write(sp);
  serialSIM800.write("\",\"thr\":\"");
  serialSIM800.write(thr);
  serialSIM800.write("\",\"lat\":\"");
  serialSIM800.write(lat);
  serialSIM800.write("\",\"lon\":\"");
  serialSIM800.write(lon);
  serialSIM800.write("\"}");
  serialSIM800.write("\r\n");    
 // "r_id":"123","v_id":"ATB1837","cool_temp":"100","maf_pre":"65","rpm":"4003","speed":"180","thr":"34","lat":"23.5555","lon":"24.8742"
  delay(1500); //3000 2000 1000
  serialSIM800.write("AT+HTTPACTION=1\r\n");
  delay(2000); //2000---> max 5 secs
  serialSIM800.write("AT+HTTPREAD\r\n");
  delay(1000);
  Serial.println("UPLOAD OK");
  }
