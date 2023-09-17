//----------------------------------LIBRARIES AND VARIABLE DECLARATIONS---------------------------------------//


#include <ESP8266WiFi.h> //wifi library 
#include <PubSubClient.h> // Library MQTT connections 
#include <time.h> // Libraries to verify date and time
#include <TZ.h>
#include <FS.h>
#include <LittleFS.h> // Library to upload the certificates
#include <CertStoreBearSSL.h> // Library to verify of certificates
#include <SPI.h> // Library for LoRa communications
#include <LoRa.h> // Library for LoRa communications
#define ss 15 //LoRa module NSS pin
#define rst 16 //LoRa module RST pin
#define dio0 4 //LoRa module  DIO0 pin

int counter = 0;              // Transission packet counter
const int lightpin=D1;        // LED to indicate MQTT connection succesfull
String inmsg="";              //  To store message incoming from MQTT server
String incoming="";           //  To store message incoming from slave via LoRa
const char *topic_subscribe="control"; // Topic to subscribe where MQTT will publish
const char *topic_publish = "Sensordata"; // Topic to Publish to MQTT at



// Update these with values suitable for your network.
const char* ssid = "DC";
const char* password = "dhruv1234";
const char* mqtt_server = "75a0d190cbec4e48be1541fd29bb5360.s1.eu.hivemq.cloud";
//const char* On="LedOn";

// A single, global CertStore which can be used by all connections.
// Needs to stay live the entire time any of the WiFiClientBearSSLs
// are present.
BearSSL::CertStore certStore;

WiFiClientSecure espClient;
PubSubClient * client;
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (500)
char msg[MSG_BUFFER_SIZE];

//-------------------------------------------WIFI-------------------------------------------------//

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

//--------------------------------------DATE&TIME--------------------------------------------//
void setDateTime() {
  // You can use your own timezone, but the exact time is not used at all.
  // Only the date is needed for validating the certificates.
  configTime(TZ_Europe_Berlin, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(100);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println();

  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.printf("%s %s", tzname[0], asctime(&timeinfo));
}

//--------------------------------MQTT SUBSCRIBE AND LoRa TRANSMISSION FUNCTION---------------------------------------------------// 

void callback(char* topic, byte* payload, unsigned int length) {
  for (int i = 0; i < length; i++) {
    inmsg+=(char)payload[i];
  }
  Serial.println("message arrived ["+String(topic)+"]"+inmsg);
  

  // send burst of packet so that arduino nano catches one of them 
  for(int j =0 ; j<50 ; j++){
  LoRa.beginPacket();
  LoRa.print(inmsg);
  LoRa.endPacket();
  Serial.print("Sending packet: ");
  Serial.println(j);
  }

  inmsg="";

}

//--------------------------------------MQTT CONNECTION---------------------------------------------------//

void reconnect() {
  // Loop until we’re reconnected
  while (!client->connected()) {
    Serial.print("Attempting MQTT connection…");
    String clientId = "ESP8266Client - MyClient";
    // Attempt to connect
    // Insert your password
    if (client->connect(clientId.c_str(), "device001", "device001")) {
      Serial.println("connected");
      digitalWrite(lightpin, HIGH);
      client->subscribe("control");
    } else {
      digitalWrite(lightpin, LOW);
      Serial.print("failed, rc = ");
      Serial.print(client->state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


//----------------------------------------SETUP LoRa AND MQTT CERTIFICATES----------------------------------------------//

void setup() {
  delay(500);
  // When opening the Serial Monitor, select 9600 Baud
  Serial.begin(9600);
  pinMode(lightpin, OUTPUT);
  delay(500);
  LittleFS.begin(); // upload certificate
  setup_wifi();
  setDateTime();

  int numCerts = certStore.initCertStore(LittleFS, PSTR("/certs.idx"), PSTR("/certs.ar"));
  Serial.printf("Number of CA certs read: %d\n", numCerts);
  if (numCerts == 0) {
    Serial.printf("No certs found. Did you run certs-from-mozilla.py and upload the LittleFS directory before running?\n");
    return; // Can't connect to anything w/o certs!
  }

  BearSSL::WiFiClientSecure *bear = new BearSSL::WiFiClientSecure();
  // Integrate the cert store with this connection
  bear->setCertStore(&certStore);
  
  // you can use the insecure mode, when you want to avoid the certificates
  //espclient->setInsecure();

  client = new PubSubClient(*bear);

  client->setServer(mqtt_server, 8883);
  client->setCallback(callback);

  while (!Serial);

  Serial.println("LoRa Two Way Communication");
  LoRa.setPins(ss, rst, dio0);


  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.setSyncWord(0xA5); //isolates network
}

//------------------------LOOP TO RECEIVE VIA LoRa AND MQTT -------------------------------//  


void loop() {
  if (!client->connected()) {
    reconnect();
  }
  client->loop();
 onReceive(LoRa.parsePacket()); 
}

  
//-------------------------RECEIVE LoRa AND MQTT PUBLISH FUNCTION-----------------------------//


void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return
   incoming = "";

  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }
  
  Serial.println("Message: " + incoming); //publish arduino's message
    String pkt = "{";
      pkt += incoming;
      pkt += "}";
      mqtt_publish((char *)pkt.c_str());

}
void mqtt_publish(char *data) //publish function
{
  Serial.println("Publish Topic: \"" + String(topic_publish) + "\"");
  if (client->publish(topic_publish, data))
    Serial.println("Publish \"" + String(data) + "\" ok");
  else
    Serial.println("Publish \"" + String(data) + "\" failed");
}
