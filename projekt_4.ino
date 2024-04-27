//#include <TimerOne.h>
#include <Arduino.h>
#include <DHT.h>
#include <Encoder.h>
#include <SI114X.h>
#include <SPI.h>
#include <Si115X.h>
#include <U8g2lib.h>
#include <UnoWiFiDevEd.h>
#include <WiFiNINA.h>
#include <Wire.h>

Si115X si1151;

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

char ssid[] = "iPhone (Artem)";  // your network SSID (name) // Janniks Hotspot
char pass[] =
    "12345678";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;  // your network key index number (needed only for WEP)

int status = WL_IDLE_STATUS;
WiFiServer server(80);

#define DHTTYPE DHT20  // DHT 22  (AM2302)
#define DHTPIN A1
#define MoisturePin A1
#define MoistureValue 0
#define EncoderPin1 3
#define EncoderPin2 4
#define OneSecond 1000
#define DataUpdateInterval 20000
#define NoWaterTimeOut 3
#define DHTPIN 2
#define PUMPPIN 4
DHT dht(DHTPIN, DHTTYPE);

unsigned int uiWaterVolume = 0;
// unsigned char WaterflowFlag = 0;
unsigned int WaterflowRate = 0;
unsigned char EncoderFlag = 0;
unsigned long StartTime = 0;
const int BUTTON_PIN = 5;  // Arduino pin connected to button's pin
const int RELAY_PIN = 6;   // Arduino pin connected to relay's pin
unsigned long counter = 0;

// for water flow sensor
volatile int flow_frequency = 0;  // Measures flow sensor pulsesunsigned
int l_hour;                       // Calculated litres/hour
unsigned char flowsensor = 2;     // Sensor Input
unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;
unsigned long cloopTime;

void flow()  // Interrupt function

{
  flow_frequency++;
}

float SensorHumidity = 0;
float SensorTemperature = 0;
float SensorMoisture = 0;
float UVIndex = 0;
char buffer[30];

String output1 = "off";
String output2 = "off";
String header;

// für Temperature und Humidity
#if defined(ARDUINO_ARCH_AVR)
#define debug Serial

#elif defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_SAM)
#define debug SerialUSB
#else
#define debug Serial
#endif

Encoder myEnc(3, 4);

void setup() {
  // Initialisiere den Serial Monitor
  Serial.begin(9600);
  Wire.begin();
  u8g2.begin();  // initialize u8g2 oled display
  dht.begin();

  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Verbindungsversuch: ");
    Serial.println(ssid);  // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP
    // network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(3000);
  }
  server.begin();  // start the web server on port 80
  printWifiStatus();

  // encoder test

  // initialize the button
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // set arduino pin to input pull-up mode
  pinMode(RELAY_PIN, OUTPUT);         // set arduino pin to output mode
  // attachInterrupt(0, ButtonClick, FALLING);

  // initialize the encoder
  pinMode(EncoderPin1, INPUT);
  pinMode(EncoderPin2, INPUT);
  //  attachInterrupt(1, EncoderRotate, RISING);
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, OUTPUT);
  // Setze den Pin für das Relay der Pumpe als Ausgang
  pinMode(PUMPPIN, OUTPUT);

  // Setze den Pin für das Relay des Lüfters als Ausgang
  // pinMode(5, OUTPUT);

  pinMode(8, INPUT);
  digitalWrite(8, HIGH);

  attachInterrupt(
      8, []() { flow_frequency++; }, RISING);

  // Water Flow Sensor
  pinMode(flowsensor, INPUT);
  digitalWrite(flowsensor, HIGH);  // Optional Internal Pull-Up
  Serial.begin(9600);
  attachInterrupt(0, flow, RISING);  // Setup Interrupt
  sei();                             // Enable interrupts
  currentTime = millis();
  cloopTime = currentTime;

  // für SunlightSensor
  if (!si1151.Begin()) {
    Serial.println("Si1151 is not ready!");
    while (1) {
      delay(1000);
      Serial.print(".");
    };
  } else {
    Serial.println("SinlightSensor gestartet!");
  }
}

