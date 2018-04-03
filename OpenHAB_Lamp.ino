#include <EEPROM.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <Adafruit_NeoPixel.h>

#define CLIENT_NAME "clientKatyLED"

#define PIN_ONOFF D0
#define PIN D2

#define RGB_NUMPIXELS   8

const char *ssid =  "Ntk-39";  // Имя вайфай точки доступа
const char *password =  "506938506938"; // Пароль от точки доступа

const char *mqtt_server = "192.168.1.106"; // Имя сервера MQTT
const int mqtt_port = 1883; // Порт для подключения к серверу MQTT
//const char *mqtt_user = ""; // Логи от сервер
//const char *mqtt_pass = ""; // Пароль от сервера

WiFiClient espClient;
PubSubClient client(espClient);

Adafruit_NeoPixel strip = Adafruit_NeoPixel(RGB_NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int val = 0;

int result = 10;

int select_type = 0;

int brightness = 10;
int red = 127;
int green = 127;
int blue = 127;

int repeat = 0;

int matrixPos;

#define ADDRESS_SELECTED_TYPE         0
#define ADDRESS_SELECTED_BRIGHTNESS   1
#define ADDRESS_SELECTED_RED          2
#define ADDRESS_SELECTED_GREEN        3
#define ADDRESS_SELECTED_BLUE         4

void setup() {
  EEPROM.begin(512);
  
  strip.setBrightness(brightness);
  strip.begin();
  strip.show();

  Serial.begin(115200);

  pinMode(PIN_ONOFF, INPUT);

  select_type = EEPROM.read(ADDRESS_SELECTED_TYPE);
  brightness = EEPROM.read(ADDRESS_SELECTED_BRIGHTNESS);
  red = EEPROM.read(ADDRESS_SELECTED_RED);
  green = EEPROM.read(ADDRESS_SELECTED_GREEN);
  blue = EEPROM.read(ADDRESS_SELECTED_BLUE);
  
  Serial.print("select_type - ");  
  Serial.print(select_type, DEC);
  Serial.println();

  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  static int pos = 0;

  val = digitalRead(PIN_ONOFF);
  if (val == 0) {
    brightness = EEPROM.read(ADDRESS_SELECTED_BRIGHTNESS);
    
    strip.setBrightness(brightness * 2.5);
    strip.begin();
    strip.show();

    select_type = EEPROM.read(ADDRESS_SELECTED_TYPE);

    switch (select_type) {
      case 0:
        white();
        break;
      case 1:
        fire();
        break;
      case 2:
        rainbowMatrix();        
        break;
      case 3:
        rainbowCycle(20);
        break;
      case 4:
        theaterChaseRainbow(50);
        break;
      case 5:
        rainbow(20);
        break;
      default:
        white();
        break;
    }
  } else {
    off();
  }

  if (!client.connected()) {
    reconnect();
  }

  client.loop();
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(CLIENT_NAME)) {
      Serial.println("connected");

      client.subscribe("katyled/brightness");
      client.subscribe("katyled/colour/red");
      client.subscribe("katyled/colour/green");
      client.subscribe("katyled/colour/blue");
      client.subscribe("katyled/type/lamp");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");

      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String param = "";

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    param += (char)payload[i];
  }
  Serial.println();

  if (String(topic) == "katyled/brightness") {
    brightness = param.toInt();

    EEPROM.write(ADDRESS_SELECTED_BRIGHTNESS, brightness);
    EEPROM.commit();
  }
  if (String(topic) == "katyled/colour/red") {
    red = param.toInt();

    EEPROM.write(ADDRESS_SELECTED_RED, red);
    EEPROM.commit();
  }
  if (String(topic) == "katyled/colour/green") {
    green = param.toInt();

    EEPROM.write(ADDRESS_SELECTED_GREEN, green);
    EEPROM.commit();    
  }
  if (String(topic) == "katyled/colour/blue") {
    blue = param.toInt();

    EEPROM.write(ADDRESS_SELECTED_BLUE, blue);
    EEPROM.commit();    
  }
  if (String(topic) == "katyled/type/lamp") {
    select_type = param.toInt();
    
    EEPROM.write(ADDRESS_SELECTED_TYPE, select_type);
    EEPROM.commit();
  }
}

void off() {
//  strip.setBrightness(0);
//  strip.begin();
  for (int x = 0; x < RGB_NUMPIXELS; x++) {
    strip.setPixelColor(x, 0, 0, 0);
  }
  strip.show();
}

void white() {
  red = EEPROM.read(ADDRESS_SELECTED_RED);
  green = EEPROM.read(ADDRESS_SELECTED_GREEN);
  blue = EEPROM.read(ADDRESS_SELECTED_BLUE);
  
  for (int x = 0; x < RGB_NUMPIXELS; x++) {
    switch (x) {
      case 0:
        strip.setPixelColor(x, red, green, blue);
        break;
      case 1:
        strip.setPixelColor(x, red, green, blue);
        break;
      case 2:
        strip.setPixelColor(x, red, green, blue);
        break;
      case 3:
        strip.setPixelColor(x, red, green, blue);
        break;
      case 4:
        strip.setPixelColor(x, red, green, blue);
        break;
      case 5:
        strip.setPixelColor(x, red, green, blue);
        break;
      case 6:
        strip.setPixelColor(x, red, green, blue);
        break;
      case 7:
        strip.setPixelColor(x, red, green, blue);
        break;
    }
  }

  strip.show();

  delay(random(50, 150));
}

void fire() {
  int r = 255;
  int g = r - 40;
  int b = 40;

  for (int x = 0; x < RGB_NUMPIXELS; x++) {
    int flicker = random(0, 150);
    int r1 = r - flicker;
    int g1 = g - flicker;
    int b1 = b - flicker;
    if (g1 < 0) g1 = 0;
    if (r1 < 0) r1 = 0;
    if (b1 < 0) b1 = 0;
    strip.setPixelColor(x, r1, g1, b1);
  }
  strip.show();

  delay(random(50, 150));
}

void rainbowMatrix() {
  for (int i = 0; i < RGB_NUMPIXELS; i++) {
    strip.setPixelColor(i, Wheel(((i * 256 / RGB_NUMPIXELS) + matrixPos) & 255));
  }
  strip.show();

  delay(20);

  matrixPos++;
  if (matrixPos == 1281) matrixPos = 0;
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < RGB_NUMPIXELS; i++) {
      strip.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < RGB_NUMPIXELS; i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

void theaterChaseRainbow(uint8_t wait) {
  for (int j = 0; j < 256; j++) {   // cycle all 256 colors in the wheel
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < RGB_NUMPIXELS; i = i + 3) {
        strip.setPixelColor(i + q, Wheel( (i + j) % 255)); //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i = 0; i < RGB_NUMPIXELS; i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
