#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <HTTPUpdate.h>

#include <MQUnifiedsensor.h>
#include <WiFi.h>
#include <DHT.h>
#include <EEPROM.h>
#include <ESP32Servo.h>

#define TOKEN "vZxIe39CD627irV7Zl5B"
#define FW_VERSION "1_0"  // phiên bản của code

#define from_DHT22_Heat (1)
#define from_DHT22_Humid (2)
#define from_MQ2 (3)
#define from_RainSensor (4)
#define bulb_status (5)
#define window_status (6)
#define from_server (7)

#define Board ("ESP-32")
#define MQ2_Pin (39)
#define Type ("MQ-2")
#define Voltage_Resolution (3.3)
#define ADC_Bit_Resolution (12)
#define RatioMQ2CleanAir (9.83)

#define RainSensor_Pin (34)

#define DHT_Pin (33)

#define lightBulb_Pin (27)
#define lightBulbSwitch_Pin (21)

#define buzzer_Pin (19)
#define openWindow_Pin (22)
#define closeWindow_Pin (23)

#define servoPin (25)

String server1 = "http://huyhoa308.000webhostapp.com";  // tên miền riêng để check OTA

char thingsboardServer[] = "mqtt.thingsboard.cloud";
WiFiClient wifiClient;
PubSubClient client(wifiClient);
int status = WL_IDLE_STATUS;

// Variables
int statusCode;
String ssid = "";
String pass = "";
String st;
String content;

unsigned long previousMillis = 0;
const unsigned long interval = 18000000;
unsigned long lastSend;

bool windowFlag = false;

// volatile bool bulbFlag = false;
// void sendStatusToServer();
// void sendWindowStatus(bool status);

MQUnifiedsensor MQ2(Board, Voltage_Resolution, ADC_Bit_Resolution, MQ2_Pin, Type);
DHT dht(DHT_Pin, DHT22);
Servo myservo;


bool testWifi(void);
void launchWeb(void);
void setupAP(void);
void readWifiEEPROM();
void OTA_update();
void on_message(char *topic, byte *payload, unsigned int length);
void InitWiFi();
void connectThingsboard();
void reconnect();

void MQ2_Setup();
void DHT_Setup();
static void vReadMQ2(void *pvParameters);
static void vReadDHT22Heat(void *pvParameters);
static void vReadDHT22Humid(void *pvParameters);
static void vReadRainSensor(void *pvParameters);
static void vBulbStatus(void *pvParameters);
static void vWindowStatus(void *pvParameters);
static void vReceiverTask(void *pvParameters);
static void vConnect(void *pvParameters);
static void IRAM_ATTR control_Bulb(void *pvParameters);
// static void IRAM_ATTR send_bulb_status(void *pvParameters);
static void IRAM_ATTR open_Window(void *pvParameters);
static void IRAM_ATTR close_Window(void *pvParameters);

WebServer server(80);

QueueHandle_t xQueue;
SemaphoreHandle_t xMutex;
SemaphoreHandle_t xLightBulbSemaphore;
// SemaphoreHandle_t xLightBulbStatusSemaphore;
SemaphoreHandle_t xOpenWindowSemaphore;
SemaphoreHandle_t xCloseWindowSemaphore;

int pos = 0;  // variable to store the servo position
bool gas_Warning = false;
//bool close_window = false;
//bool open_window = false;

typedef struct xData {
  float ucValue;
  unsigned char ucSource;
  char topic[200];
  char payload[200];
  unsigned int length;
} xData;

