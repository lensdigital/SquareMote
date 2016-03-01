// Example of using 6 Buttons with SquareMote board
// www.lensdigital.com

#define BUTTON_PIN_1 14 // A0
#define BUTTON_PIN_2 A3 // A3
#define BUTTON_PIN_3 A1 // A1
#define BUTTON_PIN_4 4
#define BUTTON_PIN_5 A2 // A2
#define BUTTON_PIN_6 7

#define HOLD_TIME 2000 // 2 seconds concidered a hold

int buttonPins[6]={BUTTON_PIN_1,BUTTON_PIN_2,BUTTON_PIN_3,BUTTON_PIN_4,BUTTON_PIN_5,BUTTON_PIN_6};

boolean currentState[6] = {HIGH,HIGH,HIGH,HIGH,HIGH,HIGH};//stroage for current measured button state
boolean lastState[6] = {HIGH,HIGH,HIGH,HIGH,HIGH,HIGH};//storage for last measured button state
boolean debouncedState[6] = {HIGH,HIGH,HIGH,HIGH,HIGH,HIGH};//debounced button state
boolean butHeld = false; // Detect button hold

int debounceInterval = 20;//wait 20 ms for button pin to settle
unsigned long timeOfLastButtonEvent[6] = {0,0,0,0,0,0};//store the last time the button state changed
unsigned long heldTime[6] = {0,0,0,0,0,0};

void setup() {
  Serial.begin(115200);
  for (int i=0;i<6;i++) {
    pinMode(buttonPins[i],INPUT_PULLUP);
  }
}

void loop(){
 // unsigned long currentTime = millis();
  //for (int i=0;i<6;i++) 
  for (int i=0;i<6;i++) {
    currentState[i] = digitalRead(buttonPins[i]);
    if (currentState[i] != lastState[i]){
      timeOfLastButtonEvent[i] = millis();
    }
    
    if (millis() - timeOfLastButtonEvent[i] > debounceInterval){//if enough time has passed
       if (currentState[i] != debouncedState[i]){//if the current state is still different than our last stored debounced state
        debouncedState[i] = currentState[i];//update the debounced state
        //trigger an event
        if (debouncedState[i] == LOW){
          Serial.print (i+1); Serial.println(" button pressed");
          heldTime[i]=millis();
          butHeld=true; // Allows button hold logic to procede
        } else {
          Serial.print (i+1); Serial.println(" button released"); // Nothing happens here
        }
      }
      // Logic that detects button HOLD event
      /*
      if (debouncedState[i] == LOW) { // If we get here, button is still held
        if (butHeld) if (  millis()-heldTime > HOLD_TIME) { //If we allowed, check how long it passed since button was pressed
          Serial.println ("Button Held!"); // Bingo, it is held!
          butHeld=false;  // This makes sure that we skip button held check logic, we only need to go here once per hold
        }
      }
      */
    }
    
    lastState[i] = currentState[i];
    
  
  }
}
