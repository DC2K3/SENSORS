

#include <SPI.h>
#include <LoRa.h>
#define ss 10 //LoRa module NSS pin
#define rst 9 //LoRa module RST pin
#define dio0 2 //LoRa module dio0 pin


const int trigPin = 7;
const int echoPin = 4;
const int lightPin = 3;

//define sound speed in cm/uS
#define SOUND_SPEED 0.034

long duration;
float distanceCm;
String outgoing = "";
String status="";


void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(lightPin, OUTPUT); // Sets the lightPin as an Onput
  digitalWrite(lightPin, LOW);
  while (!Serial);

  Serial.println("LoRa Two Way Communication");
  LoRa.setPins(ss, rst, dio0);


  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1); // runs only once
  }
  LoRa.setSyncWord(0xA5); // isolates network
}

void loop() {
  
  delay(1000);
  
  onReceive(LoRa.parsePacket());
  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_SPEED/2;
  
  
  // Prints the distance in the Serial Monitor
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
  if (distanceCm <= 10)
  {
   outgoing="OBJECT AT DISTANCE : ";
   Serial.print("sending object distance" );
   LoRa.beginPacket();                                       // start packet
   LoRa.print(outgoing + String(distanceCm));                // add payload
   LoRa.endPacket();                                         // end payload
  }
  
}

void onReceive(int packetSize) {
   
  if (packetSize == 0) return;          // if there's no packet, return
  String incoming = "";


  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }
  Serial.println("Message: " + incoming);
   
  delay(500);

    if (incoming=="1"){
      digitalWrite(lightPin, HIGH);
      status="LED ON";
      Serial.println("light on");
    }
    else if (incoming=="0"){
    digitalWrite(lightPin, LOW);
    status="LED OFF";
    Serial.println("light off");
    }
    else if (incoming=="status") {
    Serial.println("SENDING STATUS")  ;
    for(int j =0 ; j<5 ; j++){
    LoRa.beginPacket();
    LoRa.print(status);
    LoRa.endPacket();
    Serial.print("packet no.");
    Serial.println(j);
    }
  }
  }

 



  



