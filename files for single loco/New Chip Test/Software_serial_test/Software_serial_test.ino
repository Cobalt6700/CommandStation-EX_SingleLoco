#include <Arduino.h>
#include <UART.h>
#include <SoftwareSerial.h>

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
    SoftwareSerial SW_ESP32_Serial(PIN_PA5, PIN_PA4); // RX, TX
#elif defined ( NANO_EVERY_PINOUT )
    HardwareSerial & DCC_EX_Serial = Serial3;
    HardwareSerial & iBUS_Serial = Serial1;
    HardwareSerial & Terminal = Serial;
    HardwareSerial & ESP32_Serial = Serial2;
#endif

const byte numChars = 32;

char DCC_EX_Data[numChars];   // an array to store the received data
boolean newDCC_EX_Data = false;

char ESP32_Data[numChars];   // an array to store the received data
boolean newESP32_Data = false;

void HWrecvWithEndMarker(HardwareSerial &serialPort, char* buffer, boolean &dataFlag) {
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

// void HWsendNewDataEnd(HardwareSerial &outputSerial, char* buffer, boolean &dataFlag) {
//     if (dataFlag == true) {
//         outputSerial.println(buffer);
//         dataFlag = false;
//     }
// }

void HWsendNewData(HardwareSerial &outputSerial, char* buffer, boolean &dataFlag) {
    if (dataFlag == true) {    
        outputSerial.println(buffer);
    }
  }

void sendComplete(boolean &dataFlag){
    dataFlag = false;
}

void SWrecvWithEndMarker(SoftwareSerial &serialPort, char* buffer, boolean &dataFlag) {
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
  
//   void SWsendNewDataEnd(SoftwareSerial &outputSerial, char* buffer, boolean &dataFlag) {
//     if (dataFlag == true) {
//         outputSerial.println(buffer);
//         dataFlag = false;
//     }
//   }
  void SWsendNewData(SoftwareSerial &outputSerial, char* buffer, boolean &dataFlag) {
    if (dataFlag == true) {
        outputSerial.println(buffer);
    }
  }

void setup() { 
    // set the digital pin as output:
    pinMode(ledPin, OUTPUT);
    #if defined(CHIP_1)
        SW_ESP32_Serial.begin(9600);
    #elif defined(CHIP_2)
      
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



    #if defined(CHIP_1)  
        SWrecvWithEndMarker(SW_ESP32_Serial, ESP32_Data, newESP32_Data);

        HWsendNewData(DCC_EX_Serial, ESP32_Data, newESP32_Data);
        HWsendNewData(Terminal, ESP32_Data, newESP32_Data);
        sendComplete(newESP32_Data);
    #elif defined(CHIP_2)        
        HWrecvWithEndMarker(DCC_EX_Serial, DCC_EX_Data, newDCC_EX_Data);

        HWsendNewData(Terminal, DCC_EX_Data, newDCC_EX_Data);
        sendComplete(newDCC_EX_Data);
    #endif
  

}
