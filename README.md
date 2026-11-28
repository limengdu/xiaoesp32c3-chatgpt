# Learn to use WiFiClient and HTTPClient on XIAO ESP32C3 - XIAO ESP32C3 & ChatGPT in action

<div align=center><img width = 1000 src="https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/18.png"/></div>

ChatGPT is a new chatbot model, an artificial intelligence technology-powered natural language processing tool, released by OpenAI, an artificial intelligence research lab, on November 30, 2022.

It is able to carry out conversations by learning and understanding human language, and can also interact based on the context of the chat, truly chatting and communicating like a human, even performing tasks such as writing emails, video scripts, copywriting, translating, and coding.

<div align=center><img width = 800 src="https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/2.png"/></div>

In embedded systems, ChatGPT can be a good helper to assist us in writing simple programs, or even checking and fixing bugs that appear in the program.

What is exciting is that OpenAI officially provides a Chat Completions API, which allows us to call current ChatGPT models and deploy them into our own embedded systems through a number of methods.

Seeed Studio XIAO ESP32C3 is an IoT mini development board based on the Espressif ESP32-C3 WiFi/Bluetooth dual-mode chip. It has excellent radio frequency performance, supporting IEEE 802.11 b/g/n WiFi, and Bluetooth 5 (LE) protocols. It can perfectly support the services of WiFi Client and WiFi Server provided by ESP32 official. Of course, it is able to support Arduino perfectly.

<div align=center><img width = 200 src="https://files.seeedstudio.com/wiki/XIAO_WiFi/board-pic.png"/></div>

<p style=":center"><a href="https://www.seeedstudio.com/seeed-xiao-esp32c3-p-5431.html" target="_blank"><img src="https://files.seeedstudio.com/wiki/Seeed-WiKi/docs/images/get_one_now.png" /></a></p>


So in this tutorial, we will guide users to learn and use XIAO ESP32C3 WiFiClient and HTTPClient libraries, how to connect to the network, how to publish web pages and the basics of HTTP GET and POST. The goal is to call OpenAI ChatGPT and create your own Q&A website.

## Getting Started

In this tutorial, we will use an XIAO ESP32C3 to configure a ChatGPT Q&A page of our own. In this page, you can enter your question and XIAO ESP32C3 will record your question and use the API call method provided by OpenAI to send a request command using HTTP Client to get ChatGPT's answer and print it in the serial port.

<div align=center><img width = 800 src="https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/19.png"/></div>

The tasks in this tutorial can be divided into the following four main steps.

