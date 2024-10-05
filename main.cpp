#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "GyverButton.h"
#include <ThreeWire.h>
#include <RtcDS1302.h>


#define OLED_SCK 13
#define OLED_MOSI 11
#define OLED_CS 10
#define OLED_DC 9
#define OLED_RES 8
#define CLOCK_SDA A4 // DAT/IO
#define CLOCK_SCL A5 // CLK/SCL
#define CLOCK_RST 7  // RST/CE
#define BTN_H_PIN 2	 // кнопка подключена сюда (BTN_PIN --- КНОПКА --- GND)
#define BTN_M_PIN 3	 // кнопка подключена сюда (BTN_PIN --- КНОПКА --- GND)
#define BTN_RESET_PIN 4	 // кнопка подключена сюда (BTN_PIN --- КНОПКА --- GND)

ThreeWire myWire(CLOCK_SDA,CLOCK_SCL,CLOCK_RST);   // Указываем вывода IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

GButton buttonH(BTN_H_PIN);
GButton buttonM(BTN_M_PIN);
GButton buttonReset(BTN_RESET_PIN);

U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R2, OLED_CS, OLED_DC, OLED_RES);


void printLogo();
void printTime(const RtcDateTime& dt);
void printFullTime(const RtcDateTime& dt);
void printDate(const RtcDateTime& dt);
void printSetHour(const RtcDateTime& dt, int count);
void printSetMinute(const RtcDateTime& dt, int count);
void printSetDay(const RtcDateTime& dt, int count);
void printSetMonth(const RtcDateTime& dt, int count);
void printSetYear(const RtcDateTime& dt, int count);

int sHour = 0;
int hourIncrement(int hour);
int hourDecrement(int hour);
int sMinute = 0;
int minuteIncrement(int minute);
int minuteDecrement(int minute);
int sDay = 0;
int dayIncrement(int day);
int dayDecrement(int day);
int sMonth = 0;
int monthIncrement(int month);
int monthDecrement(int month);
int sYear = 0;

enum class CurrentDisplay {Time, Fulltime, SetHour, SetMinute,SetDay,SetMonth,SetYear};
CurrentDisplay Display = CurrentDisplay::Time;
static const unsigned char image_InfraredArrowDown_bits[] U8X8_PROGMEM = {0xff,0x7e,0x3c,0x18};
static const unsigned char image_nissan_laurel_logo_bits[] U8X8_PROGMEM = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xf8,0x3f,0x00,0x00,0x00,0x00,0x00,0xfe,0xff,0x01,0x00,0x00,0x00,0x80,0x1f,0xfe,0x07,0x00,0x00,0x00,0xc0,0x03,0x7f,0x1e,0x00,0x00,0x00,0xf0,0x01,0x3f,0x38,0x00,0x00,0x00,0x78,0x80,0x3f,0x70,0x00,0x00,0x00,0x38,0x80,0x1f,0xe0,0x00,0x00,0x00,0x1c,0xc0,0x0f,0xc0,0x01,0x00,0x00,0x0e,0xc0,0x0f,0x80,0x03,0x00,0x00,0x06,0xe0,0x07,0x00,0x03,0x00,0x00,0x07,0xe0,0x07,0x00,0x03,0x00,0x00,0x03,0xf0,0x03,0x00,0x06,0x00,0x88,0x03,0xf0,0x03,0x00,0x86,0x00,0x84,0x01,0xf8,0x01,0x00,0x0c,0x00,0x88,0x01,0xfc,0x01,0x00,0x0c,0x01,0x8a,0x01,0xfc,0x00,0x00,0x8c,0x00,0xc2,0x00,0xfe,0x00,0x00,0x8c,0x01,0xc2,0x00,0x7e,0x00,0x00,0xcc,0x00,0xd2,0x00,0x7f,0x00,0x00,0x0c,0x03,0xd6,0x00,0x3f,0x00,0x00,0x0c,0x02,0xd2,0x80,0x3f,0x00,0x00,0x8c,0x02,0x92,0xc1,0x1f,0x00,0x00,0x8c,0x00,0x92,0xe1,0x0f,0x00,0x00,0xcc,0x02,0x8a,0xf1,0x0f,0x00,0x00,0x6e,0x01,0x8c,0xfb,0xaf,0xff,0xff,0x07,0x01,0x08,0xff,0xff,0xff,0xff,0x97,0x01,0x44,0xff,0xff,0xff,0xff,0x43,0x01,0x54,0xfe,0xff,0xff,0xff,0x03,0x00,0x88,0x0e,0x00,0x00,0x80,0xa9,0x00,0x48,0x1d,0x00,0x00,0xc0,0x55,0x00,0x50,0x38,0x00,0x00,0xe0,0x22,0x00,0xe0,0x70,0x00,0x00,0x70,0x39,0x00,0xa0,0xe4,0x00,0x00,0x3c,0x00,0x00,0x40,0xc8,0x03,0x00,0xdf,0x12,0x00,0x80,0x28,0x1f,0xc0,0x27,0x09,0x00,0x80,0x91,0xfe,0xff,0x49,0x06,0x00,0x00,0x1e,0xe2,0x3f,0xe2,0x01,0x00,0x00,0x48,0x00,0x00,0x91,0x00,0x00,0x00,0x10,0x09,0x00,0x64,0x00,0x00,0x00,0xc0,0x7d,0xf8,0x1f,0x00,0x00,0x00,0x00,0x02,0x00,0x02,0x00,0x00,0x00,0x00,0x08,0x00,0x01,0x00,0x00,0x00,0x00,0xe0,0x7d,0x00,0x00,0x00};

