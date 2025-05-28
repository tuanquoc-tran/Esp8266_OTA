#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>

ESP8266WebServer webServer(80);
DNSServer dnsServer;

const char *ssid_ap = "Esp8266";
const char *pass_ap = "12345678";
uint8_t max_connections = 8;

const byte DNS_PORT = 53;
int numOfDevices = 0; 
int pre_numOfDevices = 0;

#define LED_STATUS      D4

#define CONFIG_AP_URL                 "demoesp8266.local"
#define WIFI_AP_IP                    IPAddress(192, 168, 4, 1)
#define WIFI_AP_Subnet                IPAddress(255, 255, 255, 0)

void on();
void off();
void mainpage();


//=========Biến chứa mã HTLM Website==//
const char MainPage[] PROGMEM = R"=====(
  <!DOCTYPE html> 
  <html>
   <head> 
       <title>HOME PAGE</title> 
       <title>HOME PAGE</title> 
       <style> 
          body {text-align:center;}
          h1 {color:#003399;}
          a {text-decoration: none;color:#FFFFFF;}
          .bt_on {height:50px; width:100px; margin:10px 0;background-color:#FF6600;border-radius:5px;}
          .bt_off {height:50px; width:100px; margin:10px 0;background-color:#00FF00;border-radius:5px;}
       </style>
       <meta charset="UTF-8">
   </head>
   <body> 
      <div class="centered">
      <h1>Web Local</h1> 
        <button ><a href="/on">ON</a></button>
        <button ><a href="/off">OFF</a></button>
        </form>
      </div>

   </body> 
  </html>
)=====";

//=========================================//
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("__Debug: Serial is connecting...");

  pinMode(LED_STATUS,OUTPUT);
  digitalWrite(LED_STATUS,HIGH);

  /* Access Point Mode */
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  IPAddress myAPIP;
  WiFi.softAPConfig(WIFI_AP_IP, WIFI_AP_IP, WIFI_AP_Subnet);
  if (WiFi.softAP(ssid_ap, pass_ap,1,false,max_connections))
  {
    myAPIP = WiFi.softAPIP();
    Serial.println("__Debug: Access Point is Creadted");
    
    /* Set up DNS Server */
    dnsServer.setTTL(300); // Time-to-live 300s
    dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure); // Return code for non-accessible domains
    dnsServer.start(DNS_PORT, CONFIG_AP_URL, myAPIP);
    Serial.print ("__Debug: URL: ");
    Serial.println(CONFIG_AP_URL);
  }
  else
  {
    Serial.println("__Debug: Unable to Create Access Point");
  }
  
  webServer.on("/",mainpage);
  webServer.on("/on",on);
  webServer.on("/off",off);

  webServer.begin();
  
}
void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();

  numOfDevices = WiFi.softAPgetStationNum(); // Get the number of devices connected to the access point
  if (pre_numOfDevices != numOfDevices)
  {
    Serial.println("__Debug: Device connected!");
    Serial.println("__Debug: Number of connected devices: " + String(numOfDevices));
  }
  pre_numOfDevices = numOfDevices;
}

void on()
{
  digitalWrite(LED_STATUS, LOW); 
  String s = FPSTR(MainPage);
  webServer.send(200,"text/html",s);
}

void off()
{
  digitalWrite(LED_STATUS, HIGH); 
  String s = FPSTR(MainPage);
  webServer.send(200,"text/html",s);
}

void mainpage(){
  String s = FPSTR(MainPage);
  webServer.send(200,"text/html",s);
}
