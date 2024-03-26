//#include <TimerOne.h>
#include <Arduino.h>
#include <DHT.h>
#include <Encoder.h>
#include <SI114X.h>
#include <SPI.h>
#include <Si115X.h>
#include <U8g2lib.h>
#include <UnoWiFiDevEd.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiNINA.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>
#include <Wire.h>

#include "Si115X.h"
Si115X si1151;

U8G2_SSD1306_128X64_ALT0_F_HW_I2C u8g2(
    U8G2_R0, /* reset=*/U8X8_PIN_NONE);  // SSD1306 and SSD1308Z are compatible
// Lichtsensor
SI114X SI1145 = SI114X();

char ssid[] = "Janniks Hotspot";  //  your network SSID (name)
char pass[] = "12345678";         // your network password
int keyIndex = 0;  // your network key Index number (needed only for WEP)
int status = WL_IDLE_STATUS;  // Statuscode für das WiFi Modul
WiFiServer server(80);        // server socket
WiFiClient client =
    server.available();  // HTTP Client um auf Requests zu antworten

String curr_line = "";  // Dynamischer Buffer für die HTTP Requests

#define DHTTYPE DHT11
//#define DHTTYPE DHT20
#define DHTPIN A0
#define MoisturePin A1
#define MoistureValue 0
#define ButtonPin 2
#define ButtonClick
#define EncoderPin1 3
#define EncoderPin2 4
#define WaterflowPin 5
#define RelayPin 6
#define OneSecond 1000
#define DataUpdateInterval 20000
#define RelayOn HIGH
#define RelayOff LOW
#define NoWaterTimeOut 3

unsigned int uiWaterVolume = 0;
// unsigned char WaterflowFlag = 0;
unsigned int WaterflowRate = 0;
unsigned char EncoderFlag = 0;
unsigned long StartTime = 0;
unsigned char ButtonFlag = 0;
unsigned long counter = 0;
volatile int flow_frequency;  // Measures flow sensor pulsesunsigned

int l_hour;                    // Calculated litres/hour
unsigned char flowsensor = 2;  // Sensor Input
unsigned long currentTime;
unsigned long cloopTime;

void flow()  // Interrupt function

{
  flow_frequency++;
}

DHT dht(DHTPIN, DHTTYPE);
float SensorHumidity = 0;
float SensorTemperature = 0;
float SensorMoisture = 0;
float UVIndex = 0;
char buffer[30];

void setup(void) {
  // Initialisiere den Serial Monitor
  Serial.begin(9600);
  Wire.begin();
  u8g2.begin();           // initialize u8g2 oled display
  u8g2.clearDisplay();    // clear the oled display
  u8g2.refreshDisplay();  // set standart mode oled display
  // u8g2.setFlipMode(); / set addressing mode to Page Mode

  enable_WiFi();
  connect_WiFi();

  server.begin();
  printWifiStatus();

  // Water Flow Sensor
  pinMode(flowsensor, INPUT);
  digitalWrite(flowsensor, HIGH);  // Optional Internal Pull-Up
  Serial.begin(9600);
  attachInterrupt(0, flow, RISING);  // Setup Interrupt
  sei();                             // Enable interrupts
  currentTime = millis();
  cloopTime = currentTime;

  // encoder test
  // Serial.println("DHTxx test!");
  // dht.begin();

  // initialize the button
  pinMode(ButtonPin, INPUT);
  // attachInterrupt(0, ButtonClick, FALLING);

  // initialize the encoder
  pinMode(EncoderPin1, INPUT);
  pinMode(EncoderPin2, INPUT);
  //  attachInterrupt(1, EncoderRotate, RISING);
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, OUTPUT);
  // Setze den Pin für das Relay der Pumpe als Ausgang
  pinMode(4, OUTPUT);

  // Setze den Pin für das Relay des Lüfters als Ausgang
  pinMode(5, OUTPUT);

  pinMode(8, INPUT);
  digitalWrite(8, HIGH);

  // Reserviere 128 bytes an Speicher für HTTP-Requests
  curr_line.reserve(128);
}

