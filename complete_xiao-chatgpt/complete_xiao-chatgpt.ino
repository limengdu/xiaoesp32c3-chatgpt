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

// Set web server port number to 80
WiFiServer server(80);
WiFiClient client1;

HTTPClient https;

String chatgpt_Q;
String chatgpt_A;
String json_String;
uint16_t dataStart = 0;
uint16_t dataEnd = 0;
String dataStr;
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
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Connection: close\r\n"  // the connection will be closed after completion of the response
    //"Refresh: 1\r\n"         // refresh the page automatically every n sec
    "\r\n"
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
    "</body>\r\n"
    "<html>\r\n"
};

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
        // an http request ends with a blank line
        boolean currentLineIsBlank = true;
        while (client1.connected()){
          if (client1.available()){
            char c = client1.read();             // Read the rest of the content, used to clear the cache
            json_String += c;
            if (c == '\n' && currentLineIsBlank) {
              dataStr = json_String.substring(0, 4);
              Serial.println(dataStr);
              if(dataStr == "GET "){
                client1.print(html_page);        //Send the response body to the client
              }
              else if(dataStr == "POST"){
                json_String = "";
                while(client1.available()){
                  json_String += (char)client1.read();
                }
                Serial.println(json_String);
                dataStart = json_String.indexOf("chatgpttext=") + strlen("chatgpttext="); // parse the request for the following content
                chatgpt_Q = json_String.substring(dataStart, json_String.length());
                int amp = chatgpt_Q.indexOf('&');
                if (amp >= 0) {
                  chatgpt_Q = chatgpt_Q.substring(0, amp);
                }
                chatgpt_Q.trim();
                chatgpt_Q = urlDecode(chatgpt_Q);
                client1.print(html_page);
                Serial.print("Your Question is: ");
                Serial.println(chatgpt_Q);
                // close the connection:
                delay(10);
                client1.stop();
                currentState = send_chatgpt_request;
              }
              json_String = "";
              break;
            }
            if (c == '\n') {
              // you're starting a new line
              currentLineIsBlank = true;
            }
            else if (c != '\r') {
              // you've gotten a character on the current line
              currentLineIsBlank = false;
            }
          }
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
