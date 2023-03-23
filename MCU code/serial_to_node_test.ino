#include <AESLib.h>

AESLib aesLib;

unsigned long long time;
float temp;
float ph;
float flow;
float cdt;
float tb; 
float tds;
char messageBuffer[96];
char hashBuffer[16];
char payloadBuffer[112];
char encryptedPayloadBuffer[112];
char decryptedPayloadBuffer[112];
byte secret_key[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
byte bad_seret_key[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15} ;
byte aes_iv[16] = { 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34,35, 36 };
byte aes_iv_to[16] = { 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34,35, 36 };
byte aes_iv_from[16] = { 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34,35, 36 };

 
void setup() {
    reset_hashBuffer();
    Serial.begin(9600);
    time = 1667770372826;
    temp = 50.00;
    ph = 70.00;
    flow = 30.00;
    cdt = 20.00;
    tb = 10.00;
    tds = 10.00;
}

void loop() {
    JSONdotStringify(time, temp, ph, flow, cdt, tb, tds, messageBuffer);
    gen_hash();
    write_payload_buffer();
    encrypt_payload();
    Serial.println(String(messageBuffer));
    //Serial.println(String(hashBuffer));
    //print_hashBuffer();
    print_encryptedPayloadBuffer();
    //decrypt_payload();
    //delay(2000);
    //print_decryptedPayloadBuffer();
    //Serial.println(String(encryptedPayloadBuffer));
    //Serial.println("-------------------------");
    reset_hashBuffer();
    delay(5000);
}

void JSONdotStringify(unsigned long long time, float temp, float ph, float flow, float cdt, float tb, float tds, char *messageBuffer){
    char timeString[16];
    ulltoa(time, timeString, 10);

    char tempString[6];
    dtostrf(temp, 5, 2, tempString);

    char phString[6];
    dtostrf(ph, 5, 2, phString);

    char flowString[6];
    dtostrf(flow, 5, 2, flowString);

    char cdtString[6];
    dtostrf(cdt, 5, 2, cdtString);

    char tbString[6];
    dtostrf(tb, 5, 2, tbString);

    char tdsString[6];
    dtostrf(tds, 5, 2, tdsString);

    sprintf(messageBuffer, "{\"time\":%s,\"temp\":%s,\"ph\":%s,\"flow\":%s,\"cdt\":%s,\"tb\":%s,\"tds\":%s}", timeString, tempString, phString, flowString, cdtString, tbString, tdsString);
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
      hashBuffer[i % 16] = hashBuffer[i % 16] ^ messageBuffer[i];
    }
}

void reset_hashBuffer() {
    for (int i = 0; i < 16; i++) {
      hashBuffer[i] = secret_key[i];
    }
}

void write_payload_buffer() {
    for (int i = 0; i < 96; i++) {
      payloadBuffer[i] = messageBuffer[i];
    }
    for (int i = 0; i < 16; i++) {
      payloadBuffer[i+96] = hashBuffer[i];
    }
}

void encrypt_payload() {
  memcpy(aes_iv, aes_iv_to, sizeof(aes_iv_to));
  aesLib.encrypt((byte*) payloadBuffer, 112, (byte*)encryptedPayloadBuffer, secret_key, 128, aes_iv);
}

void decrypt_payload() {
  memcpy(aes_iv, aes_iv_from, sizeof(aes_iv_from));
  aesLib.decrypt((byte*) encryptedPayloadBuffer, 112, (byte*)decryptedPayloadBuffer, secret_key, 128, aes_iv);
}

void print_hashBuffer(){
    for(int i = 0; i < 16; i++){
        Serial.print((int)hashBuffer[i]);
        Serial.print(", ");
    }
  Serial.println();
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

void print_decryptedPayloadBuffer(){
    for(int i = 0; i < 112; i++){
        Serial.print(decryptedPayloadBuffer[i]);
    }
}