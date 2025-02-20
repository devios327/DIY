#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

// Данные для подключения к Wi-Fi
const char *ssid = "";
const char *password = "";

// Настройки GitLab
const char *gitlabToken = "";                            // Токен для доступа к API GitLab
const char *gitlabUrl = ""; // URL для запроса merge requests

// Номер пина, к которому подключен светодиод (GPIO2 на ESP01)
const int ledPin = 2;
// Клиент для безопасного подключения по Wi-Fi
WiFiClientSecure client;

/* Функция для мигания светодиодом*/
void blinkLED(int times, int delayTime)
{
  for (int i = 0; i < times; i++)
  {
    digitalWrite(ledPin, HIGH);
    delay(delayTime);
    digitalWrite(ledPin, LOW);
    delay(delayTime);
  }
}

void setup()
{
  // Инициализация последовательного соединения для отладки
  Serial.begin(9600);
  // Установка пина светодиода как выхода
  pinMode(ledPin, OUTPUT);
  // Изначально гасим светодиод
  digitalWrite(ledPin, LOW);

  blinkLED(1, 500);

  // Подключение к Wi-Fi
  WiFi.begin(ssid, password);
  int attempt = 0;
  while (WiFi.status() != WL_CONNECTED && attempt < 10)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    attempt++;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    // Мигание 3 раза при успешном подключении
    blinkLED(3, 500);
  }
  else
  {
    // Быстрое мигание при ошибке подключения к Wi-Fi
    blinkLED(30, 100);
  }

  // Разрешение небезопасных соединений (для тестирования)
  client.setInsecure();
}

void loop()
{
  // Проверка подключения к Wi-Fi
  if (WiFi.status() == WL_CONNECTED)
  {
    // Создание HTTP-клиента
    HTTPClient http;
    // Ссылка на gitlab c запросом к API
    String url = String(gitlabUrl) + "?scope=assigned_to_me&state=opened&private_token=" + gitlabToken;

    // Использование WiFiClientSecure для HTTPS
    http.begin(client, url);
    // Выполнение GET-запроса
    int httpCode = http.GET();

    Serial.println(httpCode);

    // Если запрос выполнен успешно
    if (httpCode > 0)
    {
      // Получение ответа в виде строки
      String payload = http.getString();
      // Создание объекта для работы с JSON
      JsonDocument doc;
      // Десериализация JSON
      DeserializationError error = deserializeJson(doc, payload);

      if (error)
      {
        // Если ошибка десериализации, выходим из функции
        return;
      }

      // Флаг для отслеживания наличия merge requests без статуса WIP
      bool hasNonWipMergeRequests = false;

      // Перебор всех merge requests
      for (JsonObject mr : doc.as<JsonArray>())
      {
        // Получение заголовка merge request
        const char *title = mr["title"];

        // Проверка, содержится ли в заголовке "WIP"
        if (strstr(title, "WIP") == nullptr)
        {
          // Если не содержится, устанавливаем флаг
          hasNonWipMergeRequests = true;
          break;
        }
      }

      // Зажигаем светодиод, если есть merge requests без статуса WIP
      if (hasNonWipMergeRequests)
      {
        digitalWrite(ledPin, HIGH);
      }
      else
      {
        // Гасим светодиод, если все merge requests в статусе WIP
        digitalWrite(ledPin, LOW);
      }
    }
    else
    {
      // Мигание 10 раз при ошибке выполнения запроса
      blinkLED(10, 1000);
    }

    // Завершение HTTP-запроса
    http.end();
  }

  // Проверка каждые 60 секунд
  delay(60000);
}
