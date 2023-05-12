#include <AESLib.h>
#include <OneWire.h>

#include <SD.h>

AESLib aesLib;

unsigned long long time;
float temp;
float cdt;
float tb; 
float tds;
bool lowBatt;
char payloadBuffer[112];
char encryptedPayloadBuffer[112];
byte secret_key[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
byte aes_iv[16] = { 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34,35, 36 };
byte aes_iv_to[16] = { 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34,35, 36 };
byte aes_iv_from[16] = { 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34,35, 36 };

#define VREF 5.0      // analog reference voltage(Volt) of the ADC
int analogBufferIndex = 0,copyIndex = 0;
float averageVoltage = 0,tdsValue = 0;
//int DS18S20_Pin = 2; //DS18S20 Signal pin on digital 2
volatile double flow;
int connection = 0;

OneWire ds(2);

 
void setup() {
    attachInterrupt(1, pulse, RISING);
    reset_hashBuffer();
    Serial.begin(9600);
    time = 1668670372826;
    temp = 50.00;
    flow = 30.00;
    cdt = 20.00;
    tb = 10.00;
    tds = 10.00;
    pinMode(7, INPUT);
}

void loop() {
   
    for(int i = 0; i<100; i++){                //take lots of samples of measurements
        temp = temp + getTemp();
        tb = tb + getTurbidity();
        tds = tds + getTDS2(temp/(i));
    }

    temp = temp/100;            //get avg temp measuremnt
    tb = tb/100;                //get avg turbidiy measuremnt
    tds = tds/100;                            //get avg tds measurement
    cdt = tds*2.0;
    temp = temp*1.8 + 32;

    byte warningCode = alert(tds,temp,tb,cdt);
    getBattLevel();

    JSONdotStringify(time, temp, flow, cdt, tb, tds, payloadBuffer);
    //Serial.println(payloadBuffer);
    gen_hash();
    encrypt_payload();
    print_encryptedPayloadBuffer();
    reset_hashBuffer();
    time+= 900000;
    delay(10000);

    // print warning code and low battery
    //JSONdotStringify2(lowBatt, warningCode, payloadBuffer);
    //Serial.println(payloadBuffer);
    //print_payloadBuffer();
    //gen_hash();
    //encrypt_payload();
    //print_encryptedPayloadBuffer();
    //reset_hashBuffer();

    // for(SEND_DATA_INTERVAL){
    //   //sync time
    //   //get params

    //   delay(900000)
    // }

}



void JSONdotStringify(unsigned long long time, float temp, float flow, float cdt, float tb, float tds, char *payloadBuffer){
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
}

void JSONdotStringify2(byte lowBatt, byte warningCode, char *payloadBuffer){
   //Serial.println(warningCode);
   int charWarningCode = (int)warningCode;
   //Serial.println(charWarningCode);
   char charLowBatt[8];
   if(lowBatt !=0) {
     strcpy(charLowBatt, "true ");
   } else {
     strcpy(charLowBatt, "false");
   } 

    sprintf(payloadBuffer, "{\"warningCode\": %i, \"lowBatt\": %s}", charWarningCode, charLowBatt);
}

char *ulltoa(uint64_t value, char *buf, int radix) {
	char tmp[64 + 1];
	char *p1 = tmp, *p2;
	static const char xlat[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	if(radix < 2 || radix > 36)	return nullptr;
	do {*p1++ = xlat[value % (unsigned)radix];} while((value /= (unsigned)radix));
	for(p2 = buf; p1 != tmp; *p2++ = *--p1) ;
	*p2 = '\0';
	return buf;
}

void gen_hash(){
    for (int i = 0; i < 96; i++) {
      payloadBuffer[96+(i % 16)] = payloadBuffer[96 + (i % 16)] ^ payloadBuffer[i];
    }
}

void reset_hashBuffer() {
    for (int i = 0; i < 16; i++) {
      payloadBuffer[96+i] = secret_key[i];
    }
}

void encrypt_payload() {
  memcpy(aes_iv, aes_iv_to, sizeof(aes_iv_to));
  aesLib.encrypt((byte*) payloadBuffer, 112, (byte*)encryptedPayloadBuffer, secret_key, 128, aes_iv);
}

void print_payloadBuffer(){
    for(int i = 0; i < 112; i++){
        Serial.print(payloadBuffer[i]);
    }
  Serial.println();
}

void print_encryptedPayloadBuffer(){
    for(int i = 0; i < 112; i++){
        Serial.print(encryptedPayloadBuffer[i]);
    }
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
  if (error_code == 0){
    //Serial.println("All metrics within safe values");
  }
  return error_code;
  }

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

void getBattLevel(){
  int x = digitalRead(7);
  if (x == 0) {
    lowBatt = false;
  } else {
    lowBatt = true;
  }
}

