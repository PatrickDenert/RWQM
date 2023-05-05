#include <DFRobot_SIM7000.h>
#include <Base64.h>
#include <OneWire.h>
#include "OneWire.h"
//#include "DallasTemperature.h"
//#include <SD.h>
#include <SPI.h>
#include <SD.h>
OneWire ds(2);
//DallasTemperature tempSensor(&ds);

#define PIN_TX     11
#define PIN_RX     10
SoftwareSerial     mySerial(PIN_RX,PIN_TX);
DFRobot_SIM7000    sim7000(&mySerial);
unsigned long long time = 1668670372826;

//unsigned char jsonString[] = "{\"time\":\"1668670372826\",\"temp\":77.75,\"tb\":1962.658,\"tds\":34.03,\"cdt\":68.06}";
//unsigned char base64String[1];
volatile double flow;

//unsigned long long time = 1668670372826;  
//float temp = 20;
//float tb = 30;
#define TdsSensorPin A1
#define VREF 5.0      // analog reference voltage(Volt) of the ADC

float averageVoltage = 0,tdsValue = 0;

char payloadBuffer[300];


bool connection;                //determine if mcu cellular is working (ie toggle sd module)

float temp;
float tds;
float tb;

float cdt;

int inputStringLength;
int encodedLen;
int count = 0;



void setup() {
  //pinMode(1,INPUT);
  pinMode(18,OUTPUT);
  attachInterrupt(0, pulse, RISING);  //DIGITAL Pin 2: Interrupt 0

  pinMode(TdsSensorPin,INPUT);
  
  Serial.begin(9600);
  mySerial.begin(19200);
  Serial.println(F("Turn ON SIM7000......"));
  if(sim7000.turnON()){                             //Turn ON SIM7000
    //Serial.println(F("Successfully turned on"));
    connection = true;
  } else {
    //Serial.println(F("Failed to turn on!"));
    connection = false;
  }
  delay(1000);
  
  Serial.println(F("Set baud rate......"));

  
  if(connection == true){
    while(1){
      if(sim7000.setBaudRate(19200)){               //Set SIM7000 baud rate from 115200 to 19200 reduce the baud rate to avoid distortion
        Serial.println(F("Set baud rate:19200"));
        connection = true;
          break;
        }else{
          Serial.println(F("Faile to set baud rate"));
          connection = false;
          delay(1000);
        }
     
    }

    delay(1000);
  
    mySerial.begin(19200);

  
    if(sim7000.attacthService()){
      Serial.println(F("Attatched to Spervice"));
      connection = true;
    } else {
      Serial.println(F("Fail to Attatch to service"));
      connection = false;
    }
    delay(1000);
  }
  else{
    SD.begin(0);  
  }
}

void loop() {
  temp = 0.0;
  tds = 0.0;
  tb = 0.0;
  cdt = 0.0;
  flow = 0.0;
  
  time = time + 900000;
    
  
  //float tds = 0;
  //float tb = 0;
  
 
  
  //sensorCollect();
 
  
  temp = getTemp();

  
  tds = getTDS(temp);
  tb = getTurbidity();   
  cdt = tds*2.0;
  temp = temp*1.8 + 32;

 

  JSONdotStringify(time,temp,flow,cdt,tb,tds,payloadBuffer);  


  if(connection == false){
    //SDCardWrite(payloadBuffer);
    Serial.println("no connection");
    Serial.println(payloadBuffer);
    //SDCardRead();

    delay(10000);  
  }

  else{
  inputStringLength = strlen(payloadBuffer);
  encodedLen = Base64.encodedLength(inputStringLength);

  char base64String[encodedLen+1];
  //char * base64String;
  Serial.println("len: ");
  Serial.println(encodedLen);
  //base64String = (char*) malloc(encodedLen+1);
  
  char message[encodedLen+35];
  //char * message;
  //message = (char*) malloc(encodedLength+35);
  
  Base64.encode(base64String, payloadBuffer, inputStringLength);
  Serial.println(payloadBuffer);

  //free(payloadBuffer);

  sprintf(message, "{\"k\":\"5#p7ILQi\",\"d\":\"%s\",\"t\":\"test\"}", base64String);
  delay(1000);
  
  //free(base64String);
  
  delay(10000);
  sendmsg(message);
  delay(1000);
  Serial.flush();

  
  
  

  
  delay(30000);
  //free(message);

  byte error_code = alert(tds,temp,tb,cdt);
  Serial.println(error_code);
  //error_code = 1;
  if(error_code !=0 ){
    char * alertmessage;
    alertmessage = (char*) malloc(100);
    sprintf(alertmessage, "{\"k\":\"5#p7ILQi\",\"d\":\"%i\",\"t\":\"alert\"}", error_code);
    //Serial.println(alertmessage);
    delay(1000);
    sendmsg(alertmessage);
    delay(1000);
    
    free(alertmessage);
  }
  Serial.println("memory left: ");
  Serial.println(availableMemory());

  delay(10000);
  }
  count = count + 1;
  Serial.println("count: ");
  Serial.println(count);
} 