void setup() {
  u8g2.begin();
  Rtc.Begin();
  //RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__); //раскоментировать при первой прошивке
  //Rtc.SetDateTime(compiled);                              //потом снова закоментировать и прошить
  buttonH.setType(HIGH_PULL);
  buttonM.setType(HIGH_PULL);
  buttonReset.setType(HIGH_PULL);
  buttonH.setDirection(NORM_OPEN);
  buttonM.setDirection(NORM_OPEN);
  buttonReset.setDirection(NORM_OPEN); 
  printLogo();
}

void loop() {
  buttonH.tick();
  buttonM.tick();
  buttonReset.tick();
  RtcDateTime now = Rtc.GetDateTime();
  switch (Display)
  {
    case CurrentDisplay::Time:
      printTime(now);
      if (buttonH.isClick()) { 
        Display = CurrentDisplay::Fulltime;
      }
      if (buttonM.isClick()) {
        printDate(now);
        delay(2000);
      }  
      if (buttonReset.isClick()) {
        sHour = now.Hour();
        Display = CurrentDisplay::SetHour;
      }
    break;

    case CurrentDisplay::Fulltime:
      printFullTime(now);
      if (buttonH.isClick()) { 
        Display = CurrentDisplay::Time;
      }
      if (buttonM.isClick()) {
        printDate(now);
        delay(2000);
      }
      if (buttonReset.isClick()){
        sHour = now.Hour();
        Display = CurrentDisplay::SetHour;
      }
    break;

    case CurrentDisplay::SetHour:
      printSetHour(now, sHour);
      if (buttonH.isClick()) {
        sHour = hourIncrement(sHour);
      }
      if (buttonM.isClick()){
        sHour = hourDecrement(sHour);
      }
      if (buttonReset.isClick()) {
        Display = CurrentDisplay::SetMinute;
        sMinute = now.Minute();
      }
    break;

    case CurrentDisplay::SetMinute:
      printSetMinute(now, sMinute);
      if (buttonH.isClick()) { 
        sMinute = minuteIncrement(sMinute);
      }
      if (buttonM.isClick()) {
        sMinute = minuteDecrement(sMinute);
      }
      if (buttonReset.isClick()) {
        Display = CurrentDisplay::SetDay;
        RtcDateTime update = RtcDateTime(now.Year(), now.Month(), now.Day(), sHour, sMinute, 0);
        Rtc.SetDateTime(update);
        sDay = now.Day();
      }
    break;

    case CurrentDisplay::SetDay:
      printSetDay(now, sDay);
      if (buttonH.isClick()) { 
        sDay = dayIncrement(sDay);
      }
      if (buttonM.isClick()) {
        sDay = dayDecrement(sDay);
      }
      if (buttonReset.isClick()) {
        Display = CurrentDisplay::SetMonth;
        sMonth = now.Month();
      }
    break;

    case CurrentDisplay::SetMonth:
      printSetMonth(now, sMonth);
      if (buttonH.isClick()) { 
        sMonth = monthIncrement(sMonth);
      }
      if (buttonM.isClick()) {
        sMonth = monthDecrement(sMonth);
      }
      if (buttonReset.isClick()) {
        Display = CurrentDisplay::SetYear;
        sYear = now.Year();
      }
    break;

    case CurrentDisplay::SetYear:
      printSetYear(now, sYear);
      if (buttonH.isClick()) { 
        sYear = sYear + 1;
      }
      if (buttonM.isClick()) {
        sYear = sYear -1;
      }
      if (buttonReset.isClick()) {
        Display = CurrentDisplay::Time;
        RtcDateTime update = RtcDateTime(sYear, sMonth, sDay, now.Hour(), now.Minute(), 0);
        Rtc.SetDateTime(update);
      }
    break;
  }

}

