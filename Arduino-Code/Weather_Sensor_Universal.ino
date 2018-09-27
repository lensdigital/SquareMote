// by LensDigital
// To use RFM69 uncomment #define RFM69_CHIP, for RFM12B comment it out
// V09: 04/23/2015  Removed Get Up time due to buggy code
// V10: 08/17/2016  Increased buffer size for 100% humidity
// V11: 11/03/2016  Option for Si7021 temp/hum sensor
// V12: 4/23/2017 Fix Node and Frequency Mixed up!

//#define RFM69_CHIP // Uncomment for RFM69, comment out for RFM12b
//#define TH02_IC // Uncomment for TH02 sensor

#define ver 12

#include <EEPROM.h> 
#include "LowPower.h"
#if defined (RFM69_CHIP) 
   #include <RFM69.h> 
#else
  #include <RFM12B.h> 
#endif
#include "Wire.h" 
#ifdef TH02_IC
  #include <TH02_dev.h>
#else 
  #include <SI7021.h>
  SI7021 sensor;
#endif

// ============ Frequent USER DEFINITIONSS ========================================================
// ------------------------------------------------
#define SLEEP_MULTIPLIER 38 // Multiplies by 8 seconds to get total sleep time (38 is 5 min)
#define defaultRefVolt 1078 // Change it to reflect actual int voltage reading on AREF pin, and do setEEPROM(true) first time; 
// ------------------------------------------------------------------
// --------Radio Definitions
// ------------------------------------------------------------------
#define NODEID        5    //unique for each node on same network
#define NETWORKID     1  //the same on all nodes that talk to each other
#define GATEWAYID     1
#if defined (RFM69_CHIP) 
  #define FREQUENCY     RF69_915MHZ 
#else 
  #define FREQUENCY     RF12_915MHZ
#endif
#define ENCRYPTKEY    "thisIsEncryptKey" //exactly the same 16 characters/bytes on all nodes!
#define ACK_TIME      30 // max # of ms to wait for an ack
#define SERIAL_BAUD 115200
// ===================================================================================================

// ============= Other Defines ======================================
// ----- PINS -------------------
// ------------------------------------------------------------------
#ifdef TH02_IC
  #define TH02_PWR_PIN 8     // Pin that powers TH02 sensor
#endif
#define LED 9 // Pin for LED indicator
// ------------------------------------------------------------------
// ----- THO2 Linearization Coefficients
// ------------------------------------------------------------------
#ifdef TH02_IC
  float TH02_A0 = -4.7844;
  float TH02_A1 =   0.4008;
  float TH02_A2 =  -0.00393;
  // TH02 Temperature compensation Linearization Coefficients
  float TH02_Q0 = 0.1973;
  float TH02_Q1 =  0.00237;
#endif
// ------------------------------------------------------------------
// ----  EEPROM Locations 
// ------------------------------------------------------------------
#define intRefVoltLoc 3 // EEPROM location for storing internal voltage reference (takes 2 address, becuase it's Word)
// ===================================================================

int InternalReferenceVoltage;  // Stores specific internal BG voltage 
unsigned long lastUpdate; //keep track of last time 
unsigned long lastMillis;

int interPacketDelay = 100; //wait this many ms between sending packets
int days;
int hours;
unsigned long minutes;
unsigned long seconds;

char buffer[15];

// =====================================================================
// Need an instance of the Radio Module
#if defined (RFM69_CHIP) 
  RFM69 radio;
#else 
  RFM12B radio;
#endif
// =====================================================================

void setup()
{
  //ADCSRA &= ~(1 << ADEN); // Turn off ADC to save power
  Serial.begin(SERIAL_BAUD);
  Serial.print (F("Firmware version: ")); Serial.println (ver);
  initRadio();
  setEEPROM(false); // Read EEPROM settings and upated in needed
  #ifdef TH02_IC
    pinMode(TH02_PWR_PIN,OUTPUT); // Enable TH02 power pin
    digitalWrite(TH02_PWR_PIN, HIGH); // Turn on TH02
    delay (50);
    TH02.begin();
 #endif
  Serial.println("Ready");
}

void loop()
{
 getSensorData();
 sendData();
 goSleep();
 lastMillis=(unsigned long)millis()-lastMillis;
 lastUpdate=lastUpdate+8000UL*SLEEP_MULTIPLIER+lastMillis;
  
}

