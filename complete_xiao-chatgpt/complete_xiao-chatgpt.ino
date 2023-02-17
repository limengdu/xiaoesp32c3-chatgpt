// Load Wi-Fi library
#include "WiFi.h"
#include <HTTPClient.h>

// Replace with your network credentials
const char* ssid     = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
//chatgpt api
const char* chatgpt_token = "YOUR_API_KEY";

char chatgpt_server[] = "https://api.openai.com/v1/completions";

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
        https.addHeader("Content-Type", "application/json"); 
        String token_key = String("Bearer ") + chatgpt_token;
        https.addHeader("Authorization", token_key);
        String payload = String("{\"model\": \"text-davinci-003\", \"prompt\": \"") + chatgpt_Q + String("\", \"temperature\": 0, \"max_tokens\": 100}"); //Instead of TEXT as Payload, can be JSON as Paylaod
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
//        Serial.println(payload);
        dataStart = payload.indexOf("\\n\\n") + strlen("\\n\\n");
        dataEnd = payload.indexOf("\",\"", dataStart); 
        chatgpt_A = payload.substring(dataStart, dataEnd);
        Serial.print("ChatGPT Answer is: ");
        Serial.println(chatgpt_A);
        Serial.println("Wait 10s before next round...");
        currentState = do_webserver_index;
      }
      else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        while(1);
      }
      https.end();
      delay(10000);
      break;
  }
}
