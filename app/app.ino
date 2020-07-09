// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Please use an Arduino IDE 1.6.8 or greater

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiUdp.h>

#include <AzureIoTHub.h>
#include <AzureIoTProtocol_MQTT.h>
#include <AzureIoTUtility.h>
#include "iothubtransportmqtt.h"   // Add this because if not, there will be a "MQTT_protocol" not declare error

#include <SoftwareSerial.h> //GPS
#include <TinyGPS++.h> //GPS
#include "QuickStats.h" //GPS
static const int RXPin = D2, TXPin = D3;  //Rx for the arduino to TX of the GPS,vice versa
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);
const int no_of_sample = 250;
int pos = 0;
unsigned int interval_sec = 30;
//long elapsed;

float northing[no_of_sample];
float easting[no_of_sample];

float northing_final = 0;
float easting_final = 0;

float north_bound = 1;
float east_bound = 1;

unsigned int time_to_wait_GPS_steady = 0; //config. (second)
//delta in north, 0.00001 = 1.03m
float GPS_north_filter = 0.00007; //config
//delta in east, 0.00001 = 1.108m
float GPS_east_filter = 0.00007; //config

int GPS_filter_count = 0;
const int GPS_filter_threshold = interval_sec * 2; //config

unsigned long lastTimeReceived = 0;
QuickStats stats; //initialize an instance of this class



#include <Adafruit_MCP3008.h>
Adafruit_MCP3008 adc;

#include "config.h"

static bool messagePending = false;
static bool messageSending = true;

//***** Change SSID, Pass, and connection string **********************
#define IOT_CONFIG_WIFI_SSID            "your_ssid"
#define IOT_CONFIG_WIFI_PASSWORD        "your_pw"

const char *connectionString = "your_connection_string";
const char *ssid = IOT_CONFIG_WIFI_SSID;
const char *pass = IOT_CONFIG_WIFI_PASSWORD;


int status = WL_IDLE_STATUS;

static int interval = 10000;
float reset_time = millis() / 1000;

float reading[8];

void blinkLED()
{
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
}

void initWifi()
{
  Serial.println("Init WiFi");
  if (WiFi.status() == WL_NO_SHIELD)
  {
    Serial.println("Wifi shield not present");
    while (true);
  }

  status = WiFi.begin(ssid, pass);
  //*****************************************************

  delay(3000);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("wait...");
  }
}

void initTime()
{
  time_t epochTime;
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  while (true)
  {
    epochTime = time(NULL);

    if (epochTime == 0)
    {
      Serial.println("Fetching NTP epoch time failed! Waiting 2 seconds to retry.");
      delay(2000);
    }
    else
    {
      Serial.printf("Fetched NTP epoch time is: %lu.\r\n", epochTime);
      break;
    }
  }
}

static IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  delay(2000);
  //***** GPS **********************
  ss.begin(GPSBaud);
  memset(northing, '\0', sizeof(northing));
  //Serial.println(sizeof(northing));// 40 if = float; 20 if = int
  memset(easting, '\0', sizeof(easting));
  //*****Mcp3008 pin**********************
  // (sck, mosi, miso, cs);
  adc.begin(D8, D6, D7, D5);
  
  initWifi();
  initTime();  //necessary. Otherwise, cannot keep sending message

  /*
    Break changes in version 1.0.34: AzureIoTHub library removed AzureIoTClient class.
    So we remove the code below to avoid compile error.
  */
  // initIoThubClient();


  Serial.println("get iot hub client...");
  iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(connectionString, MQTT_Protocol);

  if (iotHubClientHandle == NULL)
  {
    Serial.println("Failed on IoTHubClient_CreateFromConnectionString.");
    while (1);
  }

  Serial.println("got client...");
  IoTHubClient_LL_SetOption(iotHubClientHandle, "product_info", "VOC-Monitor");
  if (IoTHubClient_LL_SetMessageCallback(iotHubClientHandle, receiveMessageCallback, NULL) != IOTHUB_CLIENT_OK)
  {
    Serial.println("ERROR: IoTHubClient_LL_SetMessageCallback..........FAILED!");
  }

  IoTHubClient_LL_SetDeviceMethodCallback(iotHubClientHandle, deviceMethodCallback, NULL);
  IoTHubClient_LL_SetDeviceTwinCallback(iotHubClientHandle, twinCallback, NULL);

  Serial.println("do work...");
  IoTHubClient_LL_DoWork(iotHubClientHandle);
}

static int messageCount = 1;
void loop()
{
  if (!messagePending && messageSending)
  {
    char messagePayload[MESSAGE_MAX_LEN];
    bool temperatureAlert = readMessage(messageCount, messagePayload);
    sendMessage(iotHubClientHandle, messagePayload, temperatureAlert);
    messageCount++;
    delay(interval);
  }
  IoTHubClient_LL_DoWork(iotHubClientHandle);
  delay(10);
      if (millis() / 1000 - reset_time >= 3000) {
    Serial.println("restart...");
    ESP.restart();
    reset_time = millis() / 1000;
  }
}
