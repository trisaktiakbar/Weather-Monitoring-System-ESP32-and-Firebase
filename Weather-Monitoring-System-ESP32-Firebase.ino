#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <Arduino.h>
#include <FirebaseESP32.h>
#include "DHT.h"
#include <ESP32Time.h>

#define FIREBASE_HOST "https://raraappproject-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "6LPhIZc36j9jZ3wSeSXOiw24jENwSAHODRlsxVs3"

#define WIFI_SSID "WIFI_SSID"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define PIN_CAHAYA 34
#define PIN_HUJAN 32
#define DHTPIN 4
#define DHTTYPE DHT22

ESP32Time rtc;

FirebaseData firebaseData;
FirebaseJson data_cahaya;
FirebaseJson data_suhu;
FirebaseJson data_hujan;
FirebaseJson data_kelembaban_udara;

int value_cahaya;
DHT dht(DHTPIN, DHTTYPE);
int value_kelembaban_udara;
int value_suhu;
int value_hujan;

String date;
String date_temp = " ";

void setup(){
//  client.setInsecure();
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Menghubungkan Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(firebaseData, "tiny");

  rtc.setTime(50, 45, 16, 3, 6, 2022); // Set time (Format : Detik, Menit, Jam, Tanggal, Bulan, Tahun

  dht.begin();
}

void loop()
{
  date = rtc.getTime("%Y-%m-%d %H-%M");
  Serial.println(date); // prints the value read Sensor Cahaya

  value_cahaya = analogRead(PIN_CAHAYA); // read analog input Sensor Cahaya
  Serial.println(value_cahaya);
  if(value_cahaya == 4095){
    value_cahaya = 0;
  }
  else{
    value_cahaya = 1;
  }
  Serial.println(value_cahaya);

  value_kelembaban_udara = dht.readHumidity();
  value_suhu = dht.readTemperature();
  value_hujan = analogRead(PIN_HUJAN);

  value_hujan = 4095 - value_hujan;
  value_hujan = value_hujan * 100 / 4095;

  Serial.print("Kelembaban Udara: ");
  Serial.print(value_kelembaban_udara);
  Serial.println("%");
  Serial.print("Suhu: ");
  Serial.print(value_suhu);
  Serial.println("'C");

  data_cahaya.set("Cahaya", value_cahaya);
  data_suhu.set("Suhu", value_suhu);
  data_kelembaban_udara.set("Kelembaban Udara", value_kelembaban_udara);
  data_hujan.set("Hujan", value_hujan);

  Firebase.updateNode(firebaseData, "Current Data", data_cahaya);
  Firebase.updateNode(firebaseData, "Current Data", data_suhu);
  Firebase.updateNode(firebaseData, "Current Data", data_kelembaban_udara);
  Firebase.updateNode(firebaseData, "Current Data", data_hujan);


  if(date_temp != date){
    Firebase.updateNode(firebaseData, "Collection Data/Cahaya/" + date, data_cahaya);
    Firebase.updateNode(firebaseData, "Collection Data/Suhu/" + date, data_suhu);
    Firebase.updateNode(firebaseData, "Collection Data/Kelembaban Udara/" + date, data_kelembaban_udara);
    Firebase.updateNode(firebaseData, "Collection Data/Hujan/" + date, data_hujan);
    date_temp = date;
  }
}
