#include <Wire.h>
#include <DHT.h>
#include <TimerOne.h>
#include <Encoder.h>
#include <Arduino.h>
#include <SI114X.h>
#include <Si115X.h>
#include "Si115X.h"
Si115X si1151;
#include <U8g2lib.h>
#include <SPI.h>
U8G2_SSD1306_128X64_ALT0_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); // SSD1306 and SSD1308Z are compatible

#define DHTTYPE DHT20
#define DHTPIN          A0
#define MoisturePin     A1
#define ButtonPin       2
#define EncoderPin1     3
#define EncoderPin2     4
#define WaterflowPin    5
#define RelayPin        6
#define OneSecond       1000
#define DataUpdateInterval 20000
#define RelayOn         HIGH
#define RelayOff        LOW
#define NoWaterTimeOut  3

unsigned int uiWaterVolume = 0;
//unsigned char WaterflowFlag = 0;
unsigned int WaterflowRate = 0;
unsigned char EncoderFlag = 0;
unsigned long StartTime = 0;
unsigned char ButtonFlag = 0;
unsigned long counter = 0;

SI114X SI1145 = SI114X();
DHT dht(DHTPIN, DHTTYPE);
float SensorHumidity    = 0;
float SensorTemperature = 0;
float SensorMoisture   = 0;
float UVIndex        = 0;
char buffer[30];

void setup(void) {
  Serial.begin(9600);
  Wire.begin();
  u8g2.begin(); // initialize u8g2 oled display
  u8g2.clearDisplay(); // clear the oled display
  u8g2.refreshDisplay();  // set standart mode oled display
  //u8g2.setFlipMode(); / set addressing mode to Page Mode

  //encoder test
  Serial.println("DHTxx test!");
  dht.begin();

  // initialize the button
  pinMode(ButtonPin, INPUT);
  attachInterrupt(0, ButtonClick, FALLING);

  // initialize the encoder
  pinMode(EncoderPin1, INPUT);
  pinMode(EncoderPin2, INPUT);
  attachInterrupt(1, EncoderRotate, RISING);

}

void loop() {
  //Display funсtion
  void OledDisplay() {
    u8g2.clearBuffer();                   // clear the internal memory
    u8g2.setFont(u8g2_font_ncenB08_tr);   // choose a suitable font
    u8g2.drawStr(0, 10, "Hello World!");  // write something to the internal memory
    u8g2.sendBuffer();                    // transfer internal memory to the display
    delay(1000);
  }

  //Temp & Humidity Sensor function
  void TempHum() {
    float temp_hum_val[2] = {0};
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)

    if (!dht.readTempAndHumidity(temp_hum_val)) {
      debug.print("Humidity: ");
      debug.print(temp_hum_val[0]);
      debug.print(" %\t");
      debug.print("Temperature: ");
      debug.print(temp_hum_val[1]);
      debug.println(" *C");
    } else {
      debug.println("Failed to get temprature and humidity value.");
    }

    delay(1500);
  }

  //Button funсtion
  void ButtonClick()
  {
    if (digitalRead(ButtonPin) == 0) {
      delay(10);
      if (digitalRead(ButtonPin) == 0) {
        ButtonFlag = 1;
      }
    }
  }

  void WaterPumpOn()
  {
    digitalWrite(RelayPin, RelayOn);
  }

  void WaterPumpOff()
  {
    digitalWrite(RelayPin, RelayOff);
  }
}
