#include <Arduino.h>
#include <UART.h>

// #define DEBUG_TERMINAL
#define SAME_SERIAL
// #define CHIP_1
#define CHIP_2

// constants won't change. Used here to set a pin number:
const int ledPin =  LED_BUILTIN;// the number of the LED pin

// Variables will change:
bool ledState = LOW;             // ledState used to set the LED

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store last time LED was updated

// constants won't change:
const long interval = 1000;           // interval at which to blink (milliseconds)

#if defined( MEGACOREX_DEFAULT_48PIN_PINOUT )
    HardwareSerial & DCC_EX_Serial = Serial;
    HardwareSerial & iBUS_Serial = Serial1;
    HardwareSerial & Terminal = Serial2;
    HardwareSerial & ESP32_Serial = Serial3;
#elif defined ( NANO_EVERY_PINOUT )
    HardwareSerial & DCC_EX_Serial = Serial3;
    HardwareSerial & iBUS_Serial = Serial1;
    HardwareSerial & Terminal = Serial;
    HardwareSerial & ESP32_Serial = Serial2;
#endif

const byte numChars = 32;

char DCC_EX_Data[numChars];   // an array to store the received data
boolean newDCC_EX_Data = false;

void recvWithEndMarker(HardwareSerial &serialPort, char* buffer, boolean &dataFlag) {
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;
  
  while (serialPort.available() > 0 && dataFlag == false) {
      rc = serialPort.read();

      if (rc != endMarker) {
          buffer[ndx] = rc;
          ndx++;
          if (ndx >= numChars) {
              ndx = numChars - 1;
          }
      }
      else {
          buffer[ndx] = '\0';
          ndx = 0;
          dataFlag = true;
      }
  }
}

void sendNewData(HardwareSerial &outputSerial, char* buffer, boolean &dataFlag) {
  if (dataFlag == true) {
      outputSerial.println(buffer);
      dataFlag = false;
  }
}

void setup() { 
    // set the digital pin as output:
    pinMode(ledPin, OUTPUT);
    #if defined(CHIP_1)
      DCC_EX_Serial.pins(0,5);
    #elif defined(CHIP_2)
      DCC_EX_Serial.pins(4,1);
    #endif
    DCC_EX_Serial.begin(115200);
    Terminal.begin(115200);
}

void loop() {
  // here is where you'd put code that needs to be running all the time.

  // check to see if it's time to blink the LED; that is, if the difference
  // between the current time and last time you blinked the LED is bigger than
  // the interval at which you want to blink the LED.
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(ledPin, ledState);
    
    #if defined(CHIP_1)
      Terminal.print("T1_LED STATE: "); Terminal.println(ledState);
      DCC_EX_Serial.print("D_LED STATE: "); DCC_EX_Serial.println(ledState);
    #elif defined(CHIP_2)
      Terminal.print("T2_LED STATE: "); Terminal.println(ledState);
    #endif
  }


  #if defined(CHIP_2)
    recvWithEndMarker(DCC_EX_Serial, DCC_EX_Data, newDCC_EX_Data);
    sendNewData(Terminal, DCC_EX_Data, newDCC_EX_Data);
    // if (DCC_EX_Serial.available()) {      // If anything comes in Serial 
    //         Terminal.write(DCC_EX_Serial.read());   // read it and send it out Serial
    //         Terminal.println("");
    //     } 
  #endif
  
    // char rc;
    // static byte ndx = 0;
    // boolean newData = false;
    // while (DCC_EX_Serial.available() > 0 ) {
    //   rc = Serial.read();    
    //   serialmsg[ndx] = rc;
    //   ndx++;
    //   newData = true;
    // }

    // if (newData == true) {      // If anything comes in Serial 
    //   Terminal.write(serialmsg);   // read it and send it out Serials
    //   Terminal.print("\n");
    //   #if defined(CHIP_1)
    //     DCC_EX_Serial.write(serialmsg);   // read it and send it out Serial
    //     DCC_EX_Serial.print("\n");
    //   #endif
    // }   

}

// void recvWithEndMarker() {
//     static byte ndx = 0;
//     char endMarker = '\n';
//     char rc;
    
//     while (DCC_EX_Serial.available() > 0 && newData == false) {
//         rc = DCC_EX_Serial.read();

//         if (rc != endMarker) {
//             receivedChars[ndx] = rc;
//             ndx++;
//             if (ndx >= numChars) {
//                 ndx = numChars - 1;
//             }
//         }
//         else {
//             receivedChars[ndx] = '\0'; // terminate the string
//             ndx = 0;
//             newData = true;
//         }
//     }
// }

// void showNewData() {
//     if (newData == true) {
//         // Serial.print("This just in ... ");
//         Terminal.println(receivedChars);
//         newData = false;
//     }
// }
