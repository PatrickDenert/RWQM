#include <DFRobot_SIM7000.h>
#include <Base64.h>
#include <OneWire.h>
#include "OneWire.h"
#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include <stdlib.h>     /* strtoul */

#define PIN_TX     11
#define PIN_RX     10
SoftwareSerial     mySerial(PIN_RX,PIN_TX);
DFRobot_SIM7000    sim7000(&mySerial);

OneWire ds(3);

static char        buff[1500];  //response buffer for http requests

unsigned long long time;
float averageVoltage = 0,tdsValue = 0;
volatile double flow;
volatile double flowCalc;       //store flow when sensors are read and convert reading

float temp;
float tds;
float tb;
float cdt;

int inputStringLength;
int encodedLen;
int count = 0;

bool reconnect = false;
bool connection;                //determine if mcu cellular is working (ie toggle sd module)

char payloadBuffer[300];

char *ret;
char buff2[22];
const char unixtime[9] = "unixtime";

unsigned long long getTime(){
  Serial.println("time setup");
  int count3 = 0;
  while(1){
    count3 = count3 + 1;                              
    Serial.println(count3);
    Serial.println(F("Turn ON SIM7000......"));
    if(sim7000.turnON()){                             //Turn ON SIM7000
      Serial.println(F("Successfully turned on"));
      connection = true;
      break;
    } else {
      Serial.println(F("Failed to turn on!"));
      connection = false;
      delay(1000);
    }
    if(count3 == 3){                                 //try to connect to cellular 3 times before switching to offline mode
      reconnect = true;
      break;
    }
   
   }
  
  if(connection == true){
    if(sim7000.setBaudRate(19200)){                  //Set SIM7000 baud rate from 115200 to 19200 reduce the baud rate to avoid distortion
      Serial.println(F("Set baud rate:19200"));
      connection = true;
      }else{
        Serial.println(F("Failed to set baud rate"));
        connection = false;
        delay(1000);
      }
  }
  while(connection == true){
     if(sim7000.attacthService()){
        Serial.println(F("Attatched to Service"));
        connection = true;
        break;
     } else {
        Serial.println(F("Fail to Attatch to service"));
      connection = false;
     }
  }

  delay(200);
  if(connection == true){
  Serial.println("Connecting......");
  if (sim7000.openNetwork(sim7000.eTCP, "worldtimeapi.org", 80)) {  //Start Up TCP or UDP Connection to time api
    Serial.println("Connect OK");
  } else {
    Serial.println("Fail to connect");
    while (1);
  }
  sim7000.send("GET http://worldtimeapi.org/api/timezone/America/New_York HTTP/1.1\r\nAccept:application/json\r\nHost:worldtimeapi.org\r\n\r\n");    //Send Data Through TCP or UDP Connection GET /api/timezone/America/New_York HTTP/1.0
  int dataNum = sim7000.recv(buff, 1500);                           //Receive data
  delay(500);
  
  ret = strstr(buff, unixtime);                                     //remove response headers from buffer
  sprintf(buff2,"%s\n", ret);
  
  delay(1000);
  char buff3[11];
  
  char subString[20];

  sprintf(buff3,"%s", getSubString(buff2,                           //extract unixtime from json response
        subString, 10, 10));
        
  time = strtoul(buff3,NULL,10);                                    //convert str to unsigned long
  
  if (sim7000.closeNetwork()) {                                     //End the connection
    delay(1000);
    Serial.println("Close connection");
  } else {
    Serial.println("Fail to close connection");
  }
  delay(1000);
  Serial.println(F("Turn OFF SIM7000......"));
  delay(1000);
  
  if(sim7000.turnOFF()){                                            //Turn ON SIM7000
    Serial.println(F("Successfully turned off"));
    delay(1000);
    connection = true;
  } else {
    Serial.println(F("Failed to turn off!"));
    connection = false;
  }                                                                 //Turn OFF SIM7000
  }
  time = time * 1000;                                               //convert seconds to milliseconds
  time = time - 14400000;                                           //convert time zone from est to gmt

  return time;
  
}


void pulse(){                                                       //measure the quantity of square wave from flow sensor
  flow += 1.0 / 450.0;
}

char* getSubString(char* inputString, char* subString,              //get substring from inputs (used to extract unix time)
    int index, int subStringLength){  
    int counter, inputStringLength = strlen(inputString);    
 
    if(index < 0 || index > inputStringLength || 
          (index + subStringLength) > inputStringLength){
        printf("Invalid Input");
        return NULL;
    }
    for(counter = 0; counter < subStringLength; counter++){
        subString[counter] = inputString[index++];
    }
    subString[counter] = '\0';
     
    return subString;
}

void setup() {
  SD.begin(0);                        //init chipselect of SD module
             
  pinMode(14,INPUT);                  //low battery circuit input
  attachInterrupt(2, pulse, RISING);  //DIGITAL Pin 2: Interrupt 0

  pinMode(A1,INPUT);                  //input for tds sensor
  
  Serial.begin(9600);                 
  mySerial.begin(19200);              //serial to cell module
  
  time = getTime();                   //get time from api
  delay(1000);
}



