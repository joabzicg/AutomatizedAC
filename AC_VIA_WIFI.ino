#include <IRremote.h> //INCLUSÃO DE BIBLIOTECA
IRsend irsend; //FUNÇÃO RESPONSÁVEL PELO MÉTODO DE ENVIO DO SINAL IR / UTILIZA O GPIO14(D5)
unsigned int L[199] = {4434,4346,583,1581,556,538,557,1584,554,1610,555,495,575,522,572,1583,556,526,569,510,559,1608,556,511,558,539,556,1582,557,1608,556,511,558,1608,556,467,602,538,557,1582,556,1608,557,1607,556,1583,556,1608,556,1582,556,1608,556,1582,556,536,559,510,559,538,558,511,558,539,556,513,556,539,556,513,556,1608,557,512,557,538,556,513,557,512,557,538,557,1581,557,1608,556,512,583,1582,556,1608,556,1582,556,1609,555,1583,556,5178,4390,4346,583,1582,555,522,574,1582,556,1608,555,494,576,537,558,1583,555,537,559,487,582,1608,556,511,558,539,556,1582,556,1608,556,512,557,1609,555,512,584,489,580,1608,556,1582,556,1608,556,1582,556,1608,556,1583,555,1609,556,1608,556,498,570,538,558,512,557,512,557,539,556,513,554,541,554,515,554,1611,554,514,555,540,555,514,555,541,555,514,554,1610,554,1584,555,540,555,1583,555,1609,555,1584,554,1610,554,1610,555}; //COLE A LINHA RAW CORRESPONDENTE DENTRO DAS CHAVES
unsigned int D[199] = {4410,4370,558,1582,556,535,560,1584,555,1609,555,491,578,526,569,1582,556,537,558,512,557,1609,556,511,558,514,581,1583,556,1607,557,489,579,1609,556,507,561,1609,556,1582,556,1608,556,1608,556,511,558,1609,556,1582,555,1609,555,512,557,538,557,489,581,539,556,1582,556,539,556,513,556,1608,556,1583,555,1609,556,511,558,524,571,513,556,539,556,513,556,540,555,513,556,513,582,1582,556,1609,555,1583,556,1608,556,1582,556,5147,4422,4370,558,1577,561,517,578,1583,555,1609,555,494,575,537,559,1582,557,529,565,511,558,1611,553,487,582,539,557,1582,556,1608,556,511,557,1609,556,511,584,1583,556,1608,556,1582,555,1609,556,491,578,1608,556,1582,556,1608,556,517,578,488,581,512,557,540,556,1582,556,538,557,512,554,1611,554,1610,554,1584,554,539,557,514,555,514,555,540,556,513,555,541,555,514,554,541,555,1583,555,1609,554,1584,555,1609,555,1610,554}; //COLE A LINHA RAW CORRESPONDENTE DENTRO DAS CHAVES
int tamanho = 199; //TAMANHO DA LINHA RAW(68 BLOCOS)
int frequencia = 38; //FREQUÊNCIA DO SINAL IR(32KHz)

#include <WiFi.h>
#include "esp_wpa2.h" //wpa2 library for connections to Enterprise networks
#define EAP_IDENTITY "" //if connecting from another corporation, use identity@organisation.domain in Eduroam 
#define EAP_PASSWORD "" //your Eduroam password
const char* ssid = "eduroam"; // Eduroam SSID
int counter = 0;

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String ligarARState = "off";


// Assign output variables to GPIO pins
const int ligarAR = 18;

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(ligarAR, OUTPUT);
  // Set outputs to LOW
  digitalWrite(ligarAR, LOW);
  delay(10);
  Serial.println();
  Serial.print("Connecting to network: ");
  Serial.println(ssid);
  WiFi.disconnect(true);  //disconnect form wifi to set new wifi connection
  WiFi.mode(WIFI_STA); //init wifi mode
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY)); //provide identity
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY)); //provide username --> identity and username is same
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD)); //provide password
  esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT(); //set config settings to default
  esp_wifi_sta_wpa2_ent_enable(&config); //set config settings to enable function
  WiFi.begin(ssid); //connect to wifi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    counter++;
    if(counter>=60){ //after 30 seconds timeout - reset board
      ESP.restart();
    }
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address set: "); 
  Serial.println(WiFi.localIP()); //print LAN IP
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /26/on") >= 0) {
              Serial.println("GPIO 26 on");
              ligarARState = "on";
                delay(50);
               irsend.sendRaw(L,199,38);  // PARÂMETROS NECESSÁRIOS PARA ENVIO DO SINAL IR
            } else if (header.indexOf("GET /26/off") >= 0) {
              Serial.println("GPIO 26 off");
              ligarARState = "off";
                delay(50);
                irsend.sendRaw(D,199,38);  // PARÂMETROS NECESSÁRIOS PARA ENVIO DO SINAL IR
            } 
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>Ligando AR via WI-FI!</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 26  
            client.println("<p>AR - Estado " + ligarARState + "</p>");
            // If the output26State is off, it displays the ON button       
            if (ligarARState=="off") {
              client.println("<p><a href=\"/26/on\"><button class=\"button\">LIGAR!</button></a></p>");
            } else {
              client.println("<p><a href=\"/26/off\"><button class=\"button button2\">DESLIGAR!</button></a></p>");
            } 

            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
