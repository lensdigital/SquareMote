// Simple transmission test. Sends some text at specified intervals.
//V4: Now send pocket # for range test

#include <RFM69.h>
#include <SPI.h>

#define NODEID      99
#define NETWORKID   100
#define GATEWAYID   1
#define FREQUENCY   RF69_433MHZ //Match this with the version of your Moteino! (others: RF69_433MHZ, RF69_868MHZ)
#define KEY         "thisIsEncryptKey" //has to be same 16 characters/bytes on all nodes, not more not less!
#define LED         9
#define SERIAL_BAUD 115200
#define ACK_TIME    30  // # of ms to wait for an ack
#define SEND_DELAY 1000 // Delay between transmissions

RFM69 radio;

int pocketNum; // Track pocket numbers

void setup() {
  Serial.begin(SERIAL_BAUD);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  //radio.setHighPower(); //uncomment only for RFM69HW!
  radio.encrypt(KEY);
  char buff[50];
  sprintf(buff, "\nTransmitting at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
  Serial.println(buff);
  
  //sendData();
}

long lastPeriod = -1;
void loop() {
  sendData();
  delay (5000);
}

void sendData() {
  Serial.println("Sending Data");
   if (radio.sendWithRetry(GATEWAYID, "hello", 5))
      Serial.print(" ok!");
    else Serial.print(" no ACK");
   
    Serial.println();
    Blink(LED,50);
    delay (SEND_DELAY); 
}

void Blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}