void setup(void) {
  Serial.begin(115200);
  EEPROM.begin(512);
  Serial.println("Firmware Version: " FW_VERSION);
  lastSend = 0;

  InitWiFi();
  client.setServer(thingsboardServer, 1883);
  connectThingsboard();
  client.setCallback(on_message);

  MQ2_Setup();

  dht.begin();
  DHT_Setup();

  myservo.setPeriodHertz(50);  // standard 50 hz servo
  myservo.attach(servoPin);

  xQueue = xQueueCreate(1, sizeof(xData));
  xMutex = xSemaphoreCreateMutex();

  xLightBulbSemaphore = xSemaphoreCreateBinary();
  // xLightBulbStatusSemaphore = xSemaphoreCreateBinary();

  xOpenWindowSemaphore = xSemaphoreCreateBinary();
  xCloseWindowSemaphore = xSemaphoreCreateBinary();

  attachInterrupt(digitalPinToInterrupt(openWindow_Pin), openWindowInterruptHandler, RISING);
  attachInterrupt(digitalPinToInterrupt(closeWindow_Pin), closeWindowInterruptHandler, RISING);

  attachInterrupt(digitalPinToInterrupt(lightBulbSwitch_Pin), lightInterruptHandler, RISING);

  pinMode(openWindow_Pin, INPUT);
  pinMode(closeWindow_Pin, INPUT);

  pinMode(lightBulbSwitch_Pin, INPUT);
  pinMode(lightBulb_Pin, OUTPUT);

  pinMode(MQ2_Pin, INPUT);
  pinMode(buzzer_Pin, OUTPUT);

  pinMode(RainSensor_Pin, INPUT);

  pinMode(DHT_Pin, INPUT);

  if (xQueue != NULL && xMutex != NULL) {

    xTaskCreate(vReadMQ2, "Read MQ2", 2000, NULL, 3, NULL);
    xTaskCreate(vReadDHT22Heat, "Read DHT22 Heat", 2000, NULL, 3, NULL);
    xTaskCreate(vReadDHT22Humid, "Read DHT22 Humid", 2000, NULL, 3, NULL);
    xTaskCreate(vReadRainSensor, "Read Rain Sensor", 2000, NULL, 3, NULL);
    xTaskCreate(vBulbStatus, "Read Status", 2000, NULL, 3, NULL);
    xTaskCreate(vWindowStatus, "Read Status", 2000, NULL, 3, NULL);
    xTaskCreate(vConnect, "Connect", 2000, NULL, 0, NULL);

    xTaskCreate(vReceiverTask, "Receiver", 5000, NULL, 2, NULL);
  }

  if (xLightBulbSemaphore != NULL) {

    xTaskCreate(control_Bulb, "light_bulb_Handler", 1024, NULL, 4, NULL);
  }

  // if (xLightBulbStatusSemaphore != NULL) {

  //   xTaskCreate(send_bulb_status, "bulb_status", 1024, NULL, 4, NULL);
  // }

  if (xOpenWindowSemaphore != NULL) {

    xTaskCreate(open_Window, "open_window_Handler", 1024, NULL, 1, NULL);
  }

  if (xCloseWindowSemaphore != NULL) {

    xTaskCreate(close_Window, "close_window_Handler", 1024, NULL, 1, NULL);
  }
}
static void vConnect(void *pvParameters){
  while ((WiFi.status() == WL_CONNECTED)) {
    client.loop();
  }
  reconnect();
  connectThingsboard();
  vTaskDelay(500 / portTICK_PERIOD_MS);

}


static void vReadMQ2(void *pvParameters) {

  const TickType_t xTicksToWait = 1500 / portTICK_PERIOD_MS;
  xData MQ2_Data;

  for (;;) {

    MQ2.update();
    MQ2_Data.ucValue = MQ2.readSensor();
    MQ2_Data.ucSource = from_MQ2;

    if (xQueueSend(xQueue, &MQ2_Data, 100 / portTICK_PERIOD_MS)) {
      vTaskDelay(xTicksToWait);
    }
  }
}

static void vReadDHT22Heat(void *pvParameters) {

  const TickType_t xTicksToWait = 1500 / portTICK_PERIOD_MS;
  xData DHT22_Heat_Data;

  for (;;) {

    DHT22_Heat_Data.ucValue = dht.readTemperature();
    DHT22_Heat_Data.ucSource = from_DHT22_Heat;

    if (xQueueSend(xQueue, &DHT22_Heat_Data, 100 / portTICK_PERIOD_MS)) {
      vTaskDelay(xTicksToWait);
    }
  }
}

static void vReadDHT22Humid(void *pvParameters) {

  const TickType_t xTicksToWait = 1500 / portTICK_PERIOD_MS;
  xData DHT22_Humid_Data;

  for (;;) {

    DHT22_Humid_Data.ucValue = dht.readHumidity();
    DHT22_Humid_Data.ucSource = from_DHT22_Humid;

    if (xQueueSend(xQueue, &DHT22_Humid_Data, 100 / portTICK_PERIOD_MS)) {
      vTaskDelay(xTicksToWait);
    }
  }
}

