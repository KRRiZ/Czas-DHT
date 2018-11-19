/* Zegar + czujnik wilgotności i temperatury

    czasWypisz - wypisanie wartości zegarowych na wyświetlaczy LCD
    czas - zmiana wartości zegarowych o 1 sekunde i sprawdzenie przejścia godziny i minuty
    temperatura - sprawdzenie i wypisanie wartości temperatury i wilgotności na LCD
    loop - odświeżenie timer'a oraz sprawdzenie przycisków obsługi zegara
    setup - inicjalizacja programu - określenie pinów przycisków,
    stworzenie i wypisanie stałych wartości na LCD itp.

    Podłączenie:
     - przyciski - jedna strona do +5V, druga do: dodanie minuty - A1, dodanie godziny - A0, reset sekund - A2
     - LCD - RS - 12pin, E - 11pin, D4 - 5pin, D5 - 4pin, D6 - 3pin, D7 - 2pin,
             DO MASY - R/W, VSS, K
             DO +5V - VDD, A(przez rezystor 10k)
             POTENCJOMETR - skrajne do +5V i GND, środek do VO
     - DHT - patrząc na siatkę - 1 do +5V, 2 - 6pin, 3 do niczego, 4 do GND

    Problemy:
    - jakość czujnika DHT11 - brak wartości dziesiętnych i setnych,
    - przyciski niewciśnięte dają wartość analogową ~700-800, zmieniono z digitalRead na analogRead oraz zastosowano warunek (analogRead >= 1020) jako aktywacja przycisku (wartość max = 1023),
*/

//Biblioteki
#include <LiquidCrystal.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "Timer.h"

//Ustawienia DHT
#define DHTPIN            6
#define DHTTYPE           DHT11
DHT_Unified dht(DHTPIN, DHTTYPE);

//Zmienne
//CZAS
int sekundy = 0;
int minuty = 0;
int godziny = 0;
//Piny przycisków
int plusMinuta = A1;
int plusGodzina = A0;
int resetSekunda = A2;
//Dla przycisków
int czasCzekania = 300;
int stanPrzycisku = 0;

//Dane - wyświeltacz
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//Stworzenie timera
Timer t;

//Specjalne znaki
byte stopnie[8] = {
  0b00010,
  0b00101,
  0b00010,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};





void setup() {
//Określenie pinów dla przycisków
  pinMode (plusMinuta, INPUT);
  pinMode (plusGodzina, INPUT);
  pinMode (resetSekunda, INPUT);
  pinMode (13, OUTPUT);
  digitalWrite(13,HIGH);

//Inicjalizacja LCD, DHT
  lcd.begin(16, 2);
  dht.begin();

//Stworzenie nowych znaków do LCD
  lcd.createChar(1, stopnie);

//Sensor DHT
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);

//Wypisanie stałych znaków na LCD
  lcd.setCursor(5, 1);
  lcd.write(1);
  lcd.print("C");
  lcd.setCursor(15, 1);
  lcd.print("%");

//Timery dla zmiany zegara o 1sek i sprawdzenie temperatury
  t.every(1000, czas);
  t.every(1000, temperatura);
}





void loop() {
  
//Odświeżenie timera
  t.update();

//Obsługa przycisków
  stanPrzycisku = analogRead(plusMinuta);
  if (stanPrzycisku >= 1020) {
    minuty++;
    czas();
    delay(czasCzekania);
  }
  stanPrzycisku = analogRead(plusGodzina);
  if (stanPrzycisku >= 1020) {
    godziny++;
    czas();
    delay(czasCzekania);
  }
  stanPrzycisku = analogRead(resetSekunda);
  if (stanPrzycisku >= 1020) {
    sekundy = 0;
    czas();
    delay(czasCzekania);
  }
}





//Zczytanie i wyświetlenie danych z czujnika
void temperatura() {
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    lcd.clear();
    lcd.print("DHT ERROR");
  }else {
    lcd.setCursor(0, 1);
    lcd.print(event.temperature);
  }
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    lcd.clear();
    lcd.print("DHT ERROR");
  } else {
    lcd.setCursor(10, 1);
    lcd.print(event.relative_humidity);
  }
}





//Zmiana czasu
void czas() {
  sekundy++;
  czasWypisz();
}




//Sprawdzenie wartości i wyświetlenie czasu na LCD
void czasWypisz() {
    delay(1);
  if (sekundy == 60) {
    sekundy = 0;
    minuty++;
  }
  delay(1);
  if (minuty == 60) {
    minuty = 0;
    godziny++;
  }
  delay(1);
  if (godziny == 24) {
    sekundy = 0;
    minuty = 0;
    godziny = 0;
  }
  lcd.setCursor(4, 0);
  if (godziny <= 9) {
    lcd.print("0");
    lcd.print(godziny);
  } else {
    lcd.print(godziny);
  }
  lcd.print(":");
  if (minuty <= 9) {
    lcd.print("0");
    lcd.print(minuty);
  } else {
    lcd.print(minuty);
  }
  lcd.print(":");
  if (sekundy <= 9) {
    lcd.print("0");
    lcd.print(sekundy);
  } else {
    lcd.print(sekundy);
  }
}
