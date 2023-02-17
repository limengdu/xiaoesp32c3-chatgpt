/*
 * The simplest demo of XIAO ESP32C3 calling OpenAI
*/

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "YOUR_SSED";
const char* password = "YOUR_PASSWORD";
const char* chatgpt_token = "YOUR_API_KEY";
const char* chatgpt_Q = "\"Who are you\"";

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  while(!Serial);

  // wait for WiFi connection
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
    HTTPClient https;

    Serial.print("[HTTPS] begin...\n");
    if (https.begin("https://api.openai.com/v1/completions")) {  // HTTPS
      
      https.addHeader("Content-Type", "application/json"); 
      String token_key = String("Bearer ") + chatgpt_token;
      https.addHeader("Authorization", token_key);
      
      String payload = String("{\"model\": \"text-davinci-003\", \"prompt\": ") + chatgpt_Q + String(", \"temperature\": 0, \"max_tokens\": 7}"); //Instead of TEXT as Payload, can be JSON as Paylaod
      
      Serial.print("[HTTPS] GET...\n");
      
      // start connection and send HTTP header
      int httpCode = https.POST(payload);

      // httpCode will be negative on error      
      // file found at server
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = https.getString();
        Serial.println(payload);
      }
      else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }
      https.end();
    }
    else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }

  Serial.println("Wait 10s before next round...");
  delay(10000);
}