void printSetYear(const RtcDateTime& dt, int count)
{
  char datestring[11];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u"),
            dt.Day(),
            dt.Month(),
            count);
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_utopia24_tr);
  u8g2.drawStr(0,50,datestring);
  u8g2.setBitmapMode(1);
  u8g2.drawXBMP(100, 25, 8, 4, image_InfraredArrowDown_bits);
  u8g2.sendBuffer();
}

void printSetMonth(const RtcDateTime& dt, int count)
{
  char datestring[11];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u"),
            dt.Day(),
            count,
            dt.Year());
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_utopia24_tr);
  u8g2.drawStr(0,50,datestring);
  u8g2.setBitmapMode(1);
  u8g2.drawXBMP(48, 25, 8, 4, image_InfraredArrowDown_bits);
  u8g2.sendBuffer();
}

void printSetDay(const RtcDateTime& dt, int count)
{
  char datestring[11];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u"),
            count,
            dt.Month(),
            dt.Year());
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_utopia24_tr);
  u8g2.drawStr(0,50,datestring);
  u8g2.setBitmapMode(1);
  u8g2.drawXBMP(10, 25, 8, 4, image_InfraredArrowDown_bits);
  u8g2.sendBuffer();
}

void printSetMinute(const RtcDateTime& dt, int count)
{
  char datestring[6];

  snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u:%02u"),
            dt.Hour(),
            count);
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_logisoso34_tr);
  u8g2.drawStr(10,62,datestring);
  u8g2.setBitmapMode(1);
  u8g2.drawXBMP(88, 20, 8, 4, image_InfraredArrowDown_bits);
  u8g2.sendBuffer();
}

void printSetHour(const RtcDateTime& dt, int count)
{
  char datestring[6];

   snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u:%02u"),
            count,
            dt.Minute());
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_logisoso34_tr);
  u8g2.drawStr(10,62,datestring);
  u8g2.setBitmapMode(1);
  u8g2.drawXBMP(28, 20, 8, 4, image_InfraredArrowDown_bits);
  u8g2.sendBuffer();
}

void printTime(const RtcDateTime& dt)
{
  char datestring[6];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u:%02u"),
            dt.Hour(),
            dt.Minute());
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_logisoso38_tr);
  u8g2.drawStr(6,62,datestring);
  u8g2.sendBuffer();
}

void printFullTime(const RtcDateTime& dt)
{
  char datestring[9];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u:%02u:%02u"),
            dt.Hour(),
            dt.Minute(),
            dt.Second());
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_logisoso26_tr);
  u8g2.drawStr(0,55,datestring);
  u8g2.sendBuffer();
}

void printDate(const RtcDateTime& dt)
{
    char datestring[11];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u"),
            dt.Day(),
            dt.Month(),
            dt.Year());
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_utopia24_tr);
  u8g2.drawStr(0,50,datestring);
  u8g2.sendBuffer();
}

void printLogo() {
  u8g2.clearBuffer();
  u8g2.setBitmapMode(1);
  u8g2.drawXBMP(37, 20, 50, 44, image_nissan_laurel_logo_bits);
  u8g2.sendBuffer();
  delay(4000);
}

int hourIncrement(int hour) {
  if (hour >= 24) {
    return 0;
  } else {
    return hour+1;
  }
}

int hourDecrement(int hour) {
  if (hour <= 0) {
    return 24;
  } else {
    return hour-1;
  }
}

int minuteIncrement(int minute) {
  if (minute >= 60) {
    return 0;
  } else {
    return minute+1;
  }
}

int minuteDecrement(int minute) {
  if (minute <= 0) {
    return 60;
  } else {
    return minute-1;
  }
}

int dayIncrement(int day) {
  if (day >= 31) {
    return 0;
  } else {
    return day+1;
  }
}

int dayDecrement(int day) {
  if (day <= 0) {
    return 31;
  } else {
    return day-1;
  }
}

int monthIncrement(int month) {
  if (month >= 12) {
    return 0;
  } else {
    return month+1;
  }
}

int monthDecrement(int month) {
  if (month <= 0) {
    return 12;
  } else {
    return month-1;
  }
}