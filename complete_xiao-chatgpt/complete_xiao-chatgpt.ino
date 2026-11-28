// Load Wi-Fi library
#include "WiFi.h"
#include <HTTPClient.h>
#include <stdlib.h>

// Replace with your network credentials
const char* ssid     = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
//chatgpt api
const char* chatgpt_token = "YOUR_API_KEY";
const char* chatgpt_model = "gpt-6-luna";  // Change this string to use another Chat Completions model

char chatgpt_server[] = "https://api.openai.com/v1/chat/completions";

const unsigned long CLIENT_TIMEOUT_MS = 8000;
const int MAX_POST_BYTES = 4096;

// Set web server port number to 80
WiFiServer server(80);
WiFiClient client1;

HTTPClient https;

String chatgpt_Q;
String chatgpt_A;
String json_String;
uint16_t dataStart = 0;
uint16_t dataEnd = 0;
int httpCode = 0;

typedef enum
{
  do_webserver_index,
  send_chatgpt_request,
  get_chatgpt_list,
}STATE_;

STATE_ currentState;

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

String urlDecode(const String &src) {
  String out;
  out.reserve(src.length());
  for (unsigned int i = 0; i < src.length(); i++) {
    char c = src[i];
    if (c == '+') {
      out += ' ';
    } else if (c == '%' && (i + 2) < src.length()) {
      char hex[3] = { src[i + 1], src[i + 2], 0 };
      out += (char)strtol(hex, NULL, 16);
      i += 2;
    } else {
      out += c;
    }
  }
  return out;
}

String htmlEscape(const String &src) {
  String out;
  out.reserve(src.length() + 16);
  for (unsigned int i = 0; i < src.length(); i++) {
    char c = src[i];
    switch (c) {
      case '&':  out += "&amp;";  break;
      case '<':  out += "&lt;";   break;
      case '>':  out += "&gt;";   break;
      case '"':  out += "&quot;"; break;
      case '\'': out += "&#39;";  break;
      default:   out += c;        break;
    }
  }
  return out;
}

String readHttpLine(WiFiClient &client) {
  String line;
  unsigned long started = millis();
  while (millis() - started < CLIENT_TIMEOUT_MS) {
    while (client.available()) {
      char c = (char)client.read();
      if (c == '\n') {
        if (line.endsWith("\r")) {
          line.remove(line.length() - 1);
        }
        return line;
      }
      line += c;
    }
    if (!client.connected() && !client.available()) {
      break;
    }
    delay(1);
  }
  return line;
}

