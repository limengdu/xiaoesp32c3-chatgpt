# Deploying ChatGPT on the Smallest Embedded Development Board - XIAO ESP32C3 with ChatGPT

<div align=center><img width = 800 src="https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/1.png"/></div>


ChatGPT is a new chatbot model, an artificial intelligence technology-powered natural language processing tool, released by OpenAI, an artificial intelligence research lab, on November 30, 2022.

It is able to carry out conversations by learning and understanding human language, and can also interact based on the context of the chat, truly chatting and communicating like a human, even performing tasks such as writing emails, video scripts, copywriting, translating, and coding.

<div align=center><img width = 800 src="https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/2.png"/></div>

In embedded systems, ChatGPT can be a good helper to assist us in writing simple programs, or even checking and fixing bugs that appear in the program.

What is exciting is that OpenAI officially provides interfaces to call GPT-3 models, which allows us to call these interfaces and deploy this great model into our own embedded systems through a number of methods.

Seeed Studio XIAO ESP32C3 is an IoT mini development board based on the Espressif ESP32-C3 WiFi/Bluetooth dual-mode chip. It has excellent radio frequency performance, supporting IEEE 802.11 b/g/n WiFi, and Bluetooth 5 (LE) protocols. It can perfectly support the services of WiFi Client and WiFi Server provided by ESP32 official.

<div align=center><img width = 200 src="https://files.seeedstudio.com/wiki/XIAO_WiFi/board-pic.png"/></div>

<p style=":center"><a href="https://www.seeedstudio.com/seeed-xiao-esp32c3-p-5431.html" target="_blank"><img src="https://files.seeedstudio.com/wiki/Seeed-WiKi/docs/images/get_one_now.png" /></a></p>

So in this tutorial, we will explain in detail how to use WiFi Client, deploy a website of our own, and ask the questions you want to ask ChatGPT on that website to get the answers you want.

## Getting Started

In this tutorial, we will use an XIAO ESP32C3 to configure a ChatGPT question page of our own. In this page, you can enter your question and XIAO ESP32C3 will record your question and use the API call method provided by OpenAI to send a request command using WiFi Client to get ChatGPT's answer and print it in the serial port.

<div align=center><img width = 500 src="https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/3.png"/></div>

The tasks in this tutorial can be divided into the following four main steps.

