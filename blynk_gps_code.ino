
#define BLYNK_TEMPLATE_ID "TMPL3_lCqvVQj"
#define BLYNK_TEMPLATE_NAME "GPSNEW"
#define BLYNK_AUTH_TOKEN "uJwYUuLnlYrpvAbEgf3hjV1FtgpURFs2"
//------------------------------------------------------------------------------

char auth[] = BLYNK_AUTH_TOKEN;
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "DC";
char pass[] = "dhruv1234";
//------------------------------------------------------------------------------
// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial

//------------------------------------------------------------------------------
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

//------------------------------------------------------------------------------

//GPS Module Settings
//GPS Module RX pin to ESP32 16
//GPS Module TX pin to ESP32 17
#include <TinyGPS++.h> 
#define RXD2 17
#define TXD2 16
HardwareSerial neogps(2);
TinyGPSPlus gps;
//------------------------------------------------------------------------------

BlynkTimer timer;

#define INTERVAL 1000L

   
const int trigPin = 5;
const int buzzerpin=22;
const int echoPin = 18;

//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;
float distanceInch;

/************************************************************************************
 *  This function sends Arduino's up time every second to Virtual Pin.
 *  In the app, Widget's reading frequency should be set to PUSH. This means
 *  that you define how often to send data to Blynk App.[]
 **********************************************************************************/

void sendOD ()
{
   // Clears the trigPin
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
  
  // Convert to inches
  distanceInch = distanceCm * CM_TO_INCH;
  
  // Prints the distance in the Serial Monitor
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
  Serial.print("Distance (inch): ");
  Serial.println(distanceInch);
  Blynk.virtualWrite(V3, String(distanceCm));
  if (distanceCm <= 10)
  {
   digitalWrite(buzzerpin, LOW) ;
  }
  
}
void sendGps()
{
 
  //-----------------------------------------------------------
  while(neogps.available())
  {
    if (gps.encode(neogps.read()))
    {
      break;
    }
  }
  //-----------------------------------------------------------
  if (!gps.location.isValid())
  {
    Serial.println("Failed to read from GPS Module!");
    return;
  }
  //-----------------------------------------------------------
  //get latitude and longitude
  float latitude = gps.location.lat();
  float longitude = gps.location.lng();
  float speed = gps.speed.kmph();
  //-----------------------------------------------------------
  //comment out this block of code to save space
  //used for debugging in serial monitor
  Serial.print("Latitude:  ");
  Serial.println(latitude, 6);
  Serial.print("Longitude: ");
  Serial.println(longitude, 6);
  Serial.print("Speed: ");
  Serial.println(speed, 6);
  //-----------------------------------------------------------
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V1, String(latitude, 6));
  Blynk.virtualWrite(V2, String(longitude, 6));
  Blynk.virtualWrite(V0, String(speed));
  
  //-----------------------------------------------------------
}



/************************************************************************************
 *  setup() function
 **********************************************************************************/
void setup()
{
  //-----------------------------------------------------------
  //Debug console (Serial Monitor)
  Serial.begin(9600);
  //-----------------------------------------------------------
  Blynk.begin(auth, ssid, pass);
  //You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(auth, s`sid, pass, IPAddress(192,168,1,100), 8080);
  //-----------------------------------------------------------
  //Set GPS module baud rate
  neogps.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial.println("neogps serial initialize");
  delay(10);
  //-----------------------------------------------------------
  // Setup a function to be called every second
  timer.setInterval(INTERVAL, sendGps);
  timer.setInterval(INTERVAL, sendOD);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);
  pinMode(buzzerpin, OUTPUT); // Sets the echoPin as an Input
  //-----------------------------------------------------------
}


/************************************************************************************
 *  loop() function
 **********************************************************************************/
void loop()
{
  Blynk.run();
  timer.run();
  
}