String jsonUnescape(const String &src) {
  String out;
  out.reserve(src.length());
  for (unsigned int i = 0; i < src.length(); i++) {
    char c = src[i];
    if (c == '\\' && (i + 1) < src.length()) {
      char n = src[i + 1];
      switch (n) {
        case '"': out += '"'; break;
        case '\\': out += '\\'; break;
        case '/': out += '/'; break;
        case 'b': out += '\b'; break;
        case 'f': out += '\f'; break;
        case 'n': out += '\n'; break;
        case 'r': out += '\r'; break;
        case 't': out += '\t'; break;
        default: out += n; break;
      }
      i++;
    } else {
      out += c;
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

// Chat Completions returns the reply in choices[0].message.content.
String extractAssistantContent(const String &payload) {
  int messagePos = payload.indexOf("\"message\"");
  if (messagePos < 0) {
    return "";
  }

  int contentKey = payload.indexOf("\"content\"", messagePos);
  if (contentKey < 0) {
    return "";
  }

  int colon = payload.indexOf(':', contentKey + 9);
  if (colon < 0) {
    return "";
  }

  int i = colon + 1;
  while (i < (int)payload.length() &&
         (payload[i] == ' ' || payload[i] == '\n' || payload[i] == '\r' || payload[i] == '\t')) {
    i++;
  }
  if (i >= (int)payload.length() || payload[i] != '"') {
    return "";
  }
  i++;

  String raw;
  while (i < (int)payload.length()) {
    char c = payload[i];
    if (c == '\\' && (i + 1) < (int)payload.length()) {
      raw += c;
      raw += payload[i + 1];
      i += 2;
      continue;
    }
    if (c == '"') {
      break;
    }
    raw += c;
    i++;
  }
  return jsonUnescape(raw);
}

void WiFiConnect(void){
    WiFi.begin(ssid, password);
    Serial.print("Connecting to ");
    Serial.println(ssid);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    currentState = do_webserver_index;
}

const char html_page[] PROGMEM = {
    "<!DOCTYPE HTML>\r\n"
    "<html>\r\n"
    "<head>\r\n"
      "<meta charset=\"UTF-8\">\r\n"
      "<title>Cloud Printer: ChatGPT</title>\r\n"
      "<link rel=\"icon\" href=\"https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/chatgpt-logo.png\" type=\"image/x-icon\">\r\n"
    "</head>\r\n"
    "<body>\r\n"
    "<img alt=\"SEEED\" src=\"https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/logo.png\" height=\"100\" width=\"410\">\r\n"
    "<p style=\"text-align:center;\">\r\n"
    "<img alt=\"ChatGPT\" src=\"https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/chatgpt-logo.png\" height=\"200\" width=\"200\">\r\n"
    "<h1 align=\"center\">Cloud Printer</h1>\r\n"
    "<h1 align=\"center\">OpenAI ChatGPT</h1>\r\n"
    "<div style=\"text-align:center;vertical-align:middle;\">"
    "<form action=\"/\" method=\"post\">"
    "<input type=\"text\" placeholder=\"Please enter your question\" size=\"35\" name=\"chatgpttext\" required=\"required\"/>\r\n"
    "<input type=\"submit\" value=\"Submit\" style=\"height:30px; width:80px;\"/>"
    "</form>"
    "</div>"
    "</p>\r\n"
};

void sendHtmlPage(WiFiClient &client, int statusCode, const char *statusText) {
  client.print("HTTP/1.1 ");
  client.print(statusCode);
  client.print(" ");
  client.print(statusText);
  client.print("\r\n");
  client.print("Content-Type: text/html\r\n");
  client.print("Connection: close\r\n");
  client.print("\r\n");
  client.print(html_page);
  if (chatgpt_Q.length() > 0) {
    client.print("<h3 align=\"center\">Question</h3><pre>");
    client.print(htmlEscape(chatgpt_Q));
    client.print("</pre>");
  }
  if (chatgpt_A.length() > 0) {
    client.print("<h3 align=\"center\">Answer</h3><pre>");
    client.print(htmlEscape(chatgpt_A));
    client.print("</pre>");
  }
  client.print("</body>\r\n");
  client.print("<html>\r\n");
}

void setup()
{
    Serial.begin(115200);

    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    while(!Serial);

    Serial.println("WiFi Setup done!");

    WiFiConnect();
    // Start the TCP server server
    server.begin();
}

void loop()
{
  switch(currentState){
    case do_webserver_index:
      Serial.println("Web Production Task Launch");
      client1 = server.available();              // Check if the client is connected
      if (client1){
        Serial.println("New Client.");           // print a message out the serial port
        String requestLine = readHttpLine(client1);
        Serial.println(requestLine);

        int contentLength = 0;
        while (client1.connected()) {
          String header = readHttpLine(client1);
          if (header.length() == 0) {
            break;
          }
          String lower = header;
          lower.toLowerCase();
          if (lower.startsWith("content-length:")) {
            contentLength = header.substring(header.indexOf(':') + 1).toInt();
          }
        }

        if (requestLine.startsWith("GET ")) {
          sendHtmlPage(client1, 200, "OK");
        } else if (requestLine.startsWith("POST ")) {
          if (contentLength <= 0 || contentLength > MAX_POST_BYTES) {
            sendHtmlPage(client1, 413, "Payload Too Large");
          } else {
            json_String = "";
            json_String.reserve(contentLength);
            unsigned long started = millis();
            // TCP may deliver the form body in several packets; wait for Content-Length bytes.
            while ((int)json_String.length() < contentLength && millis() - started < CLIENT_TIMEOUT_MS) {
              while (client1.available() && (int)json_String.length() < contentLength) {
                json_String += (char)client1.read();
              }
              if (!client1.connected() && !client1.available()) {
                break;
              }
              delay(1);
            }
            if ((int)json_String.length() != contentLength) {
              sendHtmlPage(client1, 408, "Request Timeout");
            } else {
              Serial.println(json_String);
              dataStart = json_String.indexOf("chatgpttext=") + strlen("chatgpttext=");
              chatgpt_Q = json_String.substring(dataStart, json_String.length());
              int amp = chatgpt_Q.indexOf('&');
              if (amp >= 0) {
                chatgpt_Q = chatgpt_Q.substring(0, amp);
              }
              chatgpt_Q.trim();
              chatgpt_Q = urlDecode(chatgpt_Q);
              chatgpt_A = "";
              sendHtmlPage(client1, 200, "OK");
              Serial.print("Your Question is: ");
              Serial.println(chatgpt_Q);
              delay(10);
              client1.stop();
              currentState = send_chatgpt_request;
            }
          }
        }
        json_String = "";
        if (client1.connected()) {
          delay(10);
          client1.stop();
        }
      }
      delay(1000);
      break;
    case send_chatgpt_request:
      Serial.println("Ask ChatGPT a Question Task Launch");
      if (https.begin(chatgpt_server)) {  // HTTPS
        https.setTimeout(30000);
        https.addHeader("Content-Type", "application/json");
        String token_key = String("Bearer ") + chatgpt_token;
        https.addHeader("Authorization", token_key);
        String payload = buildChatCompletionsPayload(chatgpt_Q, 256);
        httpCode = https.POST(payload);   // start connection and send HTTP header
        payload = "";
        currentState = get_chatgpt_list;
      }
      else {
        Serial.println("[HTTPS] Unable to connect");
        delay(1000);
      }
      break;
    case get_chatgpt_list:
      Serial.println("Get ChatGPT Answers Task Launch");
      // httpCode will be negative on error
      // file found at server
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = https.getString();
        chatgpt_A = extractAssistantContent(payload);
        if (chatgpt_A.length() == 0) {
          Serial.println("Unable to parse assistant message. Raw response:");
          Serial.println(payload);
        } else {
          Serial.print("ChatGPT Answer is: ");
          Serial.println(chatgpt_A);
        }
        Serial.println("Wait 10s before next round...");
        currentState = do_webserver_index;
      }
      else {
        Serial.printf("[HTTPS] POST... failed, error: %s\n", https.errorToString(httpCode).c_str());
        Serial.println(https.getString());
        while(1);
      }
      https.end();
      delay(10000);
      break;
  }
}
