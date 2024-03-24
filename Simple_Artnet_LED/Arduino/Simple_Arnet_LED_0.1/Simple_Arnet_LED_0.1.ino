#include <ArtnetWiFi.h>
#include <FastLED.h>

char ssid[] = "TP-Link_2C3B";
char pass[] = "MeetupCCP";

#define ID_BASE 210
#define ID 13

//#define NUM_LEDS 64 // Matrix 8x8
//#define NUM_LEDS 21 // Strip 21
#define NUM_LEDS 16 // Circle

//#define DATA_PIN 16 //GPIO16 D0 for esp8266
#define DATA_PIN 1 //GPIO1 for ESP32

IPAddress staticIP(192, 168, 1, ID_BASE+ID);
IPAddress gateway(192, 168, 1, 254);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(192, 168, 1, 254);

ArtnetWiFiReceiver artnet;

CRGB leds[NUM_LEDS];

void setup()
{
  Serial.begin(115200);
/*
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only. You need to open monitor to leave this loop...
  }
*/
  Serial.println("");
  Serial.println("Simple Artnet LED");
  
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed
  int i;
  for( i = 0; i < NUM_LEDS; i++ )
  {
    leds[i] = CRGB::Black;
  }
  leds[0] = CRGB::Red;
  FastLED.show();

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

  leds[0] = CRGB::Green;
  FastLED.show();

  artnet.begin();

  artnet.subscribe([&](const uint32_t universe, const uint8_t* data, const uint16_t size)
  {
    Serial.println("artnet package");
    int i;
    for( i = 0; i < NUM_LEDS; i++ )
    {
      int pixel = 3*i;
      leds[i].r = data[pixel];
      leds[i].g = data[pixel+1];
      leds[i].b = data[pixel+2];
    }
    FastLED.show();
  });
}

void loop()
{
  artnet.parse();
}