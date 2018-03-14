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
 * 
 * PIN AANSLUITINGEN 1-wire DS18B20
 *
 * GND  --> GND (zwart)
 * Data --> 10  (geel)
 * Vcc  --> 5V  (rood)
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

byte degreeSymbol[] = {
  0x07,
  0x05,
  0x07,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00
};

byte temperatureIcon[] = {
  0x04,
  0x0A,
  0x0A,
  0x0A,
  0x0E,
  0x1F,
  0x1F,
  0x0E
};

byte maxIcon[] = {
  0x1F,
  0x04,
  0x0E,
  0x1F,
  0x04,
  0x04,
  0x04,
  0x04
};

byte minIcon[] = {
  0x04,
  0x04,
  0x04,
  0x04,
  0x1F,
  0x0E,
  0x04,
  0x1F
};

bool redrawLCD = true;                  // geeft aan of het scherm moet worden bijgewerkt


//// DHT-SENSOR ////

#include <DHT.h>                        // Bibliotheek voor DHT sensoren
static const int dhtPin = 12;           // data pin van DHT sensor

// Stel hier in welke DHT chip je gebruikt
#define dhtType DHT11                   // DHT 11
//#define dhtType DHT22                 // DHT 22  (AM2302), AM2321
//#define dhtType DHT21                 // DHT 21 (AM2301)

DHT dht(dhtPin, dhtType);               // Initialiseer de DHT bibliotheek in variabele "dht"

float humidityVal;                      // Variabele voor luchtvochtigheid
int intervalDHT = 1000;                 // Tijd in milliseconden tussen het uitlezen van de DHT sensor
unsigned long previousMillisDHT = 0;    // Tijdstip van laatste uitlezing DHT sensor


//// 1-WIRE DS18B20 SENSOR ////

#include <OneWire.h>
#include <DallasTemperature.h>

static const int oneWireBus = 10;       // 1-wire pin
OneWire oneWire(oneWireBus);            // 1-wire instantie op de oneWireBus pin
DallasTemperature sensors(&oneWire);    // geef de instantie van de oneWire bus door aan de DallasTemperature

float temperatureValue = 0.0;           // variabele om temperatuur in op te slaan (float is getal met komma)
float temperatureMin = 100.0;           // variabele om de laagst gemeten temperatuur in op te slaan
float temperatureMax = 0.0;             // variabele om de hoogst gemeten temperatuur in op te slaan

int interval1Wire = 1000;               // Tijd in milliseconden tussen het uitlezen van de 1-wire sensor
unsigned long previousMillis1Wire = 0;           // Tijdstip van laatste uitlezing 1-wire sensor


//// ALGEMEEN ////

unsigned long currentMillis = 0;        // Variabele voor het aantal milliseconden sinds de Arduino is gestart

void setup() {
  Serial.begin(9600);                   // stel de seriële monitor in

  // LCD
  lcd.init();                           // initialiseer het LCD scherm
  lcd.backlight();                      // zet de backlight aan
  lcd.createChar(0, humidityIcon);      // definieer een symbool in geheugen positie 0
  lcd.createChar(1, degreeSymbol);      // definieer een symbool in geheugen positie 1
  lcd.createChar(2, temperatureIcon);   // definieer een symbool in geheugen positie 2
  lcd.createChar(3, maxIcon);           // definieer een symbool in geheugen positie 3
  lcd.createChar(4, minIcon);           // definieer een symbool in geheugen positie 4

  // DHT sensor
  dht.begin();                          // start het DHT sensor uitlezen

}

// Functie voor het uitlezen van de 1-wire DS18B20 sensor
void read1WireTemp() {

  // Als het tijdsverschil met de vorige uitlezing groter is dan interval1Wire
  if(currentMillis - previousMillis1Wire > interval1Wire) {

    // vraag de temperatuur aan de 1-wire sensor
    sensors.requestTemperatures();

    // neem de temperatuur van de eerste 1-wire sensor
    float readTemperatureValue = sensors.getTempCByIndex(0);

    // Controleer of de gelezen waarde anders is dan de vorige waarde
    if(temperatureValue != readTemperatureValue) {
      temperatureValue = readTemperatureValue;
      redrawLCD = true;
    }
 
    // Controleer of de gelezen waarde groter is dan de opgeslagen max. temperatuur
    if (temperatureValue > temperatureMax) {
        temperatureMax = temperatureValue;
        redrawLCD = true;
    }

    // Controleer of de gelezen waarde groter is dan de opgeslagen min. temperatuur
    if (temperatureValue < temperatureMin) {
        temperatureMin = temperatureValue;
        redrawLCD = true;
    }

    // sla het huidige tijdstip op in previousMillis1Wire
    previousMillis1Wire = currentMillis;

  }

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

    lcd.setCursor(0, 1);         // zet de cursor op positie 1, regel 2
    lcd.write(2);                // teken het thermometer icoon van geheugenpositie 2
    lcd.print(" ");
    lcd.print(temperatureValue);
    lcd.write(1);                // teken het graden icoon van geheugenpositie 1
    lcd.print("C ");

    lcd.setCursor(10, 0);        // zet de cursor op positie 11, regel 1
    lcd.write(2);                // teken het thermometer icoon van geheugenpositie 2
    lcd.write(3);
    lcd.print(" ");
    lcd.print(temperatureMax);
    lcd.write(1);                // teken het graden icoon van geheugenpositie 1
    lcd.print("C ");

    lcd.setCursor(10, 1);        // zet de cursor op positie 11, regel 2
    lcd.write(2);                // teken het thermometer icoon van geheugenpositie 2
    lcd.write(4);
    lcd.print(" ");
    lcd.print(temperatureMin);
    lcd.write(1);                // teken het graden icoon van geheugenpositie 1
    lcd.print("C ");
  
    redrawLCD = false;           // set redrawLCD weer op 'false'

  }

}


void loop() {

  currentMillis = millis();    // sla de huidige tijd op
  readDHT();                   // lees de DHT sensor uit
  read1WireTemp();             // lees de 1-wire temperatuur sensor uit
  updateLCD();                 // werk indien nodig het LCD scherm bij
  
}
