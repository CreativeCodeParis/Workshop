#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>

char ssid[] = "Livebox-8B20";
char pass[] = "Raclette_1986";

#define ID_BASE 200
#define ID 1

IPAddress staticIP(192, 168, 1, ID_BASE+ID);
IPAddress gateway(192, 168, 1, 254);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(192, 168, 1, 254);

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;
const IPAddress outIp(192,168,1,255);        // remote IP (not needed for receive)
const unsigned int outPort = 9999;          // remote port (not needed for receive)
const unsigned int localPort = 8888;        // local port to listen for UDP packets (here's where we send the packets)

int sensorPin = 1;

void setup()
{
  Serial.begin(115200);
  /*
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only. You need to open monitor to leave this loop...
  }
  */
  Serial.println("");
  Serial.println("Simple OSC ADC");

  if (!WiFi.config(staticIP, gateway, subnet, dns)) {
    Serial.println("STA Failed to configure");
  }
  
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WIFI connection retry");
    delay(100);
  }
  
  Serial.println("WIFI connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting UDP");
  Udp.begin(localPort);
}

void loop()
{
  int sensorValue = 0;
  sensorValue = analogRead(sensorPin);
  OSCMessage msg("/sensor");
  msg.add(sensorValue);
  Udp.beginPacket(outIp, outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();

  delay(100);
}