void loop() {
  delay(1000);
  if(reconnect == true){              //check if system was previously disconnected from cellular and update time
    time = getTime();
    reconnect = false;
  }
    flow = 0.0;
    //-------------------------------//
    //------ initialize sim7000 -----//
    //-------------------------------//
  
  Serial.println("--main loop--");
  int count2 = 0;
  while(1){
    count2 = count2 + 1;
    Serial.println(count2);
    Serial.println(F("Turn ON SIM7000......"));
    if(sim7000.turnON()){                             //Turn ON SIM7000
      Serial.println(F("Successfully turned on"));
      connection = true;
      break;
    } else {
      Serial.println(F("Failed to turn on!"));
      connection = false;
      delay(1000);
    }
    if(count2 == 3){                                  //try 3 times to connect to cellular and go in offline mode if not
      break;
    }
   
   }
  
  if(connection == true){
    if(sim7000.setBaudRate(19200)){                  //Set SIM7000 baud rate from 115200 to 19200 reduce the baud rate to avoid distortion
      Serial.println(F("Set baud rate:19200"));
      connection = true;
      }else{
        Serial.println(F("Failed to set baud rate"));
        connection = false;
        delay(1000);
      }
  }
  if(connection == true){                                 //attach to cellular service
     if(sim7000.attacthService()){
        Serial.println(F("Attached to Service"));
        connection = true;
     } else {
        Serial.println(F("Fail to Attach to service"));
      connection = false;
     }
  }

  //-------------------------------//
  //-------- Read Sensors ---------//
  //-------------------------------//
  temp = 0.0;                                 //reset sensor readings
  tds = 0.0;
  tb = 0.0;
  cdt = 0.0;
  
  flowCalc = getFlow();                       
    
  time += 45000;                              //update time locally to reduce connections to server when not needed
  //time = getTime();                         //update time if needed
  
  temp = getTemp();
  tds = getTDS(temp);
  tb = getTurbidity();   
  cdt = tds*2.0;                              //cdt calculation
  temp = temp*1.8 + 32;                       //celsius to Fahrenheit
  bool battery = getBattLevel();              //check for low battery
  
 
  //-------------------------------//
  //------ format String ----------//
  //-------------------------------//

  
  JSONdotStringify();                         //turn sensor values into json string 
  //Serial.println(payloadBuffer);

  if(connection == false){                    //if connection false save to sd card
    SDCardWrite(payloadBuffer);
    SDCardRead();
    reconnect = true;                         //set true to try to restart system next loop

    delay(10000);  
  }

  else{

    //-------------------------------//
    //----- Create valid Message ----//
    //-------------------------------//

    inputStringLength = strlen(payloadBuffer);
    encodedLen = Base64.encodedLength(inputStringLength);

    char base64String[encodedLen+1];
    char message[encodedLen+35];
    
    Base64.encode(base64String, payloadBuffer, inputStringLength);                      //base64encode string before sending to server
    
    sprintf(message, "{\"k\":\"5#p7ILQi\",\"d\":\"%s\",\"t\":\"test\"}", base64String); //create message json string before sending to cellular

    //-------------------------------//
    //------- Send Message ----------//
    //-------------------------------//

    sendmsg(message);
    Serial.flush();

    //-------------------------------//
    //------ Send Error Code --------//
    //-------------------------------//

    byte error_code = alert(tds,temp,tb,cdt,battery);               //generate error code
    Serial.print("error code: ");

    Serial.println(error_code);

    if(error_code !=0 ){                                            //if error detected send to server and email
      char * alertmessage;
      alertmessage = (char*) malloc(100);
      sprintf(alertmessage, "{\"k\":\"5#p7ILQi\",\"d\":\"%i\",\"t\":\"alert\"}", error_code);

      delay(1000);
      sendmsg(alertmessage);
      delay(1000);

      free(alertmessage);
    }
  }

  //-------------------------------//
  //------ Turn off sim7000 -------//
  //-------------------------------//

  count++;
  Serial.print("loop number ");
  Serial.println(count);
  Serial.println(" ended");

  if(connection == true){
  Serial.println(F("Turn OFF SIM7000......"));
  delay(1000);
  if(sim7000.turnOFF()){                                                        //Turn ON SIM7000
    Serial.println(F("Successfully turned off"));
    delay(1000);
    connection = true;
  } else {
    Serial.println(F("Failed to turn off!"));
    connection = false;
    delay(1000);
  }
  }
  delay(1000);

}



bool sendmsg(char *message){                                                    //send message to hologram cloud which sends it to server
  if(sim7000.openNetwork(sim7000.eTCP, "cloudsocket.hologram.io", 9999)){
    Serial.println(F("Connect OK"));
  } else {
    Serial.println(F("Fail to connect"));
    return false;
  }
  delay(1000);
  if(sim7000.send(message)){
    Serial.println(F("Sent OK"));
    delay(1000);
  } else {
    Serial.println(F("Fail to send"));
    return false;
   
  }
  sim7000.closeNetwork();
  return true;
}



