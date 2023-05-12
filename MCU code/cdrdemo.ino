#include <OneWire.h>
#include <tinyECC.h>
#include <ArduinoJson.h>
#include <FastCRC.h>
#include <SPI.h>
#include <SD.h>

//#define TbSensorPin A0
//#define TdsSensorPin A1
#define VREF 5.0      // analog reference voltage(Volt) of the ADC
#define SCOUNT  30           // sum of sample point
int analogBuffer[SCOUNT];    // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0,copyIndex = 0;
float averageVoltage = 0,tdsValue = 0;
//int DS18S20_Pin = 2; //DS18S20 Signal pin on digital 2
volatile double flow;
//char messageBuffer[96];
int connection = 0;    //set to 1 if conneted to server (used to toggle SD card)

class CrcWriter {               //class given by json library to hash json
public:
  CrcWriter() {
    _hash = _hasher.crc32(NULL, 0);
  }

  size_t write(uint8_t c) {
    _hash = _hasher.crc32_upd(&c, 1);
  }

  size_t write(const uint8_t *buffer, size_t length) {
    _hash = _hasher.crc32_upd(buffer, length);
  }

  uint32_t hash() const {
    return _hash;
  }

private:
  FastCRC32 _hasher;
  uint32_t _hash;
};


//Temperature chip i/o
OneWire ds(2);  // on digital pin 2one

void setup(void) {
  Serial.begin(9600);                 //set baudrate
  pinMode(A1,INPUT);                  //tds sensor
  flow = 0;
  attachInterrupt(1, pulse, RISING);  //set interrupt pin to d3, call pulse() on interrupt, call interrupt when pin goes L->H
  pinMode(7,INPUT);
  
  SD.begin(4);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  if (!SD.begin(4)) {
    Serial.println(F("SD Card initialization failed!"));
    
  }
  Serial.println(F("SD Card initialization done."));
}

void loop(void) {
  float temp = 0;
  float tb = 0;
  float tds = 0;

  StaticJsonDocument<200> doc;
  
  for(int i = 0; i<100; i++){                //take lots of samples of measurements
    temp = temp + getTemp();
    tb = tb + getTurbidity();
    tds = tds + getTDS2(temp/(i));
  }

  //temp = getTemp();
  //tb = tb + getTurbidity();
  //tds = tds + getTDS2(temp);
  
  temp = temp/100;            //get avg temp measuremnt
  tb = tb/100;                //get avg turbidiy measuremnt
  tds = tds/100;                            //get avg tds measurement
  float cdt = tds*2;
  temp = temp*1.8 + 32;

  doc["temp"] = temp;         //add parammeters to json file
  doc["tb"] = tb;
  doc["tds"] = tds;
  doc["cdt"] = cdt;
  
  Serial.println(F("--------------------------------"));
  //String payload = "temperature: " + String(temp) + " Celsius" + " Farenheight" + "\nturbidity" + String(tb) + " NTU" + "\nTDS: " + String(tds) + " ppm" + "\nConductivity: " + String(cdt) + " us/cm" + "\nWaterflow: " + String(flow) + " L/s";
  //Serial.println(payload);

  
  byte error_code = alert(tds,temp,tb,cdt);    //get error code
  Serial.println(String(error_code,BIN));

  //doc["error code"] = String(error_code,BIN);

  //Serial.println(error_code,BIN);

  //serializeJsonPretty(doc, Serial);
  serializeJson(doc,Serial);

  /* CrcWriter writer;
  serializeJson(doc, writer);
  Serial.println(writer.hash());              //hash json file

  tinyECC e;                                  
  e.plaintext = writer.hash();
  e.encrypt();                                //encrypt hash
  Serial.println(e.ciphertext);     

  e.decrypt();
  Serial.println(e.plaintext);
 */
 
  //if (connection){
  
  //}
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    //dataFile.println("{\"temp\":77.7875,\"tb\":1962.658,\"tds\":34.00633,\"cdt\":68.01266}");
    dataFile.println("payload");
    dataFile.close();
    Serial.println("SD Write Succesful");
    // print to the serial port too:
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }

  

  delay(5000);  //pause arduino
}