1. [Configure the XIAO ESP32C3 to connect to the network](#configure-the-xiao-esp32c3-to-connect-to-the-network)
2. [Build the embedded web page](#build-the-embedded-web-page)
3. [Submit questions via the built-in web page](#submit-questions-via-the-built-in-web-page)
4. [Get answers from ChatGPT](#get-answers-from-chatgpt)

### Materials Required

|              |
|:--------------:|
|<div align=center><img width = 130 src="https://files.seeedstudio.com/wiki/XIAO_WiFi/board-pic.png"/></div>|
|[**Get ONE Now**](https://www.seeedstudio.com/seeed-xiao-esp32c3-p-5431.html)|

### Preliminary Preparation

In the prep step, you will follow our steps to configure the Arduino IDE environment for the XIAO ESP32C3.

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

## Build the embedded web page

The Hypertext Transfer Protocol (HTTP) works as a request-response protocol between a client and server.

**GET** is used to request data from a specified resource. It is often used to get values from APIs.

**POST** is used to send data to a server to create/update a resource.

Create a new WiFiServer object in order to use this object to control the XIAO ESP32C3 established IoT server.

In the above step, we let the XIAO ESP32C3 connect to WiFi. after the WiFi connection is successful, you will be able to get the current IP address of XIAO from the serial monitor. At this time, XIAO has successfully set up the web server. You can access this web server through the IP address of XIAO.

Suppose the IP address of your XIAO ESP32C3 is `192.168.7.152`. Then you can enter this IP address through your browser next.

```c
WiFiServer server(80);
WiFiClient client1;
```

After entering this IP address, we may only see a blank page. This is because we have not yet published our page content for that page.

<div align=center><img width = 500 src="https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/6.png"/></div>

So let's now create an array to store the content of the page we want to layout, which is the HTML code, in C.

```c
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
```

This code gives us the page effect shown in the figure below.

<div align=center><img width = 800 src="https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/7.png"/></div>

Client1 refers to the Socket client after the web server is established, the following code is the flow of web server processing.

```c
client1 = server.available();
if (client1){
    Serial.println("New Client.");           // print a message out the serial port
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;    
    while (client1.connected()){
        if (client1.available()){  // Check if the client is connected
            char c = client1.read();
            json_String += c;
            if (c == '\n' && currentLineIsBlank) {                                 
                dataStr = json_String.substring(0, 4);
                Serial.println(dataStr);
                if(dataStr == "GET "){
                    client1.print(html_page);  //Send the response body to the client
                }         
                else if(dataStr == "POST"){
                    json_String = "";
                    while(client1.available()){
                        json_String += (char)client1.read();
                    }
                    Serial.println(json_String); 
                    dataStart = json_String.indexOf("chatgpttext=") + strlen("chatgpttext=");
                    chatgpt_Q = json_String.substring(dataStart, json_String.length());                    
                    client1.print(html_page);        
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
```

In the example program above, we need to use `server.begin()` to start the IoT server. The statement needs to be placed in the `setup` function.

Once the program runs smoothly, you will be able to see our configured page on the XIAO ESP32C3's IP address page.

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

OpenAI provides very detailed [API usage instructions](https://platform.openai.com/docs/api-reference/making-requests) so that users can use their own API key to call ChatGPT.

According to the documentation of ChatGPT, the format we need to send the request is as follows:

```shell
curl https://api.openai.com/v1/completions \
-H "Content-Type: application/json" \
-H "Authorization: Bearer YOUR_API_KEY" \
-d '{"model": "text-davinci-003", "prompt": "Say this is a test", "temperature": 0, "max_tokens": 7}'
```

This request queries the Davinci model to complete the text starting with a prompt of "Say this is a test". The max_tokens parameter sets an upper bound on how many tokens the API will return. You should get a response back that resembles the following:

```shell
{
    "id": "cmpl-GERzeJQ4lvqPk8SkZu4XMIuR",
    "object": "text_completion",
    "created": 1586839808,
    "model": "text-davinci:003",
    "choices": [
        {
            "text": "\n\nThis is indeed a test",
            "index": 0,
            "logprobs": null,
            "finish_reason": "length"
        }
    ],
    "usage": {
        "prompt_tokens": 5,
        "completion_tokens": 7,
        "total_tokens": 12
    }
}
```

The above information is very important and provides the theoretical basis for writing HTTP Request and HTTP GET programs.

```c
char chatgpt_server[] = "api.openai.com";
WiFiClient client2;

if (client2.connect(chatgpt_server,443)){
    delay(3000);
    // Make a HTTP request          
    Serial.println("connect success!");
    client2.println(String("POST /v1/completions HTTP/1.1"));       
    client2.println(String("Content-Type: application/json"));
    client2.println(String("Authorization: Bearer ")+ chatgpt_token);
    client2.println(String("{\"model\":\"text-davinci-003\",\"prompt\":\"")+ chatgpt_Q + String("\",\"temperature\":0,\"max_tokens\":100}"));
    json_String= "";
}
else
{
    Serial.println("connect failed!");
    client2.stop();
    delay(1000);
}
```

In the program, we send the response body to the client via the `println()` method. `chatgpt_token` represents the OpenAI API key we obtained in the previous steps, and `chatgpt_Q` is the content of the question we want to send to ChatGPT, which will be available in the Get Question page.

Next, let's test the program to see if the serial port can print out the content of the questions we entered.

<div align=center><img width = 800 src="https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/12.png"/></div>

As you can see, I typed "Who are you" in the input box and clicked the **Submit** button, and the question was successfully logged.

## Get answers from ChatGPT

The next step is the last step of the whole tutorial, how we get the answer to ChatGPT and record it.

From the reference documentation provided by OpenAI, we know that the location of the answer to the question in the message returned by the interface is in `{"choices": [{"text": "xxxxxxx",}]}`.

```c
while (client2.available()) {
    json_String += (char)client2.read();
    data_now =1; 
}
if(data_now)
{
    Serial.println(json_String);
    dataStart = json_String.indexOf("\"text\":\"") + strlen("\"text\":\"");
    dataEnd = json_String.indexOf("\",\"", dataStart); 
    chatgpt_A = json_String.substring(dataStart+4, dataEnd);
    Serial.println(chatgpt_A);
    chatgpt_Q.replace("+", " ");
    Serial.println(chatgpt_Q);         

    chatgpt_num++;
    json_String = "";
    data_now =0;
    client2.stop();
    delay(1000);
}
```

Then, in the program, we can retrieve the position where the text starts and ends by using the `indexOf()` method, and store the content of the returned answer.

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

Then, feel free to use it! For example, right now you can enter your question in the question box, "What do you think of Seeed Studio XIAO as a product?"

<div align=center><img width = 800 src="https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/13.png"/></div>

## What's Next?

In this tutorial, we provide the basic method of calling OpenAI interface using ChatGPT for an embedded development board like Arduino - XIAO ESP32C3. Next, you can let your creativity run wild!

For example, could you consider adding a screen or keyboard to make him a stand-alone display device that works only for you? Come see Gavin's creativity, he made a particularly interesting surveillance device! We also give special thanks to him for providing the necessary steps and ideas for this tutorial.

- [Gavin - ChatGPT Recorder & Monitor](https://www.hackster.io/gavinchiong/chatgpt-recorder-monitor-601ef6)

<div align=center><img width = 600 src="https://files.seeedstudio.com/wiki/xiaoesp32c3-chatgpt/14.jpg"/></div>

Or go one step further and add a voice recognition module to get rid of the keyboard and mouse from now on and make a voice assistant of your own, etc. All in all, we'd love to see you share your work with a great product like the XIAO ESP32C3!

## Troubleshooting

### Q1: Why can't I get a reply from ChatGPT?

> A: It is possible that your XIAO ESP32C3 is connected to a network in a region that does not support ChatGPT, please refer to the [official documentation for the list of countries supported by ChatGPT](https://platform.openai.com/docs/supported-countries).

## Tech Support

Please do not hesitate to submit the issue into our [forum](https://forum.seeedstudio.com/).

<br /><p style="text-align:center"><a href="https://www.seeedstudio.com/act-4.html?utm_source=wiki&utm_medium=wikibanner&utm_campaign=newproducts" target="_blank"><img src="https://files.seeedstudio.com/wiki/Wiki_Banner/new_product.jpg" /></a></p>
