#define WIFI_SSID "Xiaomi 11T"
#define WIFI_PASS "645482705"
#define BOT_TOKEN "6886663736:AAGxELrECTpczSJprg8_gO1bhSx5IDhNGZA"
#define CHAT_ID "6955351249"
 
#include <FastBot.h>
#include <CharPlot.h>
#include <LittleFS.h>
 
FastBot bot(BOT_TOKEN);
 
bool restart = 0;
String temperaturFile = "/temp/temperatur.txt";  //!!!Wichtig: name nicht lenger als 31 zeichen
String lichtSt = "/variable/lichtStatus.txt";
 
float arr[40];
float tempTS;
int indexTemp = 0;
int temperaturSD = 23;
 
bool licht;
 
void setup() {
  connectWiFi();
  bot.attach(newMsg);
  //bot.setChatID("6955351249");
  bot.sendMessage("Telegram Bot connected");
  pinMode(LED_BUILTIN, OUTPUT);
  bot.showMenu("state \n Licht an \t Licht aus \n Temperatur zeigen \n Edit Temperatur \n on \t off \n restart");
  if (LittleFS.begin()) {
    bot.sendMessage("File System OK", CHAT_ID);
  }
 
 
 
  sendData();
}
 
// обработчик сообщений
 
 
 
void newMsg(FB_msg& msg) {
 
 
 
  if (msg.text == "Edit Temperatur") {
    bot.sendMessage("Aktuelle Temperatur: " + String(temperaturSD), CHAT_ID);
 
    bot.inlineMenu("Temperatur anpassen: ", "plus 1 \t minus 1",CHAT_ID);
  }
 
  if (msg.text == "Licht an") {
    File temp = LittleFS.open(lichtSt, "a");
    temp.truncate(0);
    temp.print("1,");
    bot.sendMessage("Licht wird angemacht", CHAT_ID);
 
 
    sendData();
    temp.close();
  }
  if (msg.text == "Licht aus") {
    File temp = LittleFS.open(lichtSt, "a");
    temp.truncate(0);
    temp.print("0,");
 
 
    bot.sendMessage("Licht wird ausgemacht", CHAT_ID);
    sendData();
    temp.close();
  }
 
  if (msg.data == "plus 1") {
 
    temperaturSD = temperaturSD + 1;
    bot.sendMessage("Aktuelle Temperatur: " + String(temperaturSD), CHAT_ID);
    sendData();
  }
  if (msg.data == "minus 1") {
    temperaturSD = temperaturSD - 1;
    bot.sendMessage("Aktuelle Temperatur: " + String(temperaturSD), CHAT_ID);
    sendData();
  }
 
  if (msg.text == "on") {
    bot.sendMessage("LED an", CHAT_ID);  // Светодиод включен
    digitalWrite(LED_BUILTIN, LOW);      // HIGH for ESP32
                                         //digitalWrite(ledPin, ledState);
  }
 
  if (msg.text == "off") {
    bot.sendMessage("LED aus", CHAT_ID);  // Светодиод выключен
    digitalWrite(LED_BUILTIN, HIGH);      // LOW for ESP3
    //digitalWrite(ledPin, ledState);
  }
  if (msg.text == "state") {
    if (digitalRead(LED_BUILTIN)) {
      bot.sendMessage("LED ist aus", CHAT_ID);  // Светодиод включен (ОС)
    } else {
      bot.sendMessage("LED ist an", CHAT_ID);  // Светодиод выключен (ОС)
    }
    File temp = LittleFS.open(lichtSt, "r");
    String wert = temp.readString();
    bot.sendMessage("Status" + wert, CHAT_ID);
    if (wert == "1,") {
      bot.sendMessage("Licht ist an", CHAT_ID);
    } else {
      bot.sendMessage("Licht ist aus", CHAT_ID);
    }
    temp.close();
 
    bot.sendMessage("Aktuelle Temperatur ist " + String(arr[indexTemp - 1]), CHAT_ID);
  }
  if (msg.text == "restart") {
    //.answer("ESP wird restartet", true);
    restart = 1;
  }
  if (msg.text == "Temperatur zeigen") {
    uint8_t sz = 40;
 
 
    bot.sendMessage(CharPlot<COLON_X2>(arr, sz, 10, 0), CHAT_ID);
  }
}
 
 
 
 
void loop() {
  bot.tick();
  if (restart) {
    bot.tickManual();
    ESP.restart();
  }
  readDaten();
}
 
void connectWiFi() {
  delay(2000);
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if (millis() > 15000) ESP.restart();
  }
}
 
void sendData() {
  FB_Time t = bot.getTime(1);
 
  Serial.print("111,");
  Serial.print(t.hour);
  Serial.print(",");
  Serial.print(t.minute);
  Serial.print(",");
 
  File temp = LittleFS.open(lichtSt, "r");
  String wert = temp.readString();
  Serial.print(wert);
  temp.close();
 
 
  Serial.println(temperaturSD);
}
void readDaten() {
  if (Serial.available()) {
 
    char str[20];
    int temp = Serial.readBytesUntil(';', str, 20);
    str[temp] = '\0';
    char* token = strtok(str, ",");
    int count = 0;
    int daten[3];  // Теперь ожидаем три значения: час, минута, флаг для светодиода
 
    while (token != NULL && count < 3) {
      daten[count++] = atof(token);
      token = strtok(NULL, ",");
    }
 
    if (daten[0] == 222) {
      if (indexTemp > 39) {
        for (int i = 0; i < 40; i++) {
          arr[i] = arr[i++];
        }
        daten[1] = 39;
      } else {
        arr[indexTemp] = daten[1];
        indexTemp++;
      }
      if (daten[2] == 1) {
        bot.sendMessage("Es wurde falsche Schlussel benutzt.", CHAT_ID);
      }
      
    }
  }
}
 