static void vBulbStatus(void *pvParameters) {

  const TickType_t xTicksToWait = 1500 / portTICK_PERIOD_MS;
  xData Switch_status;

  for (;;) {

    // DHT22_Humid_Data.ucValue = dht.readHumidity();
    Switch_status.ucSource = bulb_status;

    if (xQueueSend(xQueue, &Switch_status, 100 / portTICK_PERIOD_MS)) {
      vTaskDelay(xTicksToWait);
    }
  }
}

static void vWindowStatus(void *pvParameters) {

  const TickType_t xTicksToWait = 1500 / portTICK_PERIOD_MS;
  xData Switch_status;

  for (;;) {

    // DHT22_Humid_Data.ucValue = dht.readHumidity();
    Switch_status.ucSource = window_status;

    if (xQueueSend(xQueue, &Switch_status, 100 / portTICK_PERIOD_MS)) {
      vTaskDelay(xTicksToWait);
    }
  }
}

static void vReadRainSensor(void *pvParameters) {

  const TickType_t xTicksToWait = 1500 / portTICK_PERIOD_MS;
  xData RainSensor_Data;

  for (;;) {

    RainSensor_Data.ucValue = digitalRead(RainSensor_Pin);
    RainSensor_Data.ucSource = from_RainSensor;

    if (xQueueSend(xQueue, &RainSensor_Data, 100 / portTICK_PERIOD_MS)) {
      vTaskDelay(xTicksToWait);
    }
  }
}

static void vReceiverTask(void *pvParameters) {

  xData xReceivedStructure;
  xSemaphoreTake(xMutex, portMAX_DELAY);

  for (;;) {

    if (xQueueReceive(xQueue, &xReceivedStructure, portMAX_DELAY) == pdPASS) {

      switch (xReceivedStructure.ucSource) {

        case (from_MQ2):
          {

            Serial.print("From MQ2 = ");
            Serial.print(xReceivedStructure.ucValue);
            Serial.println(" ppm");

            if (xReceivedStructure.ucValue >= 60) {

              gas_Warning = true;
              digitalWrite(buzzer_Pin, HIGH);

            } else {

              gas_Warning = false;
              digitalWrite(buzzer_Pin, LOW);
            }

            StaticJsonDocument<200> jsonDocument;
            JsonObject data = jsonDocument.to<JsonObject>();
            data["LPG"] = xReceivedStructure.ucValue;

            char payload[256];
            serializeJson(data, payload);
            String strPayload = String(payload);
            Serial.println(strPayload);

            client.publish("v1/devices/me/telemetry", strPayload.c_str());
            break;
          }

        case (from_server):
          {
            char json[xReceivedStructure.length + 1];
            strncpy(json, (char *)xReceivedStructure.payload, xReceivedStructure.length);
            json[xReceivedStructure.length] = '\0';

            Serial.print("Topic: ");
            Serial.println(xReceivedStructure.topic);
            Serial.print("Message: ");
            Serial.println(xReceivedStructure.payload);

            StaticJsonDocument<200> jsonDocument;
            DeserializationError error = deserializeJson(jsonDocument, json);

            if (error) {
              Serial.println("deserializeJson() failed");
              return;
            }

            String methodName = String(jsonDocument["method"].as<const char *>());  // Explicitly convert to const char*

            if (methodName.equals("controlBulb")) {
              xSemaphoreGive(xLightBulbSemaphore);
              portYIELD();
            } else if (methodName.equals("controlWindow")) {
              if (jsonDocument["params"] == true) {
                // BaseType_t xHigherPriorityTaskWoken = pdFALSE;
                xSemaphoreGive(xOpenWindowSemaphore);
                portYIELD();
              } else {
                // BaseType_t xHigherPriorityTaskWoken = pdFALSE;
                xSemaphoreGive(xCloseWindowSemaphore);
                portYIELD();
              }
            }
          }

        case (bulb_status):
          {

            StaticJsonDocument<200> jsonDocument;
            JsonObject data = jsonDocument.to<JsonObject>();
            data["bulb"] = digitalRead(lightBulb_Pin);

            char payload[256];
            serializeJson(data, payload);
            String strPayload = String(payload);
            Serial.println(strPayload);

            client.publish("v1/devices/me/attributes", strPayload.c_str());
            break;
          }

        case (window_status):
          {

            StaticJsonDocument<200> jsonDocument;
            JsonObject data = jsonDocument.to<JsonObject>();
            data["window"] = windowFlag;

            char payload[256];
            serializeJson(data, payload);
            String strPayload = String(payload);
            Serial.println(strPayload);

            client.publish("v1/devices/me/attributes", strPayload.c_str());
            break;
          }

        case from_DHT22_Heat:
          {

            if (xReceivedStructure.ucValue > 1) {

              Serial.print("Temperature = ");
              Serial.print(xReceivedStructure.ucValue);
              Serial.println("°C");

              StaticJsonDocument<200> jsonDocument;
              JsonObject data = jsonDocument.to<JsonObject>();
              data["temperature"] = xReceivedStructure.ucValue;

              char payload[256];
              serializeJson(data, payload);
              String strPayload = String(payload);
              Serial.println(strPayload);

              client.publish("v1/devices/me/telemetry", strPayload.c_str());
            }
            break;
          }

        case from_DHT22_Humid:
          {

            if (xReceivedStructure.ucValue > 1) {

              Serial.print("Humidity = ");
              Serial.print(xReceivedStructure.ucValue);
              Serial.println("%");

              StaticJsonDocument<200> jsonDocument;
              JsonObject data = jsonDocument.to<JsonObject>();
              data["humidity"] = xReceivedStructure.ucValue;

              char payload[256];
              serializeJson(data, payload);
              String strPayload = String(payload);
              Serial.println(strPayload);

              client.publish("v1/devices/me/telemetry", strPayload.c_str());
            }
            break;
          }

        case from_RainSensor:

          Serial.print("From Rain Sensor = ");

          if (!xReceivedStructure.ucValue) {

            Serial.println("Raining");
            xSemaphoreGive(xCloseWindowSemaphore);

          } else {

            Serial.println("Weather clear");
          }
          break;

        default:
          {

            Serial.println("Can not recognize this Data");
            break;
          }
      }
    }

    xSemaphoreGive(xMutex);
  }
}

