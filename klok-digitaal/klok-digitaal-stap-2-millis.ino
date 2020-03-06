/*
   Bas on Tech - I2C SSD1306 OLED 128x64 DIGITALE KLOK
   Dit project is onderdeel van de lessen op https://arduino-lessen.nl

   (c) Copyright 2020 - Bas van Dijk / Bas on Tech
   Deze code en inhoud van de lessen mag zonder schriftelijke toestemming
   niet voor commerciele doeleinden worden gebruikt

   YouTube:    https://www.youtube.com/c/BasOnTechNL
   Facebook:   https://www.facebook.com/BasOnTechChannel
   Instagram:  https://www.instagram.com/BasOnTech
   Twitter:    https://twitter.com/BasOnTech

------------------------------------------------------------------------------   

   128x64 SSD1306 OLED

   PIN AANSLUITINGEN:

   VCC    5V
   GND    GND
   SCL    A5
   SDA    A4

*/

#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

// Variabelen om de tijd in op te slaan
byte hours = 0;
byte minutes = 0;
byte seconds = 0;

// Constanten voor de pins van de knoppen 
const int PIN_BUTTON_HOURS = 3;
const int PIN_BUTTON_MINUTES = 2;

// Variabelen voor de staat van de knoppen
// We gebruiken de interne pull-up weerstanden via INPUT_PULLUP, dus
// ingedrukt is LOW en niet ingedrukt HIGH
int buttonHoursState = HIGH;
int buttonMinutesState = HIGH;

// Char array voor de tijd die op het display wordt weergegeven
char timeString[9];

// Variabelen om de tijd in op te slaan
unsigned long currentMillis = 0;

// Int is voldoende voor de verstreken tijd op te slaan
int elapsedTimeUpdateMillis = 0; 
unsigned long lastTimeUpdateMillis = 0;

// De complete lijst voor alle schermen vind je op: https://github.com/olikraus/u8g2/wiki/u8g2setupcpp
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

void setup(void) {

  // Serial.begin(9600);

  // Stel de pins van de knoppen in met de interne PULLUP weerstand
  pinMode(PIN_BUTTON_HOURS, INPUT_PULLUP);
  pinMode(PIN_BUTTON_MINUTES, INPUT_PULLUP);

  u8g2.setFont(u8g2_font_logisoso28_tf);
  u8g2.begin();
}

void loop(void) {

  // millis() zelf kost 1,812 micro seconde dat is 0.001812 milli seconden
  // https://arduino.stackexchange.com/questions/113/is-it-possible-to-find-the-time-taken-by-millis
  currentMillis = millis();

  elapsedTimeUpdateMillis = currentMillis - lastTimeUpdateMillis;

  // Kijk of de uren knop is ingedrukt
  buttonHoursState = digitalRead(PIN_BUTTON_HOURS);
  if (buttonHoursState == LOW) {
    hours++;
  }

  // Kijk of de minuten knop is ingedrukt
  buttonMinutesState = digitalRead(PIN_BUTTON_MINUTES);
  if (buttonMinutesState == LOW) {
    minutes++;
  }

  // Kijk of er een minuut is verstreken
  if (seconds > 59) {
    seconds = 0;
    minutes++;
  }

  // Kijk of er een uur is verstreken
  if (minutes > 59) {
    minutes = 0;
    hours++; 
  }

  // Kijk of er een dag is verstreken
  if (hours > 23) {
    hours = 0;
  }


  // Kijk of er 1000ms dus 1 seconde is verlopen
  if (elapsedTimeUpdateMillis > 1000) {
    seconds++;

    // Het kan zijn dat er al meer dan 1000ms zijn verlopen. Als voorbeeld nemen we 1200ms. 
    // Dan zijn er dus al 200ms verlopen van de volgende seconde. Deze moeten we in 
    // mindering brengen op de "last time". Zo wordt de volgende seconde 200ms eerder bijgewerkt. 
    // Dit voorkomt dat de klok heel snel gaat "driften".
    lastTimeUpdateMillis = currentMillis - (elapsedTimeUpdateMillis - 1000);
  }

  u8g2.firstPage();

  do {

    // Gevonden op https://forum.arduino.cc/index.php?topic=371117.0
    // sprintf_P gebruikt het Program Memory in plaats van RAM, meer info op http://gammon.com.au/progmem
    // Hier zorgen we dat alle minuten en secondes een 0 erbij krijgen als ze kleiner zijn dan 10 dus 01, 02, 03 etc.
    sprintf_P(timeString, PSTR("%2d:%02d:%02d"), hours, minutes, seconds);

    // Teken de timeString
    u8g2.drawStr(0, 45, timeString);
    
  } while (u8g2.nextPage());

}
