#include <DHT.h>

// Pin setup
#define DHTPIN 2
#define DHTTYPE DHT11
#define RED_PIN 9
#define GREEN_PIN 10
#define BLUE_PIN 11

DHT dht(DHTPIN, DHTTYPE);

// LED is COMMON-ANODE
bool commonAnode = true;

// RGB LED helper (handles common-anode inversion)
void setRGB(int r, int g, int b) {
  if (commonAnode) {
    r = 255 - r;
    g = 255 - g;
    b = 255 - b;
  }
  analogWrite(RED_PIN, r);
  analogWrite(GREEN_PIN, g);
  analogWrite(BLUE_PIN, b);
}

// TINYML DECISION TREE LOGIC (from model I have trained)
int classifyComfort(float temp, float hum) {
  if (hum <= 60.05) {
    if (temp <= 18.30) {
      return 1;
    } else {
      if (temp <= 24.55) {
        return 2;
      } else {
        return 1;
      }
    }
  } else {
    if (hum <= 71.10) {
      return 1;
    } else {
      if (temp <= 18.30) {
        return 1;
      } else {
        return 0;
      }
    }
  }
}

void setup() {
  Serial.begin(9600);
  dht.begin();

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  Serial.println("Comfort Monitor (TinyML + Common-Anode RGB) Started...");

  // LED test
  setRGB(255, 0, 0); delay(300);     // Red
  setRGB(0, 255, 0); delay(300);     // Green
  setRGB(255, 255, 0); delay(300);   // Yellow
  setRGB(0, 0, 0);                   // Off
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Sensor error!");
    return;
  }

  int comfortClass = classifyComfort(t, h);

  Serial.print("Temp: "); Serial.print(t);
  Serial.print(" °C | Humidity: "); Serial.print(h);
  Serial.print(" % | Comfort Class: "); Serial.println(comfortClass);

  // COLOR MAPPING
  // 0 → RED
  // 1 → YELLOW
  // 2 → GREEN
  if (comfortClass == 2) {
    setRGB(0, 255, 0);       // GREEN
  } 
  else if (comfortClass == 1) {
    setRGB(255, 255, 0);     // YELLOW
  } 
  else {
    setRGB(255, 0, 0);       // RED
  }

  delay(1500);
}