1. [Configure the XIAO ESP32C3 to connect to the network](#configure-the-xiao-esp32c3-to-connect-to-the-network) : Inside this step, we will learn the basic WiFi configuration process using XIAO ESP32C3 and learn the basic operations of XIAO ESP32C3 such as network configuration, connecting to network services and obtaining IP address.

2. [Build the embedded web page](#build-the-embedded-web-page) : In this step, we mainly touch on the WiFi Client library. By using the GET and POST functions of this library, we can write our own Q&A web page using HTML and deploy it on top of XIAO ESP32C3.

3. [Submit questions via the built-in web page](#submit-questions-via-the-built-in-web-page) : In this step we will mainly learn to use the POST method in the HTTP Client to POST the questions we ask according to the OpenAI API standard. we will focus our main attention on the process of how to collect and store the questions from the web page.

4. [Get answers from ChatGPT](#get-answers-from-chatgpt) : In this step we learn to use the POST method in the HTTP Client and extract the answers to the questions we need from the returned messages. The last step is to sort out the structure of the code and make the final integration.

### Materials Required

<table align="center">
	<tr>
	    <th>Material</th>
	</tr>
    <tr>
	    <td align="center"><div align=center><img width = 130 src="https://files.seeedstudio.com/wiki/XIAO_WiFi/board-pic.png"/></div></td>
	</tr>
	<tr>
	    <td align="center"><a href="https://www.seeedstudio.com/seeed-xiao-esp32c3-p-5431.html"><strong>Get ONE Now</strong></a></td>
	</tr>
</table>


### Preliminary Preparation

All procedures and steps in this tutorial have been completed based on the XIAO ESP32C3. In the preparation phase, we first need to complete the configuration of the environment for using the XIAO ESP32C3.

**Step 1.** Connect XIAO ESP32C3 to your computer via a USB Type-C cable.

<div align=center><img src="https://files.seeedstudio.com/wiki/XIAO_WiFi/cable-connect.png" alt="pir" width="120" height="auto"></div>

**Step 2.** Download and Install the latest version of Arduino IDE according to your operating system

<p style="text-align:center;"><a href="https://www.arduino.cc/en/software"><img src="https://files.seeedstudio.com/wiki/Seeeduino_Stalker_V3_1/images/Download_IDE.png" alt="pir" width="600" height="auto"></a></p>

**Step 3.** Launch the Arduino application.

<div align=center><img width = 600 src="https://files.seeedstudio.com/wiki/seeed_logo/arduino.jpg"/></div>

- **Step 4.** Add ESP32 board package to your Arduino IDE

Navigate to **File > Preferences**, and fill **"Additional Boards Manager URLs"** with the url below:
*https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_dev_index.json*

<div align=center><img width = 600 src="https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/4.png"/></div>

Navigate to **Tools > Board > Boards Manager...**, type the keyword "**esp32**" in the search box, select the latest version of ****esp32****, and install it.

<div align=center><img src="https://files.seeedstudio.com/wiki/XIAO_WiFi/add_esp32c3.png" alt="pir" width="650" height="auto"></div>

- **Step 5.** Select your board and port

Navigate to **Tools > Board > ESP32 Arduino** and select "**XIAO_ESP32C3**". The list of board is a little long and you need to roll to the buttom to reach it.

<div align=center><img src="https://files.seeedstudio.com/wiki/Seeed-Studio-XIAO-ESP32/XIAO_ESP32_board.png" alt="pir" width="800" height="auto"></div>

Navigate to **Tools > Port** and select the serial port name of the connected XIAO ESP32C3. This is likely to be COM3 or higher (**COM1** and **COM2** are usually reserved for hardware serial ports).

## Configure the XIAO ESP32C3 to connect to the network
The use of WiFi has been described in detail in the [XIAO ESP32C3 tutorial on using WiFi](https://wiki.seeedstudio.com/XIAO_ESP32C3_WiFi_Usage/#connect-to-a-wifi-network).

When the ESP32 is set as a Wi-Fi station, it can connect to other networks (like your router). In this scenario, the router assigns a unique IP address to your ESP board.

The first thing you need to do to use the ESP32 Wi-Fi functionalities is to include the WiFi.h library in your code, as follows:

```c
#include <WiFi.h>
```

To connect the ESP32 to a specific Wi-Fi network, you must know its SSID and password. Additionally, that network must be within the ESP32 Wi-Fi range.

First, set the Wi-Fi mode. If the ESP32 will connected to another network (access point/hotspot) it must be in station mode.

```c
WiFi.mode(WIFI_STA);
```

Then, use `WiFi.begin()` to connect to a network. You must pass as arguments the network SSID and its password.

Connecting to a Wi-Fi network can take a while, so we usually add a while loop that keeps checking if the connection was already established by using `WiFi.status()`. When the connection is successfully established, it returns `WL_CONNECTED`.

When the ESP32 is set as a Wi-Fi station, it can connect to other networks (like your router). In this scenario, the router assigns a unique IP address to your ESP32 board. To get your board’s IP address, you need to call `WiFi.localIP()` after establishing a connection with your network.

```c
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
}
```

The `ssid` and `password` variables hold the SSID and password of the network you want to connect to.

```c
// Replace with your network credentials
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";
```

This is a very simple WiFi connection program, upload the program to XIAO ESP32C3, then open the serial assistant and set the baud rate to 115200. If the connection goes well, you will see the IP address of XIAO printed out.

<div align=center><img width = 600 src="https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/5.png"/></div>


If you are interested in reading more about ESP32C3 applications and functions in WiFi, we recommend reading <a href="https://randomnerdtutorials.com/esp32-useful-wi- fi-functions-arduino/">ESP32 Useful Wi-Fi Library Functions</a>.

## Build the embedded web page

ESP32 integrates many very useful WiFiClient functions in WiFi library, which allows us to design and develop embedded web pages without adding additional libraries.

Create a new WiFiServer object in order to use this object to control the XIAO ESP32C3 established IoT server.

```c
WiFiServer server(80);
WiFiClient client1;
```

In the above step, we let the XIAO ESP32C3 connect to WiFi. after the WiFi connection is successful, you will be able to get the current IP address of XIAO from the serial monitor. At this time, XIAO has successfully set up the web server. You can access this web server through the IP address of XIAO.

Suppose the IP address of your XIAO ESP32C3 is `192.168.7.152`. Then you can enter this IP address through your browser next.

After entering this IP address, we may only see a blank page. This is because we have not yet published our page content for that page.

<div align=center><img width = 500 src="https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/6.png"/></div>

So let's now create an array to store the content of the page we want to layout, which is the HTML code, in C.

The sketch sends the HTTP status line separately (`200`, `408`, or `413`) and then this HTML. Question and answer text are appended with HTML escaping.

```c
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
    "</body>\r\n"
    "<html>\r\n"
};
```

This code gives us the page effect shown in the figure below.

<div align=center><img width = 800 src="https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/7.png"/></div>

!!!Tip
    HTML syntax for web pages is beyond the scope of this tutorial. You can learn to use HTML on your own, or, alternatively, we can use existing generation tools to do the code generation work. We recommend using [HTML Generator](https://webcode.tools/generators/html).
    It is worth noting that in C programs, "\\" and """ are special characters, and if you want to retain the functionality of these special characters in your program, you need to add a right slash in front of them.

Client1 refers to the Socket client after the web server is established, the following code is the flow of web server processing.

The request line and headers are read line by line until the blank line that ends the headers. `Content-Length` is parsed so the POST body can be collected in full when TCP delivers the form in several packets.

```c
client1 = server.available();
if (client1){
    Serial.println("New Client.");
    String requestLine = readHttpLine(client1);
    Serial.println(requestLine);

    int contentLength = 0;
    while (client1.connected()){
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

    if (requestLine.startsWith("GET ")){
        sendHtmlPage(client1, 200, "OK");
    }
    else if (requestLine.startsWith("POST ")){
        if (contentLength <= 0 || contentLength > MAX_POST_BYTES) {
            sendHtmlPage(client1, 413, "Payload Too Large");
        } else {
            json_String = "";
            json_String.reserve(contentLength);
            unsigned long started = millis();
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
                sendHtmlPage(client1, 200, "OK");
                delay(10);
                client1.stop();
            }
        }
    }
}
```

In the example program above, we need to use `server.begin()` to start the IoT server. The statement needs to be placed in the `setup` function.

```c
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
```

Once the above program is run and the IP address of the XIAO ESP32C3 is entered into the browser (provided that your host also needs to be on the same LAN as the XIAO ESP32C3), then the GET step of WiFiClient will start to execute. At this point we send the page HTML, including any previous question and answer with HTML escaping.

```c
if (requestLine.startsWith("GET ")){
    sendHtmlPage(client1, 200, "OK");
}
```

And, we design the input box for question input in the page, the web page will get the status of the button and store the input question into the string variable `chatgpt_Q` after the user enters the content and clicks the **Submit** button.

The POST body is read until exactly `Content-Length` bytes have arrived (with an 8-second timeout). Question and answer text are HTML-escaped before they are written back into the page.

```c
json_String = "";
json_String.reserve(contentLength);
unsigned long started = millis();
while ((int)json_String.length() < contentLength && millis() - started < CLIENT_TIMEOUT_MS) {
    while (client1.available() && (int)json_String.length() < contentLength) {
        json_String += (char)client1.read();
    }
    if (!client1.connected() && !client1.available()) {
        break;
    }
    delay(1);
}
Serial.println(json_String); 
dataStart = json_String.indexOf("chatgpttext=") + strlen("chatgpttext=");
chatgpt_Q = json_String.substring(dataStart, json_String.length());
chatgpt_Q.trim();
chatgpt_Q = urlDecode(chatgpt_Q);
sendHtmlPage(client1, 200, "OK");
// close the connection:
delay(10);
client1.stop();      
```

The running effect is shown below.

<div align=center><img width = 800 src="https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/15.png"/></div>

## Submit questions via the built-in web page

In the page in the step above, there is an input box. The input box is where we need the user to enter the question they want to ask. All we need to do is to get this question and send it out through the API request provided by OpenAI.

**Step 1**. Sign up for an OpenAI account.

You can go to OpenAI's registration address by clicking [here](https://beta.openai.com/signup). If you have registered for its account before, then you can skip this step.

<div align=center><img width = 400 src="https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/8.png"/></div>

**Step 2**. Get the OpenAI API.

Login to the [OpenAI website](https://platform.openai.com/overview), click your account avatar in the upper right corner, and then select **View API keys**.

<div align=center><img width = 800 src="https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/9.png"/></div>

In the new pop-up page select **+Create new secret key**, then copy your key and save it.

<div align=center><img width = 800 src="https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/10.png"/></div>

In the meantime, we can create string variables in our program and copy this key here.

```c
char chatgpt_token[] = "sk**********Rj9DYiXLJJH";
```

!!!Tip
    As of February 15, 2023, OpenAI is giving away **$18** worth of credits for free to every new user. The detailed rates can be found in OpenAI's [documentation](https://openai.com/api/pricing/).
    <div align=center><img width = 800 src="https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/11.png"/></div>

**Step 3**. Write the program according to the HTTP requests of OpenAI.

OpenAI provides very detailed [API usage instructions](https://developers.openai.com/api/docs/api-reference/chat/create) so that users can use their own API key to call ChatGPT.

Current ChatGPT models use the Chat Completions API. These examples default to [`gpt-6-luna`](https://developers.openai.com/api/docs/models/gpt-6-luna), an inexpensive model that fits an ESP32C3 Q&A demo. Change the `chatgpt_model` string in the sketch if you want another Chat Completions model.

According to the documentation of ChatGPT, the format we need to send the request is as follows:

```shell
curl https://api.openai.com/v1/chat/completions \
-H "Content-Type: application/json" \
-H "Authorization: Bearer YOUR_API_KEY" \
-d '{"model": "gpt-6-luna", "messages": [{"role": "user", "content": "Say this is a test"}], "reasoning_effort": "none", "max_completion_tokens": 64}'
```

`reasoning_effort` set to `none` keeps the on-device demo fast and inexpensive. Chat Completions uses a `messages` array rather than `prompt`, and `max_completion_tokens` is the current token-limit field.

The Hypertext Transfer Protocol (HTTP) works as a request-response protocol between a client and server.
**GET** is used to request data from a specified resource. It is often used to get values from APIs.
**POST** is used to send data to a server to create/update a resource.
ESP32 can make HTTP POST requests using three different types of body requests: URL encoded, JSON object or plain text. These are the most common methods and should integrate with most APIs or web services.

The above information is very important and provides the theoretical basis for writing HTTP POST programs. At the beginning, we start by importing the HTTPClient library.

```c
#include <HTTPClient.h>
```

You also need to type OpenAI domain name, so the ESP publishes the question to ChatGPT. And don't forget the OpenAI API key.

```c
HTTPClient https;

const char* chatgpt_token = "YOUR_API_KEY";
const char* chatgpt_model = "gpt-6-luna";
char chatgpt_server[] = "https://api.openai.com/v1/chat/completions";
```

We need to make an HTTP POST request using JSON object. The question from the web form is URL-decoded, then JSON-escaped, so quotes and line breaks stay valid inside the `messages` payload.

```c
if (https.begin(chatgpt_server)) {  // HTTPS
    https.setTimeout(30000);
    https.addHeader("Content-Type", "application/json"); 
    String token_key = String("Bearer ") + chatgpt_token;
    https.addHeader("Authorization", token_key);
    String payload = buildChatCompletionsPayload(chatgpt_Q, 256);
    httpCode = https.POST(payload);   // start connection and send HTTP header
    payload = "";
}
else {
    Serial.println("[HTTPS] Unable to connect");
    delay(1000);
}
```

In the program, we send the `payload` to the server via the `POST()` method. `chatgpt_Q` is the content of the question we want to send to ChatGPT, which will be available in the Get Question page.

If you are interested in more features of the ESP32C3 HTTPClient, we recommend you to read [ESP32 HTTP GET and HTTP POST with Arduino IDE](https://randomnerdtutorials.com/esp32-http-get-post-arduino/).


## Get answers from ChatGPT

The next step is the last step of the whole tutorial, how we get the answer to ChatGPT and record it.

Let's continue from reading the [API documentation](https://developers.openai.com/api/docs/api-reference/chat/create) provided by OpenAI to understand how the structure of the message content returned by ChatGPT looks like. This will allow us to write programs to parse the content we need.

```shell
{
  "id": "chatcmpl-B9MBs8CjcvOU2jLn4n570S5qMJKcT",
  "object": "chat.completion",
  "created": 1741569952,
  "model": "gpt-6-luna",
  "choices": [
    {
      "index": 0,
      "message": {
        "role": "assistant",
        "content": "This is indeed a test",
        "refusal": null,
        "annotations": []
      },
      "logprobs": null,
      "finish_reason": "stop"
    }
  ],
  "usage": {
    "prompt_tokens": 12,
    "completion_tokens": 6,
    "total_tokens": 18
  }
}

```

From the reference documentation provided by OpenAI, we know that the location of the answer to the question in the message returned by the interface is in `choices[0].message.content`.

The complete example finds the `"content"` string after `"message"` and unescapes JSON so the serial monitor shows the assistant text.

```c
chatgpt_A = extractAssistantContent(payload);
```

In summary, we can use the switch method with the current state of the program to determine which step of the program we should execute.

```c
typedef enum 
{
  do_webserver_index,
  send_chatgpt_request,
  get_chatgpt_list,
}STATE_;

STATE_ currentState;

switch(currentState){
    case do_webserver_index:
        ...
    case send_chatgpt_request:
        ...
    case get_chatgpt_list:
        ...
}
```

At this point, the entire program's logic is structured. The complete program code can be obtained by clicking the image below. Please don't rush to upload the program, you need to change the **ssid, password and chatgpt_token** of the program to your own.

<p style=":center"><a href="https://github.com/limengdu/xiaoesp32c3-chatgpt" target="_blank"><div align=center><img width = 300 src="https://files.seeedstudio.com/wiki/seeed_logo/github.png" /></div></a></p>

Then, feel free to use it!

<div align=center><img width = 800 src="https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/16.gif"/></div>

## What's Next?

In this tutorial, we provide the basic method of calling OpenAI interface using ChatGPT for an embedded development board like Arduino - XIAO ESP32C3. Next, you can let your creativity run wild!

For example, could you consider adding a screen or keyboard to make him a stand-alone display device that works only for you? Come see Gavin's creativity, he made a particularly interesting surveillance device! We also give special thanks to him for providing the necessary steps and ideas for this tutorial.

- [Gavin - ChatGPT Recorder & Monitor](https://www.hackster.io/gavinchiong/chatgpt-recorder-monitor-601ef6)

<div align=center><img width = 600 src="https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/14.jpg"/></div>

Or go one step further and add a voice recognition module to get rid of the keyboard and mouse from now on and make a voice assistant of your own, etc. All in all, we'd love to see you share your work with a great product like the XIAO ESP32C3!

## Troubleshooting

### Q1: Is there any limitation on the use of geography or network to get the answer by calling the OpenAI API using XIAO ESP32C3?

> A: As of February 17, 2023 testing, authors in mainland China and using China's network are also able to get ChatGPT responses very smoothly, with no restrictions for now. As long as we can get the OpenAI API key, the call will be completed smoothly.

## Tech Support

Please do not hesitate to submit the issue into our [forum](https://forum.seeedstudio.com/).

<br /><p style="text-align:center"><a href="https://www.seeedstudio.com/act-4.html?utm_source=wiki&utm_medium=wikibanner&utm_campaign=newproducts" target="_blank"><img src="https://files.seeedstudio.com/wiki/Wiki_Banner/new_product.jpg" /></a></p>
