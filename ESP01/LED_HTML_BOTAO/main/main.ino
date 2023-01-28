/*
    This sketch demonstrates how to set up a simple HTTP-like server.
    The server will set a GPIO pin depending on the request
      http://server_ip/gpio/0 will set the GPIO2 low,
      http://server_ip/gpio/1 will set the GPIO2 high
    server_ip is the IP address of the ESP8266 module, will be
    printed to Serial when the module is connected.
*/

#include <ESP8266WiFi.h>


//variáveis
IPAddress ip(192, 168, 18, 60);   //mudar o ultimo digito
IPAddress ip1(192, 168, 18, 1);
IPAddress ip2(255, 255, 255, 0);


#ifndef STASSID
#define STASSID "Raymann CWB"
#define STAPSK  "123pin45#"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

void setup() {
  Serial.begin(115200);

  // prepare LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();
  Serial.println(F("WiFi connected"));
//setup após a conexão
 WiFi.config(ip, ip1, ip2); 

  // Start the server
  server.begin();
  Serial.println(F("Server started"));

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  Serial.println(F("new client"));

  client.setTimeout(5000); // default is 1000

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(F("request: "));
  Serial.println(req);

  // Match the request
  int val;
  
  if (req.indexOf("?Desliga=") != -1)      
    val = 0;
  else if (req.indexOf("?Liga=") != -1)    
    val = 1;

   else {
    Serial.println(F("invalid request"));
    val = digitalRead(LED_BUILTIN);
  }

  // Set LED according to the request
  digitalWrite(LED_BUILTIN, val);

  // read/ignore the rest of the request
  // do not client.flush(): it is for output only, see below
  while (client.available()) {
    // byte by byte is not very efficient
    client.read();
  }

  // Send the response to the client
  // it is OK for multiple small client.print/write,
  // because nagle algorithm will group them into one single packet
  
  
  client.print(F("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\n "));
  client.print(F("<h1><html style=\"font-size:70px\"> Luminaria do Quarto\n</h1>"));
    client.print(F("<body>\n"));
   client.print(F("<form action=\"http://"));
  
    client.print(WiFi.localIP());
  
   client.print(F("\" method=\"get\">\n"));
  client.print(F("<p></p><button input name=\"Desliga\" style=\"height:100px;width:750px;font-size:65px\" >Desligar</button>")); //<br></br>
  client.print(F("<button input name=\"Liga\" style=\"height:100px;width:750px;font-size:65px\" >Ligar</button>"));
  client.print(F("<p></p>"));
  client.print(F("</form>\n"));
  client.print(F("</body>\n"));
  

  
  client.print(F("</html>"));




  // The client will actually be *flushed* then disconnected
  // when the function returns and 'client' object is destroyed (out-of-scope)
  // flush = ensure written data are received by the other side
  Serial.println(F("Disconnecting from client"));
}
