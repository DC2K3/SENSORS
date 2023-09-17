const int buzzerpin = 10 ; // buzzeer pin
const int tsalpin = 11 ; //tsal and power relay
int ndrpin = A5 ; //NDR input
int pressurepin = A4 ;//pressure switch
bool tsalstate = false;
int i = 0 ;

void setup() 
{
  Serial.begin(9600);
  pinMode(ndrpin , INPUT_PULLUP);
  pinMode(pressurepin , INPUT_PULLUP);
  pinMode(buzzerpin , OUTPUT);
  pinMode(tsalpin , OUTPUT);
  digitalWrite(tsalpin, HIGH);
  digitalWrite(buzzerpin, HIGH);
}

void loop() 
{
  int value = analogRead(A5);
  int brake = analogRead(A4);
  tsalstate = digitalRead(tsalpin);
  Serial.print("tsal state: ");
  Serial.println( tsalstate );  //basically if HV on or no
  delay (1000);

  //check for reverse value
  if((value>3) && (value<80) )
  {
    Serial.print("Car is in reverse  ");
    Serial.print( "   | ndr value : ");
    Serial.print( value );
    Serial.print( "   | brake value : ");
    Serial.println( brake );
    if(tsalstate==0)
    {
      digitalWrite(buzzerpin ,LOW); //beep
      delay(500);
      digitalWrite(buzzerpin, HIGH);
    }
  }
  //check if neutral 
  else if ((value>150) && (value<2000) )
  {
    Serial.print("Car is in nuetral  | ndr value :  ");
    Serial.print( value );
    Serial.print( "   | brake value : ");
    Serial.println( brake );
    if( brake>500)
    {
      while (i<1)
      {
        // Play buzzer for 3 seconds
        Serial.println("neutral beep is executing");
        digitalWrite(buzzerpin, LOW);
        digitalWrite(tsalpin, LOW);
        delay(2000);
        digitalWrite(buzzerpin, HIGH);
        // Turn on TACL relay and power relay
        i++;
      }
    }
  }

  //otherwise it is in drive
  else if (value<2) 
  {
    digitalWrite(buzzerpin, HIGH);
    Serial.print("Car is in drive  | ndr value :  ");
    Serial.println( value );
    Serial.print( "   | brake value : ");
    Serial.println( brake );
  }
}
