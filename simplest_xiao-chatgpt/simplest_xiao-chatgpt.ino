/*
 * The simplest demo of XIAO ESP32C3 calling OpenAI
*/

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
const char* chatgpt_token = "YOUR_API_KEY";
const char* chatgpt_model = "gpt-6-luna";  // Change this string to use another Chat Completions model
const char* chatgpt_Q = "Who are you";

String jsonEscape(const String &src) {
  String out;
  out.reserve(src.length() + 8);
  for (unsigned int i = 0; i < src.length(); i++) {
    unsigned char c = src[i];
    switch (c) {
      case '\"': out += "\\\""; break;
      case '\\': out += "\\\\"; break;
      case '\b': out += "\\b"; break;
      case '\f': out += "\\f"; break;
      case '\n': out += "\\n"; break;
      case '\r': out += "\\r"; break;
      case '\t': out += "\\t"; break;
      default:
        if (c < 0x20) {
          char buf[7];
          sprintf(buf, "\\u%04x", c);
          out += buf;
        } else {
          out += (char)c;
        }
        break;
    }
  }
  return out;
}

String buildChatCompletionsPayload(const String &question, int maxCompletionTokens) {
  String payload = String("{\"model\": \"") + chatgpt_model +
    String("\", \"messages\": [{\"role\": \"user\", \"content\": \"") + jsonEscape(question) +
    String("\"}], \"reasoning_effort\": \"none\", \"max_completion_tokens\": ") +
    String(maxCompletionTokens) + String("}");
  return payload;
}

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
    if (https.begin("https://api.openai.com/v1/chat/completions")) {  // HTTPS
      https.setTimeout(30000);
      https.addHeader("Content-Type", "application/json");
      String token_key = String("Bearer ") + chatgpt_token;
      https.addHeader("Authorization", token_key);

      String payload = buildChatCompletionsPayload(String(chatgpt_Q), 64);

      Serial.print("[HTTPS] POST...\n");
      Serial.println(payload);

      // start connection and send HTTP header
      int httpCode = https.POST(payload);

      // httpCode will be negative on error
      // file found at server
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = https.getString();
        Serial.println(payload);
      }
      else {
        Serial.printf("[HTTPS] POST... failed, error: %s\n", https.errorToString(httpCode).c_str());
        Serial.println(https.getString());
      }
      https.end();
    }
    else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }

  Serial.println("Wait 10s before next round...");
  delay(10000);
}
