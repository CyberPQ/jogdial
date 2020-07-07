
// [ Instruction ]
//Youtube https://youtu.be/FACm5D3bskQ
//Manual https://sites.google.com/view/100happythings/15-premiere-pro-dial-diy
//Source Code https://github.com/happythingsmaker/PremiereProDialDIY
//3D Printing File https://www.thingiverse.com/thing:2810760

//[ Parts and Tools ]
//Rotational encoder with switch http://ebay.to/2t6I0f4  ($1)
//Arduino pro micro http://ebay.to/2oxvIrw ($4.9)
//Non Slip Rubber http://ebay.to/2oyFGc1 ($0.5)


// Nom des touches clavier :
//     https://www.arduino.cc/en/Reference/KeyboardModifiers

// Resolution : some encoder is too sensitive. this reduces sensitivity.
#define RESOLUTION 2

// include a Keyboard library
#include "Keyboard.h"

// include a EEPROM library for memorizing last function
#include <EEPROM.h>

// "OUTPUT_B" pin.
#define  OUTPUT_B 9

// "OUTPUT_A" pin.
#define  OUTPUT_A 10

// "BUTTON" pin
#define  BUTTON 11

// "PIN_5V" is going to be replaced with "A2".
//#define PIN_5V  A2

// "PIN_GND" is going to be replaced with "A3".
//#define PIN_GND  A3

// Declare variables aState, aLastState for checking the state of OUTPUT_A of the encoder
bool aState;

// We need to save the previous state of OUTPUT_A
bool aLastState;

// this variable for check the state of button.
// in order to prevent the button from chattering, we need to check the first moment the button is pressed.
// Becasue when a button pressed, the button usually sends a lot of signals.
// So, we need to ignore following noise signals for a while.
bool lastButtonState = 0;

// mode selection
#define PREMIERE_MODE 0
#define LIGHTROOM_MODE 1
int mode = LIGHTROOM_MODE;
const int numMode = 2;

// void setup(){} function is for one time setting
void setup() {

  //led
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  //read the last mode
  mode = EEPROM.read(0);

  // in order to use the Keyboard library, begin() is necessary
  Keyboard.begin();

  // OUTPUT_A (A0) is for INPUT
  pinMode(OUTPUT_A, INPUT);

  // OUTPUT_B (15) is for INPUT
  pinMode(OUTPUT_B, INPUT);

  // BUTTON (A1) is for INPUT
  // Most pin has thier own pull-up resistor.
  // INPUT_PULLUP makes the pin high.
  // a leg of button is connected with this pin and GND
  // when the button is not pressed, the pin reads HIGH signal because of this PULL-UP
  // when the button is pressed, pin is going to be LOW which means "pressed"
  pinMode(BUTTON, INPUT_PULLUP);

  // PIN_5V (A2) is for OUTPUT
  // This pin is used for giving 5V to the encoder.
  // normally, 5v is coming from VCC, but, I didn't want to use any wires
  // So, this is a kind of trick, but it works well.
  //pinMode(PIN_5V, OUTPUT);
  //digitalWrite(PIN_5V, HIGH);

  // PIN_GND (A3) is for OUTPUT
  // This pin is used for giving GND to the encoder.
  // normally, GND is coming from GND, but, I didn't want to use any wires
  // So, this is a kind of trick, but it works well.
  // pinMode(PIN_GND, OUTPUT);
  //digitalWrite(PIN_GND, LOW);

  // read a signal from OUTPUT_A
  // this is for initialization
  aLastState = digitalRead(OUTPUT_A);

  //Initialize serial and wait for port to open:
  /*Serial.begin(115200);
    while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
    }
  */
  delay(300);

  // prints title with ending line break
  Serial.println("JogDial by CyberPQ");
  digitalWrite(LED_BUILTIN, LOW);
}

// in order to prevent chattering, we need to check the moment when was the last click moment
// for 1000ms, we will ignore all signals
long lastClickTime = 0;
long tempCount = 0;

// this loop() function repeats its code eternally
void loop() {

  //read signal from OUTPUT_A and save its state to aState
  aState = digitalRead(OUTPUT_A);

  // if aLastState is not currentState, it meant there's something changed.
  if (aState != aLastState) {

    // read another pin's state.
    // if you want to know about the theory, watch this video
    // https://www.youtube.com/watch?v=v4BbSzJ-hz4
    if (digitalRead(OUTPUT_B) != aState) {
      rotateLeft();
      delay(8);
    } else {
      rotateRight();
      delay(8);
    }
    // save current State as last State
    aLastState = aState;
  }

  // read button (short or long)
  if (digitalRead(BUTTON) == LOW) {
    if (lastButtonState == LOW) {
      // LOW -> LOW : nothing to do

    } else {
      // HIGH-> LOW
      lastClickTime = millis();
      delay(300); // ignoring chattering

    }
    lastButtonState = LOW;
  } else {

    if (lastButtonState == LOW) {   // LOW -> HIGH : check whether long press or not
      Serial.println(millis() - lastClickTime);
      if (millis() - lastClickTime >= 1000) {
        // long press : mode change
        changeMode();
      } else {
        // short press :
        pressButton();
      }
    }
    else {                          // HIGH -> HIGH : noting to do
    }
    lastButtonState = HIGH;
  }
}

void changeMode() {
  digitalWrite(LED_BUILTIN, HIGH);
  mode = ++mode % numMode;
  EEPROM.write(0, mode);
  Serial.print("changement de mode : ");
  Serial.println(mode);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
}

void rotateLeft() {
  if (tempCount++ % RESOLUTION == 0) {
    if (mode == PREMIERE_MODE) {
      Keyboard.press(KEY_LEFT_ALT);
      Keyboard.press(KEY_UP_ARROW);
    } else if (mode == LIGHTROOM_MODE) {
      Keyboard.press('m');  // , in azerty
    }
    Keyboard.releaseAll();
    Serial.println("left");
  }
}

void rotateRight() {
  if (tempCount++ % RESOLUTION == 0) {
    if (mode == PREMIERE_MODE) {
      Keyboard.press(KEY_LEFT_ALT);
      Keyboard.press(KEY_DOWN_ARROW);
    } else if (mode == LIGHTROOM_MODE) {
      Keyboard.press(',');  // ; in azerty
    }
    Keyboard.releaseAll();
    Serial.println("right");
  }
}

void pressButton() {
  if (mode == PREMIERE_MODE) {
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press('t');
  } else if (mode == LIGHTROOM_MODE) {
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press('t');
  }
  Keyboard.releaseAll();
  Serial.println("  press");
}
