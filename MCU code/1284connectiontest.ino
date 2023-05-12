#include <DFRobot_SIM7000.h>
#include <Base64.h>
#include <OneWire.h>
#include "OneWire.h"
//#include "DallasTemperature.h"
//#include <SD.h>

//OneWire ds(2);
//DallasTemperature tempSensor(&ds);

#define PIN_TX     11
#define PIN_RX     10
SoftwareSerial     mySerial(PIN_RX,PIN_TX);
DFRobot_SIM7000    sim7000(&mySerial);
//unsigned long long time = 1668670372826;

//unsigned char jsonString[] = "{\"time\":\"1668670372826\",\"temp\":77.75,\"tb\":1962.658,\"tds\":34.03,\"cdt\":68.06}";
//unsigned char base64String[1];
volatile double flow;
unsigned long long time = 1668670372826;  
  float temp = 20;
  float tb = 30;
  float tds = 40;
byte connection;                //determine if mcu cellular is working (ie toggle sd module)

void setup() {
  //pinMode(1,INPUT);
  Serial.begin(9600);
  mySerial.begin(19200);
  Serial.println(F("Turn ON SIM7000......"));
  if(sim7000.turnON()){                             //Turn ON SIM7000
    Serial.println(F("Successfully turned on"));
    connection = 1;
  } else {
    Serial.println(F("Failed to turn on!"));
    connection = 0;
  }

  Serial.println(F("Set baud rate......"));
  while(1){
    if(sim7000.setBaudRate(9600)){               //Set SIM7000 baud rate from 115200 to 19200 reduce the baud rate to avoid distortion
      Serial.println(F("Set baud rate:9600"));
      connection = 1;
        break;
      }else{
        Serial.println(F("Faile to set baud rate"));
        connection = 0;
        delay(1000);
      }
  }
  mySerial.begin(9600);

  
  if(sim7000.attacthService()){
    Serial.println(F("Attatched to Service"));
    connection = 1;
  } else {
    Serial.println(F("Fail to Attatch to service"));
    connection = 0;
  }

}

void loop() {
 
  //sprintf(message, "{\"k\":\"5#p7ILQi\",\"d\":\"%s\",\"t\":\"test\"}", base64String);
  char message[162] = "{\"k\":\"5#p7ILQi\",\"d\":\"eyJ0aW1lIjoxNjY4NjcxMjcyODI2LCJ0ZW1wIjo0NC42MCwiZmxvdyI6IDAuMDAsImNkdCI6NDYuMDAsInRiIjogICA5LjAwLCJ0ZHMiOjIzLjAwfQ==\",\"t\":\"test\"}";
  Serial.flush();


  
  sendmsg(message); 


  delay(60000);
  
}
void sendmsg(char *message){
  if(sim7000.openNetwork(sim7000.eTCP, "cloudsocket.hologram.io", 9999)){
    Serial.println(F("Connect OK"));
  } else {
    Serial.println(F("Fail to connect"));
    while (1);
  }
  delay(3000);

  if(sim7000.send(message)){
    Serial.println(F("Sent OK"));
  } else {
    Serial.println(F("Fail to send"));
    while (1);
  }
 
}
