#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#include <RCSwitch.h>

Adafruit_BMP280 bmp; // I2C

const char *ssid =  "Ntk-39";  // Имя вайфай точки доступа
const char *password =  "506938506938"; // Пароль от точки доступа

const char *mqtt_server = "192.168.1.106"; // Имя сервера MQTT
const int mqtt_port = 1883; // Порт для подключения к серверу MQTT
const char *mqtt_user = "openhabian"; // Логи от сервер
const char *mqtt_pass = "openhabian"; // Пароль от сервера

WiFiClient espClient;
PubSubClient client(espClient);

RCSwitch mySwitch = RCSwitch();

int tm = 300;
char t1[20];

void setup() {
  Serial.begin(115200);
  
  pinMode(D3, OUTPUT);
  digitalWrite(D3, LOW); 

  mySwitch.enableTransmit(D4);  

  Serial.println(F("BMP280 test"));
  if (!bmp.begin()) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }

  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  } else {
    tempSend();
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
    if (client.connect("arduinoClient2")) {
      Serial.println("connected");

      client.subscribe("test/led");
      client.subscribe("hall/switch");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");

      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if (String(topic) == "hall/switch") {
    sendRC(payload, length);
  }
}

void tempSend() {
  if (tm == 0) {
    if (client.connect("arduinoClient2")) {
      sprintf(t1, "%s", String(bmp.readTemperature()).c_str());
      Serial.print("Temperature = ");
      Serial.println(t1);
      client.publish("balkon/temp_balkon", t1);

      sprintf(t1, "%s", String(bmp.readPressure() / 100 * 0.75).c_str());
      Serial.print("Pressure = ");
      Serial.println(t1);
      client.publish("balkon/pressure_balkon", t1);
    }
    tm = 300;
  }
  tm--;
  delay(10);
}

void sendRC(byte* code, unsigned int length) {
  String sCode = "";
  
  for (int i = 0; i < length; i++) {
    sCode += (char)code[i];
  }
  
  Serial.print("Code send = ");
  digitalWrite(D3, HIGH);
  
  delay(10);

  Serial.println(sCode);
  mySwitch.send(sCode.toInt(), 24);

  delay(10);

  digitalWrite(D3, LOW);
  Serial.println("Sended");
}
