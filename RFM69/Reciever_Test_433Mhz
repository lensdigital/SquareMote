// Example sketch for receiving transmission from another node
#include <RFM69.h>
#include <SPI.h>

#define NODEID      1
#define NETWORKID   100
#define GATEWAYID   99
#define FREQUENCY   RF69_433MHZ //Match this with the version of your Moteino! (others: RF69_433MHZ, RF69_868MHZ)
#define KEY         "thisIsEncryptKey" //has to be same 16 characters/bytes on all nodes, not more not less!
#define LED         9
#define ACK_TIME    30  // # of ms to wait for an ack
RFM69 radio;

void setup() {
  Serial.begin(115200);
  //radio.setCS(18);
  //Serial.println ("test");
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  radio.encrypt(KEY);
  char buff[50];
  sprintf(buff, "\nReceiving at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
  Serial.println(buff);
  Serial.print("Net:");Serial.println (NETWORKID);
  Serial.print("Node:");Serial.println (NODEID);
}

void loop() {
  recieveData();
  
}

// Recieves transmission and parse it
void recieveData() {
  if (radio.receiveDone())
  {
   
    Serial.println("Data recieved");
     Blink(LED,50);  
     Serial.print ("Sender ID:"); Serial.println (radio.SENDERID);
      for (byte i = 0; i < radio.DATALEN; i++) 
        Serial.print((char)radio.DATA[i]); // Print out characters received
      if (radio.ACKRequested())
      {
        radio.sendACK();
        Serial.println(" - ACK sent");
      }
   
  }
 // radio.sleep();
}

void Blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}