// ===================================================================
// --- Get Humidity and Temperature from sensor and format it
// ===================================================================
void getSensorData() {
  int tempCint;
  int humInt;
  float rhlinear, rhvalue,temperature;
  Serial.print(F("Requesting temperature/humidity..."));
  #ifdef TH02_IC // Get data from TH02 sensor
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
  #else // Get data from SI7021 sensor
    si7021_env data = sensor.getHumidityAndTemperature();
    delay (5);
    temperature = data.celsiusHundredths/100;
    rhvalue = data.humidityBasisPoints/100;
  #endif
  getBatVoltage(); // To stabilize voltage reading
 // Serial.println("DONE");
  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(temperature) || isnan(rhvalue))  Serial.println("Failed to read from TH02");
  Serial.println (F("Getting battery voltage..."));
  Serial.print(F("Temperature is: "));
  Serial.print (temperature); 
  Serial.println("C"); 
  Serial.print(F("Humidity is: "));
  Serial.print (rhvalue); 
  Serial.println(" %\t"); 
  
  Serial.print(F("Battery is: "));
  Serial.print (getBatVoltage()); 
  Serial.println("V"); 
  
  tempCint=int(temperature +0.5);
  humInt=int(rhvalue +0.5);
  // Create pocket in format Bxxx,Txxx,Hxxx where B is battery voltage, T is temp, and H is humidity
  snprintf(buffer,sizeof(buffer), "B%d,T%d,H%d",getBatVoltage(),tempCint,humInt);
  //snprintf(buffer,sizeof(buffer), "B%d,T%d,H5",getBatVoltage(),tempCint); // Send fake data for debugging
  //Serial.print ("Buffer: "); Serial.println (buffer);
  //delay (10);
 
}

// ===================================================================
// --- Transmit Data
// ===================================================================
void sendData () {
  Serial.println(F("Transmitting..."));
  //requestACK = !(sendSize % 3); //request ACK every 3rd xmission
  #if defined (RFM69_CHIP)
    radio.sendWithRetry(GATEWAYID, buffer, sizeof(buffer));
  #else
    radio.Wakeup();
    radio.Send(GATEWAYID, buffer, sizeof(buffer),false);
  #endif 
  Blink(LED,50);
  #if defined (RFM69_CHIP)
     radio.sleep();
   #else
     radio.Sleep();
   #endif
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
   int results = (((InternalReferenceVoltage * 1024UL) / ADC) + 5) / 10; 
   return results;
  } 

// ===================================================================
// --- Calculates how long sensor has been up (dd:hh:mm:ss)
// ===================================================================
void getUptime() {
  
  seconds = lastUpdate/1000;
  minutes=seconds/60;
  hours=minutes/60;
  days=hours/24;
  seconds=seconds-(minutes*60);
  minutes=minutes-(hours*60);
  hours=hours-(days*24);
  snprintf (buffer,sizeof(buffer),"U%02d:%02d:%02ld:%02ld",days,hours,minutes,seconds);
  Serial.print (F("Uptime:")); Serial.println (buffer);
  delay (10);
}

void Blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}

void initRadio() {
  char myFrequency[12];
  #if defined (RFM69_CHIP) 
   radio.initialize(FREQUENCY,NODEID, NETWORKID);  
  //radio.encrypt(ENCRYPTKEY);
  radio.sleep();
  #else 
    radio.Initialize(FREQUENCY,NODEID, NETWORKID);
  // radio.Encrypt(KEY);
   radio.Sleep();
  #endif
  char buff[31];
  #if defined (RFM69_CHIP) 
    snprintf(buff, sizeof(buff), "\nRFM69 Working at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
  #else 
    snprintf(buff, sizeof(buff), "\nRFM12B Working at %d Mhz...", FREQUENCY==RF12_433MHZ ? 433 : FREQUENCY==RF12_868MHZ ? 868 : 915);
  #endif
  Serial.println(buff); 
  Serial.print("Net:");Serial.print (NETWORKID);
  Serial.print(" / Node:");Serial.println (NODEID);
}

// ====================================================================================
// --- Reads EEPROM settings (such as internal ref voltage), and if needed updates them
// --- Set forceUpdate to true if you want to update setting
// --- but if it detects that value was never set correctly it will write default value anyway
// ====================================================================================
void setEEPROM(boolean forceUpdate) {
  InternalReferenceVoltage=EEPROMReadInt(intRefVoltLoc); // Read eeprom settings
  if (forceUpdate) { // Forcing update
    if (InternalReferenceVoltage !=defaultRefVolt) { // Value is different to write and set variable
    Serial.println (F("Writing data to EEPROM"));
      EEPROMWriteInt (intRefVoltLoc,defaultRefVolt); // Write/update data in EEPROM
      InternalReferenceVoltage=defaultRefVolt; // Set variable to default,  faster then reReading EEPROM again
    }
  }
  else { // Not forced, but might still update
    if ( (InternalReferenceVoltage < 1000) ||  (InternalReferenceVoltage > 1100)) { // Validating if setting was set correctly
      Serial.println (F("Setting not found in EEPROM. Updating to default"));
      EEPROMWriteInt (intRefVoltLoc,defaultRefVolt); // Write/update data in EEPROM
      InternalReferenceVoltage=defaultRefVolt;
    }
  }
}

// ===============================================================================================
// --- Write a 2 byte integer to the eeprom at the specified address and address + 1
// ===============================================================================================
void EEPROMWriteInt(int p_address, int p_value)
     {
     byte lowByte = ((p_value >> 0) & 0xFF);
     byte highByte = ((p_value >> 8) & 0xFF);

     EEPROM.write(p_address, lowByte);
     EEPROM.write(p_address + 1, highByte);
     }

// ==================================================================================================
// --- Read a 2 byte integer from the eeprom at the specified address and address + 1
// ==================================================================================================
unsigned int EEPROMReadInt(int p_address)
     {
     byte lowByte = EEPROM.read(p_address);
     byte highByte = EEPROM.read(p_address + 1);

     return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
     }
