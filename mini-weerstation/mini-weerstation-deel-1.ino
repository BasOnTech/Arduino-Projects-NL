/*
 * Bas on Tech - Arduino tutorial
 * http://basontech.nl
 * 
 *
 * PIN AANSLUITINGEN DHT11 & DHT22
 *
 * Vcc  --> 5V  (rood)
 * Data --> 12  (groen)
 * NC
 * GND  --> GND (zwart)
 *
 * 4K7 (4700) Ohm weerstand tussen 5V en Data pin
 *
 *
 * PIN AANSLUITINGEN LCD
 * 
 * SDA is serial data
 * SCL is serial clock
 *
 * GND --> GND (wit)
 * VCC --> 5V  (grijs)
 * SDA --> A4  (rood)
 * SCL --> A5  (blauw)
 * 
 * I2C aanlsluitingen per Arduino:
 * Uno, Ethernet    A4 (SDA), A5 (SCL)
 * Mega2560         20 (SDA), 21 (SCL)
 * Leonardo          2 (SDA),  3 (SCL)
 * Due              20 (SDA), 21 (SCL) of SDA1, SCL1
 *
 */


//// LCD SCHERM ////

// Bibliotheek voor communicatie met I2C / TWI apparaten
#include <Wire.h> 

// Bibliotheek voor het LCD scherm
#include <LiquidCrystal_I2C.h>

/* 
 * Stel hier in welke chip en foromaat LCD je hebt
 * Gebruik 0x27 als je chip PCF8574 hebt van NXP
 * Gebruik 0x3F als je chip PCF8574A hebt van Ti (Texas Instruments)
 * De laatste twee getallen geven het formaat van je LCD aan
 * bijvoorbeeld 20x4 of 16x2
 *  
 */  
LiquidCrystal_I2C lcd(0x27, 20, 4);
//LiquidCrystal_I2C lcd(0x3F, 16, 2);

/* 
 * Dit druppel-icoon is online gemaakt met de LCD symbool generator op:
 * https://maxpromer.github.io/LCD-Character-Creator/
 */
byte humidityIcon[] = {
  0x04,
  0x04,
  0x0E,
  0x0E,
  0x1F,
  0x1F,
  0x1F,
  0x0E
};

bool redrawLCD = true;                 // geeft aan of het scherm moet worden bijgewerkt


//// DHT-SENSOR ////

#include <DHT.h>                       // Bibliotheek voor DHT sensoren
static const int dhtPin = 12;          // data pin van DHT sensor

// Stel hier in welke DHT chip je gebruikt
#define dhtType DHT11                  // DHT 11
//#define dhtType DHT22                // DHT 22  (AM2302), AM2321
//#define dhtType DHT21                // DHT 21 (AM2301)

DHT dht(dhtPin, dhtType);              // Initialiseer de DHT bibliotheek in variabele "dht"

float humidityVal;                     // Variabele voor luchtvochtigheid
int intervalDHT = 1000;                // Tijd in milliseconden tussen het uitlezen van de DHT sensor
unsigned long previousMillisDHT = 0;   // Tijdstip van laatste uitlezing DHT sensor


//// ALGEMEEN ////

unsigned long currentMillis = 0;   // Variabele voor het aantal milliseconden sinds de Arduino is gestart


void setup() {
  Serial.begin(9600);              // stel de seriële monitor in

  // LCD
  lcd.init();                      // initialiseer het LCD scherm
  lcd.backlight();                 // zet de backlight aan
  lcd.createChar(0, humidityIcon); // definieer een symbool in geheugen positie 0

  // DHT sensor
  dht.begin();                     // start het DHT sensor uitlezen

}

// Functie voor het uitlezen van de DHT sensor
void readDHT() {

    // Als het tijdsverschil met de vorige uitlezing groter is dan intervalDHT
    if(currentMillis - previousMillisDHT > intervalDHT) {

        // vraag de luchtvochtigheid aan de DHT sensor
        float readHumidityVal = dht.readHumidity();       

        // Controleer of de gelezen waarde een getal is (Nan = Not A Number)
        if (isnan(readHumidityVal)) {
            Serial.println("Uitlezen van DHT sensor mislukt!");
            return;
        }

        // Controleer of de gelezen waarde anders is dan de vorige waarde
        if (readHumidityVal != humidityVal) {
          humidityVal = readHumidityVal;     // werk humidityVal bij met de nieuwe waarde
          redrawLCD = true;                  // geef aan dat het LCD bijgewerkt moet worden
        }

        previousMillisDHT = currentMillis;   // sla het huidige tijdstip op in previousMillisDHT
    }

}

// Functie om het LCD scherm bij te werken
void updateLCD() {
  
  // Als redrawLCD door iets op 'true' is gezet
  if(redrawLCD) {
    
    lcd.clear();                 // wis het scherm
  
    lcd.setCursor(0, 0);         // zet de cursor op positie 1, regel 1
    lcd.write(0);                // teken het druppel icoon van geheugenpositie 0
    lcd.print(" ");              // schrijf een spatie
    lcd.print(humidityVal);      // schrijf de huidige luchtvochtigheid
    lcd.print("%");              // schrijf een % teken
  
    redrawLCD = false;           // set redrawLCD weer op 'false'

  }

}


void loop() {

    currentMillis = millis();    // sla de huidige tijd op
    readDHT();                   // lees de DHT sensor uit
    updateLCD();                 // werk indien nodig het LCD scherm bij
  
}
