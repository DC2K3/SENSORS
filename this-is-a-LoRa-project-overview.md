

## LoRa communication protocol

LoRa (from "long range") is a physical proprietary radio communication technique. It is based on spread spectrum modulation techniques derived from chirp spread spectrum (CSS) technology.It encodes information on radio waves using chirp pulses - similar to the way dolphins and bats communicate!
LoRa provides for long-range communications: up to three miles (five kilometers) in urban areas, and up to 10 miles (15 kilometers) or more in rural areas (line of sight).
In this project we will create a duplex communication involving esp8266 (master) and arduino nano(slave). We will also be connecting it with a MQTT server (HiveMQ). An LED will be connected to the arduino nano which will be controlled via LoRa and through MQTT.
A message controlling the LED on or off will be published from the MQTT server to esp8266 then that same message will be transmitted to arduino nano using LoRa. Hence arduino will act accordingly.
We will also be able to fetch the status of the light from arduino nano to esp8266 with the help of LoRa and then publish it on the MQTT server. An ultrasonic sensor will also be connected to the arduino nano and if its reading goes below a certain point , it will be published on the MQTT.
This project can be used in the real world to create remotly accessed doors , lights and many more such devices.
For example. This can be used detect a car and open the gate for the car automatically or remotly open in the car without physically going near the gate.
