#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define CLIENT_NAME "client3dPrinter"

const char *ssid =  "Ntk-39";  // Имя вайфай точки доступа
const char *password =  "506938506938"; // Пароль от точки доступа

const char *mqtt_server = "192.168.1.106"; // Имя сервера MQTT
const int mqtt_port = 1883; // Порт для подключения к серверу MQTT

const char *subscribe_string = "printer/switch";

WiFiClient espClient;
PubSubClient client(espClient);

int tm = 300;
char t1[20];

#define RELAY_PIN            D0

void setup() {
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);

  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  } else {
    statusSend();
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

      client.subscribe(subscribe_string);
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

  if (String(topic) == subscribe_string) {
    sendSwitch(payload, length);
  }
}

void statusSend() {
  if (tm == 0) {
    if (client.connect(CLIENT_NAME)) {
      Serial.println("PRINTER");
      sprintf(t1, "%s", String(digitalRead(RELAY_PIN)).c_str());
      Serial.print("Status = ");
      Serial.println(t1);
      client.publish("printer/switch/status", t1);
    }
    tm = 300;
  }
  tm--;
  delay(10);
}

void sendSwitch(byte* code, unsigned int length) {
  String sCode = "";

  for (int i = 0; i < length; i++) {
    sCode += (char)code[i];
  }

  Serial.print("Code send = ");
  Serial.println(sCode);

  if (sCode.toInt() == 1) {
    digitalWrite(RELAY_PIN, HIGH);
  }

  if (sCode.toInt() == 0) {
    digitalWrite(RELAY_PIN, LOW);
  }
}