char *ulltoa(uint64_t value, char *buf, int radix) {                              //unsigned long to char
    char tmp[64 + 1];
    char *p1 = tmp, *p2;
    static const char xlat[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if(radix < 2 || radix > 36)    return nullptr;
    do {*p1++ = xlat[value % (unsigned)radix];} while((value /= (unsigned)radix));
    for(p2 = buf; p1 != tmp; *p2++ = *--p1) ;
    *p2 = '\0';
    return buf;
}

bool JSONdotStringify(){                                                          //convert readings and generate json string
  char timeString[16];
  ulltoa(time, timeString, 10);

  char tempString[7];
  dtostrf(temp, 6, 2, tempString);

  char flowString[6];
  dtostrf(flow, 5, 2, flowString);

  char cdtString[8];
  dtostrf(cdt, 7, 2, cdtString);

  char tbString[8];
  dtostrf(tb, 7, 2, tbString);

  char tdsString[8];
  dtostrf(tds, 7, 2, tdsString);

  sprintf(payloadBuffer, "{\"time\":%s,\"temp\":%s,\"flow\":%s,\"cdt\":%s,\"tb\":%s,\"tds\":%s}", timeString, tempString, flowString, cdtString, tbString, tdsString);

  return true;
}

int availableMemory() {                                                  //check memory of mcu (debugging purposes)
  int size = 16000;
  byte *buf;
  while ((buf = (byte *) malloc(--size)) == NULL);
  free(buf);
  return size;
}

byte alert(float tds, float temp, float tb, float cdt,byte battery){     //error code generator
  byte error_code = B00000;
  if(battery == false){
    error_code = error_code | B10000;
  }
  if((tb * 1.073) > 5){
      error_code = error_code | B01000;
  }
  if((temp * 1.005) > 104 | (temp * 1.005) < 32){
    error_code = error_code | B00100;
  }

  if((tds * 1.1) > 500){
    error_code = error_code | B00010;
  }

  if((cdt * 1.073) > 1000){
    error_code = error_code | B00001;
  }

  return error_code;
}

void SDCardWrite(char *payload) {
    File file = SD.open("reading.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (file) {
    file.println(payload);
    // close the file:
    file.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening reading.txt");
  }
 
}

void SDCardRead() {                                   //print data in sd card file
  File file = SD.open("reading.txt");
  if (file) {
    //Serial.println("test.txt:");

    // read from the file until there's nothing else in it:
    while (file.available()) {
      Serial.write(file.read());
    }
    // close the file:
    file.close();
    Serial.println(F("--- end of file ---"));
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}

float getTDS(float temperature){                          //get tds reading from sensor
   int sensorvalue = analogRead(A1);
   
    float volt = sensorvalue * (float)5.0 / 1024.0;
    float compensationCoefficient=1.0+0.02*(temperature-25.0);
    float compensationVolatge=volt/compensationCoefficient;  //temperature compensation
    tdsValue=(133.42*compensationVolatge*compensationVolatge*compensationVolatge - 255.86*compensationVolatge*compensationVolatge + 857.39*compensationVolatge)*0.5;
  
    return tdsValue * (2.0/3.0);
   
}

float getTurbidity(){
  int sensorValue = analogRead(A0);                         // read the input on analog pin 0:
  float volt = sensorValue * (5.0 / 1023.0);                // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  
  if(volt < 2.5){
    return 3000;
  }
  if(volt > 4.2){
    return 0;
  }
  else{
    float turbidity = -1120.4*square(volt) + 5742.3*volt - 4352.9;    //convert voltage to turbidity value
    return turbidity;
  }

}
float getTemp(){
  //returns the temperature from one DS18S20 in DEG Celsius

  byte data[12];
  byte addr[8];

  if ( !ds.search(addr)) {        //get address of ds18b20
      //no more sensors on chain, reset search
      ds.reset_search();
      return -1000;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {    //get crc
      Serial.println("CRC is not valid!");
      return -1000;
  }

  if ( addr[0] != 0x10 && addr[0] != 0x28) {
      Serial.print("Device is not recognized");
      return -1000;
  }

  ds.reset();
  ds.select(addr);                              //choose correct temperature sensor
  ds.write(0x44,1);                             // start conversion, with parasite power on at the end

  byte present = ds.reset();                    //1 = device is present
  ds.select(addr);                              //choose correct temperature sensor
  ds.write(0xBE);                               // Read Scratchpad and crc


  for (int i = 0; i < 9; i++) {                 // we need 9 bytes
    data[i] = ds.read();
  }

  ds.reset_search();                            //reset between searches

  byte MSB = data[1];                           // msb of temp measuremtn
  byte LSB = data[0];                           //lsb of temp measurement

  float tempRead = ((MSB << 8) | LSB);          //using two's compliment //co,bine 2 numbers into 1
  float TemperatureSum = tempRead / 16;         //convert temp reading

  return TemperatureSum;

}

volatile double getFlow(){
  flowCalc = flow * 4;                           //convert L/h to L/15s to ensure correct flow is sent
  return flowCalc;
}

bool getBattLevel(){                             //read input of low battery circuit
  int x = digitalRead(14);
  if (x == 0) {
    return true;
  } else {
    return false;
  }
}