bool sendmsg(char *message){
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

char *ulltoa(uint64_t value, char *buf, int radix) {
    char tmp[64 + 1];
    char *p1 = tmp, *p2;
    static const char xlat[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if(radix < 2 || radix > 36)    return nullptr;
    do {*p1++ = xlat[value % (unsigned)radix];} while((value /= (unsigned)radix));
    for(p2 = buf; p1 != tmp; *p2++ = *--p1) ;
    *p2 = '\0';
    return buf;
}
bool JSONdotStringify(unsigned long long time, float temp, float flow, float cdt, float tb, float tds, char *payloadBuffer){
    char timeString[16];
    ulltoa(time, timeString, 10);

    char tempString[6];
    dtostrf(temp, 5, 2, tempString);

    char flowString[6];
    dtostrf(flow, 5, 2, flowString);

    char cdtString[6];
    dtostrf(cdt, 5, 2, cdtString);

    char tbString[8];
    dtostrf(tb, 7, 2, tbString);

    char tdsString[6];
    dtostrf(tds, 5, 2, tdsString);

    sprintf(payloadBuffer, "{\"time\":%s,\"temp\":%s,\"flow\":%s,\"cdt\":%s,\"tb\":%s,\"tds\":%s}", timeString, tempString, flowString, cdtString, tbString, tdsString);

    return true;    
}

bool sensorCollect(){
  flow = 0;  
  float temp = temp + random(10);
  float tb = tb + random(20);
  float tds = tds + random(30);
  time = time + 900000;
  float cdt = tds*2.0;
  temp = temp*1.8 + 32;

  char payloadBuffer[150];

  JSONdotStringify(time,temp,flow,cdt,tb,tds,payloadBuffer);  

  //Serial.println(payloadBuffer);

  int inputStringLength = strlen(payloadBuffer);
  int encodedLength = Base64.encodedLength(inputStringLength);

  char base64String[encodedLength+1];

  char message[encodedLength+35];

  Base64.encode(base64String, payloadBuffer, inputStringLength);

  sprintf(message, "{\"k\":\"5#p7ILQi\",\"d\":\"%s\",\"t\":\"test\"}", base64String);
  delay(1000);
  
  sendmsg(message);

  delay(1000);
  

}
int availableMemory()
{
int size = 16000;
byte *buf;
while ((buf = (byte *) malloc(--size)) == NULL);
free(buf);
return size;
}
byte alert(float tds, float temp, float tb, float cdt){     //error code generator
  byte error_code = B0000;
  if((tds * 1.1) > 500){
    //Serial.println("Warning: tds levels past maximum limit!");
    error_code = error_code | B1000;
    
  }

  if((temp * 1.005) > 104 | (temp * 1.005) < 32){
    //Serial.println("Warning: temperature outside safe limit!");
    error_code = error_code | B0100;

  }

  if((tb * 1.073) > 1){
    //Serial.println("Warning: turbidity past maximum limit!");
    error_code = error_code | B0010;
    
  }

  if((cdt * 1.073) > 1000){
    //Serial.println("Warning: conductivity past maximum limit!");
    error_code = error_code | B0001;
    
  }
  
  return error_code;


  }

  void SDCardWrite(char *payload) {
    //SD.begin(0);
    File file = SD.open("temp100.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (file) {
    Serial.print("Writing to temp.txt...");
    file.println(payload);
    // close the file:
    file.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening data.txt");
  }
 
}

void SDCardRead() {
 //SD.begin(0);
  File file = SD.open("temp100.txt");
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
    //Serial.println("error opening test.txt");
  }
}

float getTDS(float temperature){
   int sensorvalue = analogRead(A1);
   
    float volt = sensorvalue * (float)VREF / 1024.0;
    //Serial.println("volt1: "+String(volt));

    float compensationCoefficient=1.0+0.02*(temperature-25.0);
    float compensationVolatge=volt/compensationCoefficient;  //temperature compensation
    tdsValue=(133.42*compensationVolatge*compensationVolatge*compensationVolatge - 255.86*compensationVolatge*compensationVolatge + 857.39*compensationVolatge)*0.5;
    //Serial.print("TDS Value test 2:");
    //Serial.print(tdsValue,0);
    //Serial.println("ppm");

    return tdsValue;
   
}

float getTurbidity(){
  int sensorValue = analogRead(A0);// read the input on analog pin 0:
  float volt = sensorValue * (5.0 / 1023.0); // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  //Serial.println(volt);
  volt = volt - 0.17;
  
  //Serial.print("voltage: ");
  //Serial.println(volt);
  if(volt < 2.5){
    return 3000;
  }
  if(volt > 4.2){
    return 0;
  }
  else{
    float turbidity = -1120.4*square(volt) + 5742.3*volt - 4352.9;
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
  ds.select(addr);   //choose correct temperature sensor
  ds.write(0x44,1); // start conversion, with parasite power on at the end

  byte present = ds.reset();      //1 = device is present
  ds.select(addr);                //choose correct temperature sensor
  ds.write(0xBE); // Read Scratchpad and crc


  for (int i = 0; i < 9; i++) { // we need 9 bytes 
    data[i] = ds.read();
  }

  ds.reset_search();            //reset between searches

  byte MSB = data[1];           // msb of temp measuremtn
  byte LSB = data[0];           //lsb of temp measurement

  float tempRead = ((MSB << 8) | LSB); //using two's compliment //co,bine 2 numbers into 1
  float TemperatureSum = tempRead / 16;     //convert temp reading

  return TemperatureSum;

}


void pulse()   //measure the quantity of square wave
{
  flow += 1.0 / 450.0;
}
