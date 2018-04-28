/*
   Bas on Tech - Arduino tutorial
   https://arduino-lessen.nl

   PIN AANSLUITINGEN LCD

   SDA is serial data
   SCL is serial clock

   GND --> GND (wit)
   VCC --> 5V  (grijs)
   SDA --> A4  (rood)
   SCL --> A5  (blauw)

   I2C aansluitingen per Arduino:
   Uno, Ethernet    A4 (SDA), A5 (SCL)
   Mega2560         20 (SDA), 21 (SCL)
   Leonardo          2 (SDA),  3 (SCL)
   Due              20 (SDA), 21 (SCL) of SDA1, SCL1

*/

//// ALGEMEEN ////

// Variabele voor het aantal milliseconden sinds de Arduino is gestart
unsigned long currentMillis = 0;       

//// DRUKKNOPPEN ////
const int intervalButtonRead = 100;         // tijd in milliseconden tussen het uitlezen van de drukknop
unsigned long previousMillisButton = 0;     // tijdstip in milliseconden van vorig uitleesmoment van drukknop

const int buttonStartPin = 12;              // start/stop knop pin
int buttonStartState = LOW;                 // staat van de start/stop knop
int buttonStartStatePrevious = LOW;         // vorige staat van start/stop knop

const int buttonResetPin = 10;              // reset knop pin
int buttonResetState = LOW;                 // staat van de reset knop
int buttonResetStatePrevious = LOW;         // vorige staat van reset knop

// Functie voor het uitlezen van de drukknop
void readButtonState() {

  // Als het tijdsverschil met de vorige uitlezing groter is dan intervalButtonRead
  if (currentMillis - previousMillisButton > intervalButtonRead) {

    // Lees de digitale waarden van de schakelaars (LOW/HIGH)
    buttonStartState = digitalRead(buttonStartPin);
    buttonResetState = digitalRead(buttonResetPin);

    // Als de start knop is ingedrukt
    if (buttonStartState == HIGH) {
      Serial.println("start knop ingedrukt");
    }

    // Als de reset knop is ingedrukt
    if (buttonResetState == HIGH) {
      Serial.println("reset knop ingedrukt");
    }
  }
}



void setup() {
  Serial.begin(9600);                   // stel de seriële monitor in
}

void loop() {
  currentMillis = millis();             // sla de huidige tijd op
  readButtonState();                    // lees de huidige staat van de drukknopen uit
}