long oldPosition = -999;

void loop() {
  webServer();

  float temperature = dht.readTemperature();  // Читаем температуру
  float humidity = dht.readHumidity();        // Читаем влажность
  int moisture = analogRead(MoisturePin);  // Читаем влажность почвы
  int sunlight = si1151.ReadVisible();  // Читаем сигнал от солнечного датчика

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_t0_11_mr);
  u8g2.setCursor(0, 10);
  u8g2.print("Temperature: ");
  u8g2.print(temperature);
  u8g2.print(" C");
  u8g2.setCursor(0, 20);
  u8g2.print("Humidity: ");
  u8g2.print(humidity);
  u8g2.print(" %");
  u8g2.setCursor(0, 30);
  u8g2.print("Moisture: ");
  u8g2.print(moisture);
  u8g2.setCursor(0, 40);
  u8g2.print("Sunlight: ");
  u8g2.print(sunlight);
  u8g2.setCursor(0, 50);
  u8g2.print(l_hour);
  u8g2.print(" L/hour: ");

  u8g2.sendBuffer();

  delay(1000);  // Задержка в миллисекундах между обновлениями значений

  // Temp & Humidity Sensor function
  float temp_hum_val[2] = {0};

  // Water Flow Sensor
  currentTime = millis();  // Every second, calculate and print litres/hour
  if (currentTime >= (cloopTime >= 1000)) {
    cloopTime = currentTime;  // Updates cloopTime
    // Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min.
    l_hour = (flow_frequency /
              7.5);  // (Pulse frequency x 60 min) / 7.5Q = flowrate in L/hour
    flow_frequency = 0;  // Reset Counter

    if (l_hour == 0) {
      digitalWrite(PUMPPIN, LOW);
    }
  }

  //  Button funсtion with Relay
  int buttonState = digitalRead(BUTTON_PIN);  // read new state

  if (buttonState == LOW) {
    digitalWrite(RELAY_PIN, LOW);  // turn on
  } else if (buttonState == HIGH) {
    digitalWrite(RELAY_PIN, HIGH);  // turn off
  }

  // Encoder
  long newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
    Serial.println(newPosition);
  }
}

void webServer() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New Client.");
    String currentLine = "";
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        header += c;
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            if (header.indexOf("GET /ledon1") >= 0) {
              output1 = "on";
              digitalWrite(RELAY_PIN, HIGH);
            } else if (header.indexOf("GET /ledoff1") >= 0) {
              output1 = "off";
              digitalWrite(RELAY_PIN, LOW);
            }

            client.println("<!DOCTYPE html><html>");
            client.println(
                "<head><meta name=\"viewport\" content=\"width=device-width, "
                "initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println(
                "<style> html {font-family: Arial; display: inline-block; "
                "text-align: center;}");
            client.println(
                ".on { background-color: #FF0000; border: 5px; color: white; "
                "padding: 16px 40px; border-radius: 20px;");
            client.println(
                "text-decoration: none; font-size: 30px; margin: 2px; cursor: "
                "pointer;}");
            client.println(
                ".off {background-color: #000000;border: 5px; color: white; "
                "padding: 16px 40px; border-radius: 20px;");
            client.println(
                "text-decoration: none; font-size: 30px; margin: 2px; cursor: "
                "pointer;}</style></head>");

            client.println("<body><h1>arduino project</h1>");

            client.println("<p>Relay " + output1 + "</p>");
            if (output1 == "off") {
              client.println(
                  "<p><a href=\"/ledon1\"><button class=\"off\">Turn "
                  "ON</button></a></p>");
            } else {
              client.println(
                  "<p><a href=\"/ledoff1\"><button class=\"on\">Turn "
                  "OFF</button></a></p>");
            }

            client.println("<p>Moisture: ");
            // client.println(sensorValue);
            client.println("</p>");
            /*
            client.println("<p>Infrarotlicht: " + si1151.ReadIR());
            client.println("</p>");
            client.println("<p>Sichtbares Licht: " + si1151.ReadVisible());
            client.println("</p>");
            */
            client.println("</body></html>");

            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
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
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}
