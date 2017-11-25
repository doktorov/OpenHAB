#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_HTU21DF.h>

#define CLIENT_NAME "clientWeatherStation1"

Adafruit_BME280 bme; // I2C
Adafruit_HTU21DF htu = Adafruit_HTU21DF();

const char *ssid =  "Ntk-39";  // Имя вайфай точки доступа
const char *password =  "506938506938"; // Пароль от точки доступа

const char *mqtt_server = "192.168.1.106"; // Имя сервера MQTT
const int mqtt_port = 1883; // Порт для подключения к серверу MQTT
const char *mqtt_user = ""; // Логи от сервер
const char *mqtt_pass = ""; // Пароль от сервера

//const char* narodmon_host = "narodmon.ru";
//const int narodmon_port = 8283;

WiFiClient espClient;
PubSubClient client(espClient);

#define LOCAL_TIMER 300
//#define NARODMON_TIMER 60000

int tmLocal = 0;
//int tmNarodMon = 0;
char t1[20];

void setup() {
  Serial.begin(115200);

  Serial.println(F("BME280 test"));
  if (!bme.begin()) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    //while (1);
  }

  Serial.println("HTU21D-F test");
  if (!htu.begin()) {
    Serial.println("Couldn't find sensor!");
    //while (1);
  }

  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);
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
        Serial.println("Local sending...");
        Serial.println("BME280");
        sprintf(t1, "%s", String(bme.readTemperature()).c_str());        
        Serial.print("Temperature = ");
        Serial.println(t1);
        client.publish("street1/temp_street", t1);

        sprintf(t1, "%s", String(bme.readPressure() / 100 * 0.75).c_str());
        Serial.print("Pressure = ");
        Serial.println(t1);
        client.publish("street1/pressure_street", t1);

        sprintf(t1, "%s", String(bme.readHumidity()).c_str());        
        Serial.print("Humidity = ");
        Serial.println(t1);
        client.publish("street1/humidity_street", t1);

        Serial.println("HTU21DF");
        sprintf(t1, "%s", String(htu.readTemperature()).c_str());     
        Serial.print("Temperature = ");
        Serial.println(t1);
        client.publish("street1/temp_badroom", t1);

        sprintf(t1, "%s", String(htu.readHumidity()).c_str());  
        Serial.print("Humidity = ");
        Serial.println(t1);
        client.publish("street1/humidity_badroom", t1);
      }
    }
    tmLocal = LOCAL_TIMER;
  }
  tmLocal--;

//  if (tmNarodMon == 0) {
//    if (WiFi.status() == WL_CONNECTED) {
//      if (espClient.connect(narodmon_host, narodmon_port)) {
//        Serial.println("Narod Monitoring sending...");
//        // заголовок
//        espClient.print("#");
//        espClient.print(WiFi.macAddress()); // отправляем МАС нашей ESP8266
//        espClient.print("#");
//        espClient.print("ESP8266+BME280"); // название устройства
//        espClient.print("#");
//        espClient.print("54.940994#83.184592"); // координаты местонахождения датчика
//        espClient.println();
//
//        espClient.print("#T1#");
//        espClient.print(bme.readTemperature());
//        espClient.print("#Температура");
//        espClient.println();
//
//        espClient.print("#H1#");
//        espClient.print(bme.readHumidity());        
//        espClient.print("#Влажность");
//        espClient.println();
//
//        espClient.print("#P1#");
//        espClient.print(bme.readPressure() / 100 * 0.75);
//        espClient.print("#Атм. давление");
//        espClient.println();
//
//        espClient.print("##");
//
//        // читаем ответ с и отправляем его в сериал
//        // вообще на ответ нужно както реагировать
//        Serial.print("Requesting: ");
//        while (espClient.available()) {
//          String line = espClient.readStringUntil('\r');
//          Serial.print(line);
//        }
//      }
//    }
//    tmNarodMon = NARODMON_TIMER;
//  }
//  tmNarodMon--;

  delay(10);
}