void loop() {
  // Display funсtion
  /* int OledDisplay {
    // u8g2.clearBuffer();                  // clear the internal memory
    u8g2.setFont(u8g2_font_ncenB08_tr);  // choose a suitable font
    u8g2.drawStr(0, 10,
                 "Hello World!");  // write something to the internal memory
    u8g2.sendBuffer();             // transfer internal memory to the display
    delay(1000);
    }
  */

  // Temp & Humidity Sensor function
  /*  int TempHum {
      float temp_hum_val[2] = {0};
      // Reading temperature or humidity takes about 250 milliseconds!
      // Sensor readings may also be up to 2 seconds 'old' (its a very slow
      // sensor)

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
  */
  /*// Water Flow Sensor
    currentTime = millis();  // Every second, calculate and print litres/hour
    if (currentTime >= (cloopTime + 1000)) {
    cloopTime = currentTime;  // Updates cloopTime
    // Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min.
    l_hour = (flow_frequency * 60 /
              7.5);  // (Pulse frequency x 60 min) / 7.5Q = flowrate in L/hour
    flow_frequency = 0;         // Reset Counter
    Serial.print(l_hour, DEC);  // Print litres/hour
    Serial.println(" L/hour");
    }*/

  //  Button funсtion
  /*int ButtonClick {
    if (digitalRead(ButtonPin) == 0) {
      delay(10);
      if (digitalRead(ButtonPin) == 0) {
        ButtonFlag = 1;
      }
    }
    }*/

  /* // Wasserpumpe
    void WaterPumpOn { digitalWrite(RelayPin, RelayOn); }

    void WaterPumpOff() { digitalWrite(RelayPin, RelayOff); }
  */
  client = server.available();

  if (client) {
    printWEB();
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");

  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

void enable_WiFi() {
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }
}

void connect_WiFi() {
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP
    // network:
    status = WiFi.begin(ssid, pass);

    delay(1500);
  }
}

void printWEB() {
  if (client) {                    // if you get a client,
    Serial.println("new client");  // print a message out the serial port
    String currentLine =
        "";  // make a String to hold incoming data from the client
    while (client.connected()) {  // loop while the client's connected
      if (client.available()) {   // if there's bytes to read from the client,
        char c = client.read();   // read a byte, then
        Serial.write(c);          // print it out the serial monitor
        if (c == '\n') {          // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a
          // row. that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            char msg[1500];

            snprintf(msg, 1500,
                     "<html>\
  <head>\
    <meta http-equiv='refresh' content='4'/>\
    <meta name='viewport' content='width=device-width, initial-scale=1'>\
    <link rel='stylesheet' href='https://use.fontawesome.com/releases/v5.7.2/css/all.css' integrity='sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr' crossorigin='anonymous'>\
    <title>ESP32 DHT Server</title>\
    <style>\
    html { font-family: Arial; display: inline-block; margin: 0px auto; text-align: center;}\
    h2 { font-size: 3.0rem; }\
    p { font-size: 3.0rem; }\
    .units { font-size: 1.2rem; }\
    .dht-labels{ font-size: 1.5rem; vertical-align:middle; padding-bottom: 15px;}\
    </style>\
  </head>\
  <body>\
      <h2>ESP32 DHT Server!</h2>\
      <p>\
        <i class='fas fa-thermometer-half' style='color:#ca3517;'></i>\
        <span class='dht-labels'>Temperature</span>\
        <span>%.2f</span>\
        <sup class='units'>&deg;C</sup>\
      </p>\
      <p>\
        <i class='fas fa-tint' style='color:#00add6;'></i>\
        <span class='dht-labels'>Humidity</span>\
        <span>%.2f</span>\
        <sup class='units'>&percnt;</sup>\
      </p>\
  </body>\
</html>",
//                     readDHTTemperature(), readDHTHumidity());
            server.on(200, "text/html", msg);
          }
        } else {  // if you got a newline, then clear currentLine:
          currentLine = "";
        }
      } else if (c != '\r') {  // if you got anything else but a carriage return
        // character,
        currentLine += c;  // add it to the end of the currentLine
      }
    }
  }
}
}

float readDHTTemperature() {
  // Sensor readings may also be up to 2 seconds
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  if (isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return -1;
  } else {
    Serial.println(t);
    return t;
  }
}

float readDHTHumidity() {
  // Sensor readings may also be up to 2 seconds
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return -1;
  } else {
    Serial.println(h);
    return h;
  }
}
