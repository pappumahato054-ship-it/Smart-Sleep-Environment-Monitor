#include <WiFiS3.h>
#include <DHT.h>
#include <ThingSpeak.h>

char ssid[] = "realme C11 2021";
char pass[] = "dubai123";
  
unsigned long myChannelNumber = 3228203;   
const char* myWriteAPIKey = "2J98DX1WQW8K5NOD";

WiFiClient client;

#define DHTPIN 2
#define DHTTYPE DHT11
#define RED_PIN 9
#define GREEN_PIN 10
#define BLUE_PIN 11

DHT dht(DHTPIN, DHTTYPE);

bool commonAnode = true;


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

// TINYML DECISION TREE LOGIC
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

void connectWiFi() {
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, pass);
    delay(2000);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(9600);
  dht.begin();

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  Serial.println("Comfort Monitor (TinyML + Common-Anode RGB) Started...");


  setRGB(255, 0, 0); delay(300);    
  setRGB(0, 255, 0); delay(300);    
  setRGB(255, 255, 0); delay(300);   
  setRGB(0, 0, 0);                   

  connectWiFi();
  ThingSpeak.begin(client);
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Sensor error!");
    delay(2000);
    return;
  }

  int comfortLevel = classifyComfort(t, h);

  Serial.print("Temp: "); Serial.print(t);
  Serial.print(" °C | Humidity: "); Serial.print(h);
  Serial.print(" % | Comfort Level: "); Serial.println(comfortLevel);

  if (comfortLevel == 2) {
    setRGB(0, 255, 0);      
  } 
  else if (comfortLevel == 1) {
    setRGB(255, 255, 0);     
  } 
  else {
    setRGB(255, 0, 0);       
  }


  ThingSpeak.setField(1, t);
  ThingSpeak.setField(2, h);
  ThingSpeak.setField(3, comfortLevel);

  int response = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  if (response == 200) {
    Serial.println("ThingSpeak update Successful.");
  } else {
    Serial.print("ThingSpeak error: ");
    Serial.println(response);
  }

  delay(15000);
}
