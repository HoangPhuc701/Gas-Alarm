// Fill-in information from your Blynk Template here
#define BLYNK_TEMPLATE_ID "TMPL6G-vTWJ2k"
#define BLYNK_DEVICE_NAME "Báo Khí Gas"
#define BLYNK_FIRMWARE_VERSION "1.2.0"
#define BLYNK_PRINT Serial
#define APP_DEBUG
#define USE_NODE_MCU_BOARD
#include "BlynkEdgent.h"

11

//nhiet do
#include <OneWire.h>
#include <DallasTemperature.h>
unsigned int t;
#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
//XUAT excel
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
const char* host ="script.google.com";
const int httpsPort = 443;
String GAS_ID =
"AKfycbyHNcYbP1wl9p5jdjY0CiN6eF0ecJMD6f6j_mNtFPpD3FrV2yp2qMfikP_0m
QrBYSw8CA";
const char* ssid = "HIU1";
const char* password = "HiU@2020";
WiFiClientSecure client;
int buzzer=5; //D1
int mucCanhbao=500;
int canhbaonhiet=100;
BlynkTimer timer;
int timerID1,timerID2;
int mq2_value;
int button=0; //D3
boolean buttonState=HIGH;
boolean runMode=1;//Bật/tắt chế độ cảnh báo
boolean canhbaoState=0;
WidgetLED led(V0);
double nhiet;
void setup()
{
Serial.begin(115200);
delay(100);
pinMode(button,INPUT_PULLUP);
pinMode(buzzer,OUTPUT);
digitalWrite(buzzer,LOW); //Tắt buzzer
BlynkEdgent.begin();
timerID1 = timer.setInterval(1000L,handleTimerID1);
timerID2 = timer.setInterval(60000L, handleTimerID2);//thêm định kỳ gì á
//do nhiet do
sensors.begin();

12

//excel
WiFi.begin(ssid,password);
Serial.print("Connecting...");
while (WiFi.status() != WL_CONNECTED) {
Serial.print(".");
delay(500);
}
Serial.println("");
Serial.print("Ket noi thanh cong wifi: ");
Serial.println(ssid);
Serial.print("IP address: ");
Serial.println(WiFi.localIP());
Serial.println();
client.setInsecure();

}
void loop()
{
BlynkEdgent.run();
timer.run();
//sendDataToExcel();
if(digitalRead(button)==LOW){
if(buttonState==HIGH){
buttonState=LOW;
runMode=!runMode;
Serial.println("Run mode: " + String(runMode));
Blynk.virtualWrite(V4,runMode);
delay(200);
}
}else{
buttonState=HIGH;
}
}
void handleTimerID1(){
mq2_value = analogRead(A0);
nhiet = sensors.getTempCByIndex(0);
Blynk.virtualWrite(V1,mq2_value);
Blynk.virtualWrite(V5,nhiet);
//sendDataToExcel();
if(led.getValue()) {
led.off();
} else {
led.on();
}

13

if(runMode==1){
if(mq2_value>mucCanhbao||nhiet>canhbaonhiet){
if(canhbaoState==0){
canhbaoState=1;
Blynk.logEvent("canhbao", String("Khí gas =" + String(mq2_value) + ", Nhiệt độ
= " + String(nhiet) +" có nguy cơ cháy nổ"));
timerID2 = timer.setTimeout(60000L,handleTimerID2);
}
digitalWrite(buzzer,HIGH);
Blynk.virtualWrite(V3,HIGH);
Serial.println("Đã bật cảnh báo!");
}
else{
digitalWrite(buzzer,LOW);
Blynk.virtualWrite(V3,LOW);
Serial.println("Đã tắt cảnh báo!");
}

}else{
digitalWrite(buzzer,LOW);
Blynk.virtualWrite(V3,LOW);
Serial.println("Đã tắt cảnh báo!");
}
//nhiet do
if(millis() - t >1000)
{
Serial.println("Nhiệt độ: ");
sensors.requestTemperatures(); // yêu cầu thông tin từ cảm biến
float nhiet = sensors.getTempCByIndex(0);
Serial.println(sensors.getTempCByIndex(0)); // lấy giá trị nhiệt độ từ cảm biến
t = millis();
}
}
void handleTimerID2(){
canhbaoState=0;
}
BLYNK_CONNECTED() {
Blynk.syncVirtual(V2,V4);
}
BLYNK_WRITE(V2) {
mucCanhbao = param.asInt();
}
BLYNK_WRITE(V4) {
runMode = param.asInt();

14

}
BLYNK_WRITE(V6) {
canhbaonhiet = param.asInt();
}
void sendata(int gas, float nhiet) {
if (client.connect(host, httpsPort)) {
String url = "/macros/s/" + GAS_ID + "/exec?nongdo=" + String(gas) +
"&nhietdo=" + String(nhiet);
Serial.println(url);
client.print(String("GET ") + url + " HTTP/1.1\r\n" +
"Host: " + host + "\r\n" +
"User-Agent: BuildFailureDetectorESP8266\r\n" +
"Connection: close\r\n\r\n");
Serial.println("Data sent to Google Sheets");
} else {
Serial.println("Failed to connect to Google Sheets");
}
}
void sendDataToExcel() {
int mq2_value = analogRead(A0);
float nhiet = sensors.getTempCByIndex(0);
sendata(mq2_value, nhiet);
}