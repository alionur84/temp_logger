// this sketch logs to an SD card date, time, rtc temp, dht11 humidity,
//dht11 temperature, dht11 heat index, lux value from temt6000
//respectively in every 5 minutes. change interval value to determine the loging period.
// sd card is connected to MOSI 11, MISO 12, SCK 13, CS 10
// DHT pin is 9, RTC is connected via I2C, temt6000 to A0
// a button is connected to pin2 as an interrupt. controlling the navigation among different screens.
#include "SevenSegmentTM1637.h"
#include "SevenSegmentExtended.h"
#include <SPI.h>
#include <SD.h>
#include <DS3231.h>
#include "DHT.h"

#define DHTPIN 9
#define DHTTYPE DHT11
int check_num = 0; //variabla to check screen to avoid flickering
unsigned long now_time = 0; // time  to use millis instead of delay
unsigned long last_time = 0; // time  to use millis instead of delay
int counter = 0; // variable to navigate among screens
unsigned long interval = 300000; //variable to determine the interval between logs 600000
//declarations of variables to store data read from the sensors
float t;
float h;
float hic;
int sensor = A3;
int sensval;
float luxval;
int t1;
int h1;
int l1;


//pin declarations
int buton = 2;
const byte PIN_CLK = 4;
const byte PIN_DIO = 5;
SevenSegmentExtended      display(PIN_CLK, PIN_DIO);
Time rtc_time;
DHT dht(DHTPIN, DHTTYPE);
DS3231  rtc(SDA, SCL);
File myFile;


void setup()
{

  Serial.begin(9600);
  display.begin();
  display.setBacklight(20);
  pinMode(sensor, INPUT);
  dht.begin();
  rtc.begin();
  Serial.print("Initializing SD card...");
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  pinMode(buton, INPUT_PULLUP);
  attachInterrupt(0, count, LOW);

}
void loop()
{

  if (counter == 0) {
    disp_time();
    now_time = millis();
    unsigned long    a = now_time - last_time;

    if (a >= interval) {

      save_temperature();
      last_time = now_time;
    }
  }

  if (counter == 1) {
    disp_t();
    now_time = millis();
    unsigned long    a = now_time - last_time;

    if (a >= interval) {

      save_temperature();
      last_time = now_time;
    }
  }

  if (counter == 2) {
    disp_h();
    now_time = millis();
    unsigned long    a = now_time - last_time;

    if (a >= interval) {

      save_temperature();
      last_time = now_time;
    }
  }

  if (counter == 3) {
    disp_lux();
    now_time = millis();
    unsigned long    a = now_time - last_time;

    if (a >= interval) {
      
      save_temperature();
      last_time = now_time;
    }
  }

}




void save_temperature() {


  myFile = SD.open("temp.txt", FILE_WRITE);
  //read dht sensor values
  h = dht.readHumidity();
  t = dht.readTemperature();
  hic = dht.computeHeatIndex(t, h, false);

  //luxvalue is calculated
  sensval = analogRead(sensor);
  float volts = sensval * 5.0 / 1024.0;
  float amps = volts / 10000.0;  // across 10,000 Ohms
  float microamps = amps * 1000000;
  luxval = microamps * 2.0;
  h1 = (int) h;
  t1 = (int) t;
  l1 = (int) luxval;
  //read and print to file RTC values, date, time and temp.
  myFile.print(rtc.getDateStr());
  myFile.print(",");
  myFile.print(rtc.getTimeStr());
  myFile.print(",");
  myFile.print(rtc.getTemp());
  myFile.print(",");
  myFile.print(h);
  myFile.print(",");
  myFile.print(t);
  myFile.print(",");
  myFile.print(hic);
  myFile.print(",");
  myFile.println(luxval);
  myFile.close();
  Serial.println(h);
  Serial.println(t);
  Serial.println(hic);
  Serial.println(luxval);
  Serial.println(rtc.getTimeStr());

}

// display temp
void disp_t() {

  if (check_num != 1) {
    display.clear();
    display.setCursor(0, 0);
    display.setColonOn(false);
    display.print("temp");
    delay(1000);
    display.clear();
    check_num = 1;
  }
  if (check_num == 1) {
    display.setCursor(0, 0);
    display.setColonOn(false);
    String t2 = String(t1);
    display.print(t2 + 'c');
    //    display.setCursor(0, 2);
    //    display.print('c');


  }
}


//display humidity
void disp_h() {

  if (check_num != 2) {
    display.clear();
    display.setCursor(0, 0);
    display.setColonOn(false);
    display.print("hum");
    delay(1000);
    display.clear();
    check_num = 2;
  }
  if (check_num == 2) {
    display.setCursor(0, 0);
    display.setColonOn(false);
    String h2 = String(h1);
    display.print(h2 + '%');

  }
}

// display lux
void disp_lux() {
  if (check_num != 3) {
    display.clear();
    display.setCursor(0, 0);
    display.setColonOn(false);
    display.print("lux");
    delay(1000);
    display.clear();
    check_num = 3;
  }
  if (check_num == 3) {
    display.setCursor(0, 0);
    display.setColonOn(false);
    display.print(l1);

  }
}

// display time
void disp_time () {
  display.clear();
  rtc_time = rtc.getTime();
  display.printTime(rtc_time.hour, rtc_time.min, true);
}

//ISR with debounce
void count() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 200) {
    counter = counter + 1;
    counter %= 4;
    Serial.print("interrupted");
    Serial.println(counter);
  }
  last_interrupt_time = interrupt_time;
}