float alert(float tds, float temp, float tb, float cdt){     //error code generator
  byte error_code = B0000;
  if((tds * 1.1) > 500){
    Serial.println("Warning: tds levels past maximum limit!");
    error_code = error_code | B1000;
    
  }

  if((temp * 1.005) > 40 | (temp * 1.005) < 0){
    Serial.println("Warning: temperature outside safe limit!");
    error_code = error_code | B0100;

  }

  if((tb * 1.073) > 1){
    Serial.println("Warning: turbidity past maximum limit!");
    error_code = error_code | B0010;
    
  }

  if((cdt * 1.073) > 1000){
    Serial.println("Warning: conductivity past maximum limit!");
    error_code = error_code | B0001;
    
  }
  if (error_code == 0){
    Serial.println("All metrics within safe values");
  }
  return error_code;
  }

/*
float getTDS(float temperature){
  static unsigned long analogSampleTimepoint = millis();
   if(millis()-analogSampleTimepoint > 40U)     //every 40 milliseconds,read the analog value from the ADC
   {
     analogSampleTimepoint = millis();
     analogBuffer[analogBufferIndex] = analogRead(A1);    //read the analog value and store into the buffer
     analogBufferIndex++;
     if(analogBufferIndex == SCOUNT) 
         analogBufferIndex = 0;
   }
   static unsigned long printTimepoint = millis();
   if(millis()-printTimepoint > 800U)
   {
      printTimepoint = millis();
      for(copyIndex=0;copyIndex<SCOUNT;copyIndex++)
        analogBufferTemp[copyIndex]= analogBuffer[copyIndex];
      averageVoltage = getMedianNum(analogBufferTemp,SCOUNT) * (float)VREF / 1024.0; // read the analog value more stable by the median filtering algorithm, and convert to voltage value
      float compensationCoefficient=1.0+0.02*(temperature-25.0);    //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
      float compensationVolatge=averageVoltage/compensationCoefficient;  //temperature compensation
      tdsValue=(133.42*compensationVolatge*compensationVolatge*compensationVolatge - 255.86*compensationVolatge*compensationVolatge + 857.39*compensationVolatge)*0.5; //convert voltage value to tds value
   }
   return tdsValue;   
  
  
}

*/
float getTurbidity(){
  int sensorValue = analogRead(A0);// read the input on analog pin 0:
  float volt = sensorValue * (5.0 / 1024.0); // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
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

int getMedianNum(int bArray[], int iFilterLen) 
{
      int bTab[iFilterLen];
      for (byte i = 0; i<iFilterLen; i++)
      bTab[i] = bArray[i];
      int i, j, bTemp;
      for (j = 0; j < iFilterLen - 1; j++) 
      {
      for (i = 0; i < iFilterLen - j - 1; i++) 
          {
        if (bTab[i] > bTab[i + 1]) 
            {
        bTemp = bTab[i];
            bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
         }
      }
      }
      if ((iFilterLen & 1) > 0)
    bTemp = bTab[(iFilterLen - 1) / 2];
      else
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
      return bTemp;
}

void pulse()   //measure the quantity of square wave
{
  flow += 1.0 / 150.0; // 150 pulses=1L (refer to product specification）
}




void SDCardWrite(String payload) {
    File file = SD.open("temp.txt", FILE_WRITE);

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

  File file = SD.open("temp.txt");
  if (file) {
    Serial.println("test.txt:");

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



float getTDS2(float temperature){
    int sensorvalue = analogRead(A1);
   
    float volt = sensorvalue * (float)VREF / 1024.0;
    //Serial.println("volt1: "+String(volt));

    float compensationCoefficient=1.0+0.02*(temperature-25.0);
    float compensationVolatge=volt/compensationCoefficient;  //temperature compensation
    tdsValue=(133.42*compensationVolatge*compensationVolatge*compensationVolatge - 255.86*compensationVolatge*compensationVolatge + 857.39*compensationVolatge)*0.5;
    //Serial.print("TDS Value test 2:");
    //  Serial.print(tdsValue,0);
    //  Serial.println("ppm");

    return tdsValue;
}
