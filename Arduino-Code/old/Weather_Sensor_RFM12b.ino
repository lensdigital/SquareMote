// by LensDigital
// Based on Felix's Example felix@lowpowerlab.com
// V05: 12/18/2014. Update 02/08/2015: This version is for RFM12b Squaremotes

#include "LowPower.h"
#include <RFM69.h> 
#include <SPI.h>
#include <TH02_dev.h>
#include "Wire.h" 

#define TH02_PWR_PIN 8     // Pin that powers TH02 sensor
// THO2 Linearization Coefficients
#define TH02_A0   -4.7844
#define TH02_A1    0.4008
#define TH02_A2   -0.00393
// TH02 Temperature compensation Linearization Coefficients
#define TH02_Q0   0.1973
#define TH02_Q1   0.00237

// You will need to initialize the radio by telling it what ID it has and what network it's on
// The NodeID takes values from 1-127, 0 is reserved for sending broadcast messages (send to all nodes)
// The Network ID takes values from 0-255
// By default the SPI-SS line used is D10 on Atmega328. You can change it by calling .SetCS(pin) where pin can be {8,9,10}
//#define RFM69_CHIP 1 // Set to 1 for RFM69, set to 0 for RFM12b
#define NODEID        3    //unique for each node on same network
#define NETWORKID     77  //the same on all nodes that talk to each other
#define GATEWAYID     33
#define FREQUENCY     RF69_915MHZ
#define ENCRYPTKEY    "ThisIsTheKey" //exactly the same 16 characters/bytes on all nodes!
#define ACK_TIME      30 // max # of ms to wait for an ack
#define SERIAL_BAUD 9600

#define LED 9 // Pin for LED indicator
#define SLEEP_MULTIPLIER 38 // Multiplies by 8 seconds to get total sleep time (38 is 5 min)

const long InternalReferenceVoltage = 1075;  // Adjust this value to your board's specific internal BG voltage

int interPacketDelay = 100; //wait this many ms between sending packets
char input = 0;

// Need an instance of the Radio Module
RFM69 radio;
//byte sendSize=0;
//char payload[]= "Current Temperature is: ";
char buffer[13];
//int tempCint;
//bool requestACK=false;

void setup()
{
  //ADCSRA &= ~(1 << ADEN); // Turn off ADC to save power
  Serial.begin(SERIAL_BAUD);
  radio.initialize(NODEID, FREQUENCY, NETWORKID);
  //radio.encrypt(ENCRYPTKEY);
  radio.sleep(); //sleep right away to save power
  pinMode(TH02_PWR_PIN,OUTPUT); // Enable TH02 power pin
  digitalWrite(TH02_PWR_PIN, HIGH); // Turn on TH02
  delay (50);
  TH02.begin();
 // delay(100);
  //Serial.println("Ready");
}

void loop()
{
  getSensorData();
  //delay (interPacketDelay);
  sendTemp();
  goSleep();
}


// ===================================================================
// --- Get Humidity and Temperature from sensor and format it
// ===================================================================
void getSensorData() {
  int tempCint;
  int humInt;
  float rhlinear, rhvalue,temperature;
  Serial.print("Requesting temperature/humidity...");
  digitalWrite(TH02_PWR_PIN, HIGH); // Turn on TH02
  delay (50);
  temperature=TH02.ReadTemperature();
  rhvalue=TH02.ReadHumidity();
  delay (50);
  digitalWrite(TH02_PWR_PIN, LOW); // Turn off TH02
  // Calculate linear compensation
  rhlinear = rhvalue - ((rhvalue*rhvalue) * TH02_A2 + rhvalue * TH02_A1 + TH02_A0);
  rhvalue = rhlinear;
  // Calculate humidty temperature compesation
  rhvalue += (temperature - 30.0) * (rhlinear * TH02_Q1 + TH02_Q0);
  getBatVoltage(); // To stabilize voltage reading
 // Serial.println("DONE");
  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(temperature) || isnan(rhvalue))  Serial.println("Failed to read from TH02");
 /* Serial.println ("Getting battery voltage...");
  Serial.print("Temperature is: ");
  Serial.print (temperature); 
  Serial.println("C"); 
  Serial.print("Humidity is: ");
  Serial.print (rhvalue); 
  Serial.println(" %\t"); 
  
  Serial.print("Battery is: ");
  Serial.print (getBatVoltage()); 
  Serial.println("V"); 
  */
  tempCint=int(temperature +0.5);
  humInt=int(rhvalue +0.5);
  // Create pocket in format Bxxx,Txx,Hxx where B is battery voltage, T is temp, and H is humidity
  snprintf(buffer,sizeof(buffer), "B%d,T%d,H%d",getBatVoltage(),tempCint,humInt);
  //Serial.print ("Buffer: "); Serial.println (buffer);
  //delay (10);
 
}

// ===================================================================
// --- Transmit temperature
// ===================================================================
void sendTemp () {
  //Serial.println("Transmitting...");
  //requestACK = !(sendSize % 3); //request ACK every 3rd xmission
  radio.sendWithRetry(GATEWAYID, buffer, sizeof(buffer));
  Blink(LED,50);
  radio.sleep();
  delay(interPacketDelay);
}

// ===================================================================
// --- Enter power saving sleep mode
// ===================================================================
void goSleep()
{
 //Serial.println ("Going to sleep");
// delay (10);
  for (int i=0;i<SLEEP_MULTIPLIER;i++) {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    //LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);
  }
//  Serial.println ("Wakup up");
}

// ===================================================================
// --- Get battery voltage (via internal ref voltage)
// ===================================================================
int getBatVoltage () 
  {
  // REFS0 : Selects AVcc external reference
  // MUX3 MUX2 MUX1 : Selects 1.1V (VBG)  
   ADMUX = bit (REFS0) | bit (MUX3) | bit (MUX2) | bit (MUX1);
   ADCSRA |= bit( ADSC );  // start conversion
   while (ADCSRA & bit (ADSC))
     { }  // wait for conversion to complete
   int results = (((InternalReferenceVoltage * 1024) / ADC) + 5) / 10; 
   return results;
  } 

void Blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}