static void lightInterruptHandler(void) {
  int time = millis();
  while (digitalRead(lightBulbSwitch_Pin)) {
    if (millis() - time > 2000) break;
  }
  while (millis() - time <= 100) {
    ;
  }
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xSemaphoreGiveFromISR(xLightBulbSemaphore, (BaseType_t *)&xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

  while (millis() - time <= 200) {
    ;
  }
}

static void openWindowInterruptHandler(void) {

  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xSemaphoreGiveFromISR(xOpenWindowSemaphore, (BaseType_t *)&xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static void closeWindowInterruptHandler(void) {

  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xSemaphoreGiveFromISR(xCloseWindowSemaphore, (BaseType_t *)&xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void on_message(char *topic, byte *payload, unsigned int length) {
  xData msg;
  Serial.println("on message");

  strncpy(msg.topic, topic, sizeof(msg.topic) + 1);
  msg.topic[sizeof(msg.topic)] = '\0';

  strncpy(msg.payload, (char *)payload, sizeof(msg.payload) + 1);
  msg.payload[sizeof(msg.payload) ] = '\0';

  msg.length = length;
  msg.ucSource = from_server;  // Set the source of the message

  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xQueueSendFromISR(xQueue, &msg, &xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static void IRAM_ATTR control_Bulb(void *pvParameters) {

  for (;;) {

    xSemaphoreTake(xLightBulbSemaphore, portMAX_DELAY);
    Serial.print("Control bulb task - Processing event.\r\n");

    digitalWrite(lightBulb_Pin, !(digitalRead(lightBulb_Pin)));

    // BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    // xSemaphoreGiveFromISR(xLightBulbStatusSemaphore, (BaseType_t *)&xHigherPriorityTaskWoken);
    // portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}


static void IRAM_ATTR open_Window(void *pvParameters) {

  for (;;) {

    xSemaphoreTake(xOpenWindowSemaphore, portMAX_DELAY);
    Serial.print("Open window task - Processing event.\r\n");

    if (digitalRead(RainSensor_Pin) && myservo.read() < 5) {

      for (pos = 0; pos <= 180; pos += 1) {
        myservo.write(pos);
        delay(15);
      }
      
      windowFlag = true;
    }

  }
}

static void IRAM_ATTR close_Window(void *pvParameters) {

  for (;;) {

    xSemaphoreTake(xCloseWindowSemaphore, portMAX_DELAY);
    Serial.print("Close window task - Processing event.\r\n");

    if (myservo.read() > 175) {

      for (pos = 180; pos >= 0; pos -= 1) {
        myservo.write(pos);
        delay(15);
      }
      windowFlag = false;
    }

  }
}

void loop() {
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


void DHT_Setup() {

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {

    Serial.println(F("Failed to read from DHT sensor!"));

    while (1) {
      if (!isnan(h) && !isnan(t)) {
        break;
      }
    }
  }
}

void MQ2_Setup() {

  MQ2.setRegressionMethod(1);
  MQ2.setA(574.25);
  MQ2.setB(-2.222);
  MQ2.init();

  Serial.print("Calibrating please wait.");

  float calcR0 = 0;

  for (int i = 1; i <= 10; i++) {

    MQ2.update();
    calcR0 += MQ2.calibrate(RatioMQ2CleanAir);
    Serial.print(".");
  }

  MQ2.setR0(calcR0 / 10);
  Serial.println("  done!.");

  if (isinf(calcR0)) {

    Serial.println("Warning: Conection issue, R0 is infinite (Open circuit detected) please check your wiring and supply");
    while (1) {
      if (!isinf(calcR0)) {

        break;
      }
    }
  }
  if (calcR0 == 0) {

    Serial.println("Warning: Conection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply");

    while (1) {
      if (calcR0 != 0) {
        break;
      }
    }
  }
}

/*----------------------------------------------------------Init Wifi--------------------------------------------------------------------------------------------------------------*/
void InitWiFi() {
  Serial.println("Reading EEPROM ssid");

  for (int i = 0; i < 32; ++i) {
    ssid += char(EEPROM.read(i));
  }
  Serial.println();
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.println("Reading EEPROM pass");

  for (int i = 32; i < 96; ++i) {
    pass += char(EEPROM.read(i));
  }
  Serial.print("PASS: ");
  Serial.println(pass);

  WiFi.begin(ssid.c_str(), pass.c_str());
  if (testWifi()) {
    Serial.println("Succesfully Connected!!!");
    return;
  } else {
    Serial.println("Turning the HotSpot On");
    launchWeb();
    setupAP();  // Setup HotSpot
  }

  Serial.println();
  Serial.println("Waiting.");

  while ((WiFi.status() != WL_CONNECTED)) {
    Serial.print(".");
    delay(100);
    server.handleClient();
  }
}
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------Reconnect wifi--------------------------------------------------------------------------------------------------------*/
void reconnect() {
  while (!client.connected()) {
    WiFi.begin(ssid.c_str(), pass.c_str());
    if (testWifi()) {
      Serial.println("Succesfully Connected!!!");
      return;
    } else {
      Serial.println("Turning the HotSpot On");
      launchWeb();
      setupAP();  // Setup HotSpot
    }

    Serial.println();
    Serial.println("Waiting.");

    while ((WiFi.status() != WL_CONNECTED)) {
      Serial.print(".");
      delay(100);
      server.handleClient();
    }
  }
}
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


/*-------------------------------------------------------------- Connect Thingsboard  ------------------------ ------------------------------------------------------------------*/
void connectThingsboard() {
  Serial.print("Connecting to ThingsBoard node ...");

  if (client.connect("ESP32 Device", TOKEN, NULL)) {
    Serial.println("[DONE]");
    client.subscribe("v1/devices/me/rpc/request/+");
    client.subscribe("v1/devices/me/rpc/response/+");
    Serial.println("Sending current Firmware Version ...");
    client.publish("v1/devices/me/attributes", get_version().c_str());
    client.publish("v1/devices/me/attributes", get_status().c_str());
  } else {
    Serial.print("[FAILED] [ rc = ");
    Serial.print(client.state());
    Serial.println(" : retrying in 5 seconds]");
    delay(5000);
  }
}
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------OTA------------------------------------------------------------------------------------------------*/
void OTA_update() {
  WiFiClient client;
  Serial.println("Checking for updates...");

  t_httpUpdate_return ret = httpUpdate.update(client, server1, FW_VERSION);
  switch (ret) {
    case HTTP_UPDATE_FAILED:
      Serial.printf("Update failed! Error (%d): %s\n", httpUpdate.getLastError(),
                    httpUpdate.getLastErrorString().c_str());
      break;
    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("No updates available");
      break;
    case HTTP_UPDATE_OK:
      Serial.println("Update successful");
      // Optionally, you may want to reset the board after a successful update
      // ESP.restart();
      break;
  }
}
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------Get Firmware Version--------------------------------------------------------------------------------------------------------*/
String get_version() {
  StaticJsonDocument<200> jsonDocument;
  JsonObject data = jsonDocument.to<JsonObject>();
  data["version"] = FW_VERSION;

  char payload[256];
  serializeJson(data, payload);
  String strPayload = String(payload);
  Serial.print("Get firmware version: ");
  Serial.println(strPayload);
  return strPayload;
}
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------Get Status--------------------------------------------------------------------------------------------------------*/
String get_status() {
  StaticJsonDocument<200> jsonDocument;
  JsonObject data = jsonDocument.to<JsonObject>();
  data["window"] = windowFlag ? true : false;
  data["bulb"] = digitalRead(lightBulb_Pin);

  char payload[256];
  serializeJson(data, payload);
  String strPayload = String(payload);
  Serial.print("Get status: ");
  Serial.println(strPayload);
  return strPayload;
}
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------Create Webserver-----------------------------------------------------------------------------------------------------*/
void createWebServer() {
  server.on("/", HTTP_GET, []() {
    IPAddress ip = WiFi.softAPIP();
    String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
    content = "<!DOCTYPE HTML>\r\n<html>";

    // Styling for centering the content
    content += "<head><style>body { font-family: Arial, sans-serif; background-color: #f4f4f4; margin: 0; padding: 0; display: flex; align-items: center; justify-content: center; height: 100vh; }";
    content += ".container { max-width: 400px; background-color: #fff; padding: 20px; border-radius: 10px; box-shadow: 0px 0px 10px #ccc; }";
    content += "h1 { color: #333; text-align: center; }";
    content += "label { color: #333; }";
    content += "input[type='text'], input[type='password'] { width: calc(100% - 22px); padding: 10px; margin: 10px 0; border: 1px solid #ccc; border-radius: 5px; }";
    content += "input[type='submit'] { background-color: #007bff; color: #fff; border: none; padding: 10px 20px; border-radius: 5px; cursor: pointer; width: 100%; }";
    content += "</style></head>";

    content += "<body><div class='container'>";
    content += "<h1>Hello from ESP32 at " + ipStr + "</h1>";
    content += "<form action=\"/scan\" method=\"POST\"><input type=\"submit\" value=\"Scan\"></form>";
    content += "<p>" + st + "</p>";
    content += "<form method='get' action='setting'>";
    content += "<label>SSID: </label><input type='text' name='ssid'><br>";
    content += "<label>Password: </label><input type='password' name='pass'><br>";
    content += "<input type='submit' value='Save'>";
    content += "</form>";

    // Group information with HTML entities
    content += "<p style='text-align:center; margin-top: 20px;'>Group 1: Truong Huy Hoa - Nguyen Nhat Minh</p>";

    content += "</div></body></html>";

    server.send(200, "text/html", content);
  });

  server.on("/", HTTP_GET, []() {
    IPAddress ip = WiFi.softAPIP();
    String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
    content = "<!DOCTYPE HTML>\r\n<html>";

    // Styling for centering the content
    content += "<head><style>body { font-family: Arial, sans-serif; background-color: #f4f4f4; margin: 0; padding: 0; display: flex; align-items: center; justify-content: center; height: 100vh; }";
    content += ".container { max-width: 400px; background-color: #fff; padding: 20px; border-radius: 10px; box-shadow: 0px 0px 10px #ccc; }";
    content += "h1 { color: #333; text-align: center; }";
    content += "label { color: #333; }";
    content += "input[type='text'], input[type='password'] { width: 100%; padding: 10px; margin: 10px 0; border: 1px solid #ccc; border-radius: 5px; }";
    content += "input[type='submit'] { background-color: #007bff; color: #fff; border: none; padding: 10px 20px; border-radius: 5px; cursor: pointer; width: 100%; }";
    content += "</style></head>";

    content += "<body><div class='container'>";
    content += "<h1>Hello from ESP32 at " + ipStr + "</h1>";
    content += "<form action=\"/scan\" method=\"POST\"><input type=\"submit\" value=\"Scan\"></form>";
    content += "<p>" + st + "</p>";
    content += "<form method='get' action='setting'>";
    content += "<label>SSID: </label><input name='ssid' length=32><br>";
    content += "<label>Password: </label><input type='password' name='pass' length=64><br>";
    content += "<input type='submit' value='Save'>";
    content += "</form>";

    // Group information with HTML entities
    content += "<p style='text-align:center; margin-top: 20px;'>Group 1: Truong Huy Hoa - Nguyen Nhat Minh</p>";

    content += "</div></body></html>";

    server.send(200, "text/html", content);
  });

  server.on("/setting", HTTP_GET, []() {
    String qsid = server.arg("ssid");
    String qpass = server.arg("pass");
    if (qsid.length() > 0 && qpass.length() > 0) {
      Serial.println("Clearing EEPROM");
      for (int i = 0; i < 96; ++i) {
        EEPROM.write(i, 0);
      }
      Serial.println(qsid);
      Serial.println("");
      Serial.println(qpass);
      Serial.println("");

      Serial.println("Writing EEPROM SSID:");
      for (int i = 0; i < qsid.length(); ++i) {
        EEPROM.write(i, qsid[i]);
        Serial.print("Wrote: ");
        Serial.println(qsid[i]);
      }
      Serial.println("Writing EEPROM Password:");
      for (int i = 0; i < qpass.length(); ++i) {
        EEPROM.write(32 + i, qpass[i]);
        Serial.print("Wrote: ");
        Serial.println(qpass[i]);
      }
      EEPROM.commit();

      content = "{\"Success\":\"Saved to EEPROM... Reset to boot into new WiFi\"}";
      statusCode = 200;
      ESP.restart();  // Use restart instead of reset on ESP32
    } else {
      content = "{\"Error\":\"404 Not Found\"}";
      statusCode = 404;
      Serial.println("Sending 404");
    }
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(statusCode, "application/json", content);
  });
}
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------Test Wifi-----------------------------------------------------------------------------------------------*/
bool testWifi(void) {
  int c = 0;
  Serial.println("Waiting for Wifi to connect");
  while (c < 20) {
    if (WiFi.status() == WL_CONNECTED) {
      return true;
    }
    delay(500);
    Serial.print("*");
    c++;
  }
  Serial.println("");
  Serial.println("Connect timed out, opening AP");
  return false;
}
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------Launch Web-------------------------------------------------------------------------------------------*/
void launchWeb() {
  Serial.println("");
  if (WiFi.status() == WL_CONNECTED)
    Serial.println("WiFi connected");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("SoftAP IP: ");
  Serial.println(WiFi.softAPIP());
  createWebServer();
  // Start the server
  server.begin();
  Serial.println("Server started");
}
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------Set up access point------------------------------------------------------------------------------------------------*/
void setupAP(void) {
  WiFi.disconnect(true);  // Disconnect WiFi on ESP32
  delay(100);
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
      delay(10);
    }
  }
  Serial.println("");
  st = "<ol>";
  for (int i = 0; i < n; ++i) {
    // Print SSID and RSSI for each network found
    st += "<li>";
    st += WiFi.SSID(i);
    st += " (";
    st += WiFi.RSSI(i);

    st += ")";
    st += (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*";
    st += "</li>";
  }
  st += "</ol>";
  delay(100);
  WiFi.softAP("PBL3", "");
  Serial.println("softap");
  launchWeb();
  Serial.println("over");
}
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
