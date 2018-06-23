#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <DallasTemperature.h>

#define SLEEP_DELAY_IN_SECONDS  30
#define ONE_WIRE_BUS            D4      // DS18B20 pin

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);

#define CLIENT_NAME "clientWeatherStationDS18B20"

const char *ssid =  "Ntk-39";  // Имя вайфай точки доступа
const char *password =  "506938506938"; // Пароль от точки доступа

const char *mqtt_server = "192.168.1.106"; // Имя сервера MQTT
const int mqtt_port = 1883; // Порт для подключения к серверу MQTT
const char *mqtt_user = "openhabian"; // Логи от сервер
const char *mqtt_pass = "openhabian"; // Пароль от сервера

WiFiClient espClient;
PubSubClient client(espClient);

#define LOCAL_TIMER 300

int tmLocal = 0;

char t1[20];

void setup() {
  Serial.begin(115200);

  DS18B20.begin();

  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  //  float temperature = getTemperature();
  //  // convert temperature to a string with two digits before the comma and 2 digits for precision
  //  //dtostrf(temperature, 2, 2, temperatureString);
  //  sprintf(t1, "%s", String(temperature).c_str());
  //  // send temperature to the serial console
  //  Serial.print("Temperature = ");
  //  Serial.println(t1);
  //
  //  delay(500);

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
    if (client.connect(CLIENT_NAME)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");

      delay(5000);
    }
  }
}

void tempSend() {
  if (tmLocal == 0) {
    if (WiFi.status() == WL_CONNECTED) {
      if (client.connect(CLIENT_NAME)) {
        float temperature = getTemperature();
        
        Serial.println("Local sending...");
        Serial.println("DS18B20");
        sprintf(t1, "%s", String(temperature).c_str());
        Serial.print("Temperature = ");
        Serial.println(t1);
        client.publish("street/temp/north", t1);
      }
    }
    tmLocal = LOCAL_TIMER;
  }
  tmLocal--;

  delay(10);
}

float getTemperature() {
  float temp;
  do {
    DS18B20.requestTemperatures();
    temp = DS18B20.getTempCByIndex(0);
    delay(100);
  } while (temp == 85.0 || temp == (-127.0));
  return temp;
}
