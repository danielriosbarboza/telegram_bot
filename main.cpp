#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "DHT.h"

char ssid[] = "****";       // Nome REDE
char password[] = "**";     // Senha WI-FI
#define BOT_TOKEN "*******" // Token BOT
#define AR 16               // GPIO_16 ->D0
#define UMID 5              // GPIO_5 ->D1
#define DHT_PIN 14          // GPIO_14 ->D5
#define DHTTYPE DHT11
WiFiClientSecure client;
DHT dht(DHT_PIN, DHTTYPE);
UniversalTelegramBot bot(BOT_TOKEN, client);
int Bot_mtbs = 1000;     // tempo entre as mensagens (ms)
long Bot_lasttime;       // tempo do último scanner das mensagens
String myChatID = "***"; // ID Telegram
void setup()
{
  pinMode(AR, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(UMID, OUTPUT);
  // Inicializa conexão Wifi
  WiFi.mode(WIFI_AP_STA);
  WiFi.disconnect();
  delay(100);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) // aguardando a conexão WEB
  {
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
  }
  // Conecta cliente com SSL
  client.setFingerprint("F2:AD:29:9C:34:48:DD:8D:F4:CF:52:32:F6:57:33:68:2E:81:C1:90");
  while (!client.connect("api.telegram.org", 443))
  {
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
  }
  digitalWrite(AR, HIGH);
  digitalWrite(UMID, HIGH);
}
void loop()
{
  if (millis() > Bot_lasttime + Bot_mtbs)
  {
    {
      digitalWrite(LED_BUILTIN, LOW);
      delay(50);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(50);
    }
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages)
    {
      for (int i = 0; i < numNewMessages; i++)
      {
        String chat_id = String(bot.messages[i].chat_id);
        String text = bot.messages[i].text;
        String from_name = bot.messages[i].from_name;
        if (text == "ArOn")
        {
          digitalWrite(AR, LOW);
          bot.sendMessage(chat_id, from_name + " - Ar-Condicionado ligado", "");
        }
        if (text == "UmidificadorOn")
        {
          digitalWrite(UMID, LOW);
          bot.sendMessage(chat_id, from_name + " - Umidificador ligado", "");
        }
        if (text == "ArOff")
        {
          digitalWrite(AR, HIGH);
          bot.sendMessage(chat_id, from_name + " - Ar-Condicionado Desligado", "");
        }
        if (text == "UmidificadorOff")
        {
          digitalWrite(UMID, HIGH);
          bot.sendMessage(chat_id, from_name + " - Umidificador Desligado", "");
        }
        if (text == "Status")
        {
          float humidity = dht.readHumidity();
          float temperature = dht.readTemperature();
          String message = "A temperatura é de " + String(temperature, 2) + " graus celsius.\n";
          message += "A umidade relativa do ar é de " + String(humidity, 2) + "%.\n\n";
          bot.sendMessage(chat_id, message, "Markdown");
        }
      }
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    Bot_lasttime = millis();
  }