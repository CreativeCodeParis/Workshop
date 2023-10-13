#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>

char ssid[] = "TP-Link_2C3B";
char pass[] = "MeetupCCP";

#define ID_BASE 200
#define ID 1

#define SENSOR_NB 1

IPAddress staticIP(192, 168, 1, ID_BASE+ID);
IPAddress gateway(192, 168, 1, 254);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(192, 168, 1, 254);

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;
const IPAddress outIp(192,168,1,255);        // remote IP (not needed for receive)
const unsigned int outPort = 9999;          // remote port (not needed for receive)
const unsigned int localPort = 8888;        // local port to listen for UDP packets (here's where we send the packets)

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
  int i;
  
  int sensorPin;
  int sensorValue;
  char sensorName[30];

  for(i=1; i<SENSOR_NB+1; i++)
  {
    sensorPin = i; // The ADC pin 1-18
    sensorValue = analogRead(sensorPin);

    sprintf(sensorName, "/Device/%d/Sensor/%d", ID, sensorPin);
    OSCMessage msg(sensorName);
    msg.add(sensorValue);
    Udp.beginPacket(outIp, outPort);
    msg.send(Udp);
    Udp.endPacket();
    msg.empty();
  }

  delay(100);
}
