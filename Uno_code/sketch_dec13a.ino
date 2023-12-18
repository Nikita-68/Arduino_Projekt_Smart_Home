#define WIFI_SSID "Xiaomi 11T" 
#define WIFI_PASS "645482705" 
#define BOT_TOKEN "6886663736:AAGxELrECTpczSJprg8_gO1bhSx5IDhNGZA" 
#define CHAT_ID "6955351249" 
#include <FastBot.h> 
#include <CharPlot.h> 
#include <LittleFS.h> 
FastBot bot(BOT_TOKEN); 
bool restart = 0; 
String fileName="/data.txt";//!!!Wicgtig: name nicht lenger als 31 zeichen 
float tempW; 
int n=0; 
void setup()  
{ 
  connectWiFi(); 
  bot.attach(newMsg); 
  bot.setChatID("6955351249"); 
  bot.sendMessage("Telegram Bot connected"); 
  pinMode(LED_BUILTIN, OUTPUT); 
  bot.showMenu("Daten synhronisieren\n on \t off \t state \n restart  \n draw Plot \n Close"); 
  sendData();
  if(LittleFS.begin()) 
  { 
    bot.sendMessage("File System OK",CHAT_ID); 
  } 

} 
// обработчик сообщений  
void newMsg(FB_msg& msg) { 
  // bei diese methode werden alle messages bearbeitet 

  if (msg.text == "on")  
  { 
  bot.sendMessage("LED an",CHAT_ID); // Светодиод включен 
  digitalWrite(LED_BUILTIN, LOW); // HIGH for ESP32 
//digitalWrite(ledPin, ledState); 
  } 
if (msg.text == "off")  
{ 
  bot.sendMessage("LED aus",CHAT_ID); // Светодиод выключен 
  digitalWrite(LED_BUILTIN, HIGH); // LOW for ESP3 
  //digitalWrite(ledPin, ledState); 
  } 
    if (msg.text == "state")  
    { 
      if (digitalRead(LED_BUILTIN)) { 
      bot.sendMessage("LED ist aus",CHAT_ID); // Светодиод включен (ОС) 
    } 
    else  
    { 
      bot.sendMessage("LED ist an",CHAT_ID); // Светодиод выключен (ОС) 
    } 
  } 
  if (msg.text == "restart")  
  { 
    restart = 1; 
  } 
  if (msg.text == "draw Plot")  
  { 
 
    uint8_t sz = 40; 


  // Массив для хранения предопределенных значений, представляющих данные датчика 
    float arr[40] = {23.2, 23.5, 24.1, 24.3, 23.8, 24.0, 24.7, 23.9, 24.6, 25.2, 

                     25.0, 24.4, 25.1, 24.9, 25.3, 25.6, 25.8, 25.4, 26.0, 25.7, 

                     26.2, 26.4, 26.1, 26.8, 26.5, 26.9, 27.0, 26.7, 26.3, 26.6, 

                     25.5, 25.9, 24.8, 23.7, 23.4, 23.1, 23.6, 24.2, 24.5, 25.2}; 

    bot.sendMessage(CharPlot<COLON_X2>(arr, sz, 10,0),CHAT_ID); 
    

  } 

}   

void loop()  
{ 
  bot.tick(); 
  if (restart) { 
    bot.tickManual(); 
    ESP.restart(); 
  } 

 
 

  if (Serial.available() > 0 && n < 4) { 
    File file = LittleFS.open(fileName, "a"); 
    if (file) { 
      String input = Serial.readStringUntil('\n'); 
      int firstValue = input.substring(0, input.indexOf(',')).toInt(); 
      int sValue = input.substring(1, input.indexOf(',')).toInt(); 
      int tValue = input.substring(2, input.indexOf(';')).toInt(); 
      file.println(firstValue); 
      n++; 
      file.close(); 
      Serial.println(firstValue); 
    } else Serial.println("Failed to open file for writing"); 
  } 

} 

void connectWiFi()  
{ 
  delay(2000); 
  Serial.begin(9600); 
  WiFi.begin(WIFI_SSID, WIFI_PASS); 
  while (WiFi.status() != WL_CONNECTED)  
  { 
    delay(500); 
    if (millis() > 15000) ESP.restart(); 
  } 

} 

 
 

void sendData()  

{ 

  FB_Time t = bot.getTime(1); 

  if(t.hour<10) 
  { 
    Serial.print("0"); 
  } 

  Serial.print(t.hour); 

 
  if(t.minute<10) 
  { 
    Serial.print("0"); 
  } 

  Serial.println(t.minute); 
} 
void readAndPrintData() { 
  File file = LittleFS.open(fileName, "r"); 
  if (file) { 
    Serial.println("Data in LittleFS:"); 
    while (file.available()) Serial.println(file.readStringUntil('\n')); 
    file.close(); 
  } else Serial.println("Failed to open file for reading"); 

} 
