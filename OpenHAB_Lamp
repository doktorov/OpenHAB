#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <Adafruit_NeoPixel.h>

#define CLIENT_NAME "clientKatyLED"

#define PIN_ONOFF D0
#define PIN D2

const char *ssid =  "Ntk-39";  // Имя вайфай точки доступа
const char *password =  "506938506938"; // Пароль от точки доступа

const char *mqtt_server = "192.168.1.106"; // Имя сервера MQTT
const int mqtt_port = 1883; // Порт для подключения к серверу MQTT
//const char *mqtt_user = ""; // Логи от сервер
//const char *mqtt_pass = ""; // Пароль от сервера

WiFiClient espClient;
PubSubClient client(espClient);

Adafruit_NeoPixel strip = Adafruit_NeoPixel(8, PIN, NEO_GRB + NEO_KHZ800);

int val = 0;

int result = 10;

int select_type = 0;

int brightness = 10;
int red = 127;
int green = 127;
int blue = 127;

int repeat = 0;

void setup() {
  Serial.begin(115200);

  pinMode(PIN_ONOFF, INPUT);

  strip.setBrightness(brightness);
  strip.begin();
  strip.show();

  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  static int pos = 0;

  val = digitalRead(PIN_ONOFF);
  if (val == 0) {
    strip.setBrightness(brightness * 2.5);
    strip.begin();
    strip.show();

    switch (select_type) {
      case 0:
        white();
        break;
      case 1:
        fire();
        break;
      case 2:       
        if (repeat == 0) {          
          rainbow(20);
        }

        if (repeat == 21) {
          repeat = 0;
        }

        repeat++;
        break;
      case 3:
        rainbowCycle(20);
        break;
      case 4:
        theaterChaseRainbow(50);
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
  }
  if (String(topic) == "katyled/colour/red") {
    red = param.toInt();
  }
  if (String(topic) == "katyled/colour/green") {
    green = param.toInt();
  }
  if (String(topic) == "katyled/colour/blue") {
    blue = param.toInt();
  }
  if (String(topic) == "katyled/type/lamp") {
    select_type = param.toInt();
  }
}


void off() {
  strip.setBrightness(0);
  strip.begin();
  strip.show();
}

void white() {
  for (int x = 0; x < 8; x++) {
    strip.setPixelColor(x, red, green, blue);
  }

  strip.show();
}

void fire() {
  int r = 255;
  int g = r - 40;
  int b = 40;

  for (int x = 0; x < 8; x++) {
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

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip.show();
    //delay(wait);
  }
}

void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

void theaterChaseRainbow(uint8_t wait) {
  for (int j = 0; j < 256; j++) {   // cycle all 256 colors in the wheel
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, Wheel( (i + j) % 255)); //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
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
