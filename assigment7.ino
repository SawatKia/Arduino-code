#include "LedControl.h"
#include "FontLEDClock.h"
#include<EEPROM.h>


#define Speaker 3
#define BT1 7//reset,confirm 
#define BT2 6//setting
#define BT3 5
#define DEBUG_MODE
//#define SET_TIME


LedControl lc = LedControl(11, 13, 10, 4);              // DIN,CLK,CS,Number of LED Module

uint8_t intensity = 8;
uint8_t hour , minute , second ; //Variable Clock mode
uint8_t day = 11, month = 8, year = 21;
uint8_t Alarm_hour = 0 , Alarm_minute = 0 , Alarm_second = 0;
int Selected = 0;
int mode = 0;
/*
   mode0==normal clock
   mode1==adjust time
   mode2==set Alarm
   Selected0==normal clock
   Selected1==set minute
   Selected2==set hour
   Selected3==set date
   Selected4==set month
   Selected5==set year
*/
int lastbt1 = LOW;
int lastbt2 = LOW;
int BT2state;
int lastbt3 = LOW;
int i, addr;
int devices = lc.getDeviceCount();                    /* find no of LED modules */

bool reset = false;
bool setting = false;
bool ShowClock = true;
bool SetAlarm = false;
bool SetAlarmComplete = false;

void setup()
{

  for (addr = 0; addr < devices; addr++)   /* set up each device         */
  { /* set up each device         */
    lc.shutdown(addr, false);                        /* turn off power saving mode */
    lc.setIntensity(addr, 2);                        /* set intensity=2 of 8       */
    lc.clearDisplay(addr);                           /* clear screen               */
  }                                                     /* set up each device         */

  pinMode(Speaker, OUTPUT); /* speaker setup*/
  pinMode(BT1, INPUT);
  pinMode(BT2, INPUT);
  pinMode(BT3, INPUT);
  noTone(Speaker);
  Serial.begin(9600);
  noInterrupts();           /* disable all interrupts             */
  readMems();
  TCCR1A  = 0;
  TCCR1B  = 0;
  TCNT1   = 3036;           /* preload timer 65536-16MHz/256/1Hz  */
  TCCR1B |= (1 << CS12);    /* 256 prescaler                      */
  TIMSK1 |= (1 << TOIE1);   /* enable timer overflow interrupt    */
  interrupts();             /* enable all interrupts              */
}

void playTone(int frequency, int delay_time) {

  tone(Speaker, frequency);
  delay(delay_time);
  noTone(Speaker);
}
void clear_display()
{
  for (uint8_t address = 0; address < 4; address++)
    lc.clearDisplay(address);
}

void fade_down()
{
  for (uint8_t i = intensity; i > 0; i--) //fade from global intensity to 1
  {
    for (uint8_t address = 0; address < 4; address++)
      lc.setIntensity(address, i);
    delay(30); //change this to change fade down speed
  }
}

void fade_up()
{
  for (uint8_t i = 1; i < intensity; i++) //fade from global intensity to 1
  {
    for (uint8_t address = 0; address < 4; address++)
      lc.setIntensity(address, i);
    delay(30); //change this to change fade down speed
  }
  for (uint8_t address = 0; address < 4; address++)
    lc.setIntensity(address, intensity);  //reset intentsity to global value
}

void plot (uint8_t x, uint8_t y, bool value)
{
  uint8_t address;                                      /* select which matrix depending on the x coord  */
  if (x >= 0  && x <= 7)  {
    address = 3;
  }
  if (x >= 8  && x <= 15) {
    address = 2;
    x = x - 8;
  }
  if (x >= 16 && x <= 23) {
    address = 1;
    x = x - 16;
  }
  if (x >= 24 && x <= 31) {
    address = 0;
    x = x - 24;
  }

  if (value == 1)
    lc.setLed(address, y, x, true);
  else
    lc.setLed(address, y, x, false);
}

void print_tiny_char(uint8_t x, uint8_t y, char c)
{
  uint8_t dots;
  if (c >= 'A' && c <= 'Z' || (c >= 'a' && c <= 'z') )  c &= 0x1F;              /* A-Z maps to 1-26  */
  else if (c >= '0' && c <= '9')                        c  = (c - '0') + 32;
  else if (c == ' ' )                                   c  =  0;                /* space             */
  else if (c == '.' )                                   c  = 27;                /* full stop         */
  else if (c == ':' )                                   c  = 28;                /* colon             */
  else if (c == '\'')                                   c  = 29;                /* single quote mark */
  else if (c == '!' )                                   c  = 30;                /* single quote mark */
  else if (c == '?' )                                   c  = 31;                /* question mark     */

  for (uint8_t col = 0; col < 3; col++)
  {
    dots = pgm_read_byte_near(&mytinyfont[c][col]);
    for (uint8_t row = 0; row < 5; row++)
    {
      if (dots & (16 >> row))     plot(x + col, y + row, 1);
      else                      plot(x + col, y + row, 0);
    }
  }
}
void readMems() {
  hour = EEPROM.read(0);
  minute = EEPROM.read(1);
  second = EEPROM.read(2);
  year = EEPROM.read(3);
  month = EEPROM.read(4);
  day = EEPROM.read(5);
  Alarm_minute = EEPROM.read(6);
  Alarm_hour = EEPROM.read(7);
#ifdef DEBUG_MODE
  Serial.println("---------------------------");
  Serial.println("readmems");
  Serial.println("dd/mm/yy read");
  Serial.print("Day : ");
  Serial.print(day);
  Serial.print(" / ");
  Serial.print(" Month : ");
  Serial.print(month);
  Serial.print(" / ");
  Serial.print(" Year : ");
  Serial.println(year);
  Serial.print("Hour : ");
  Serial.print(hour);
  Serial.print(" minute : ");
  Serial.print(minute);
  Serial.print(" second : ");
  Serial.println(second);
  Serial.print("!!Alarm!! set to ");
  Serial.print(Alarm_hour);
  Serial.print(" : ");
  Serial.println(Alarm_minute);
  Serial.println("---------------------------");
#endif
}

void clocks()
{

  plot(10, 5, 0);
  plot(20, 5, 0);
  print_tiny_char( 2, 1,  hour / 10 + '0'); //print hour
  print_tiny_char( 6, 1,  hour % 10 + '0');
  print_tiny_char(12, 1, minute / 10 + '0'); //print minute
  print_tiny_char(16, 1, minute % 10 + '0');
  print_tiny_char(22, 1, second / 10 + '0'); //print second
  print_tiny_char(26, 1, second % 10 + '0');


  if (second % 30 == 0 ) {
    EEPROM.update(0, hour);
    EEPROM.update(1, minute);
    EEPROM.update(2, second);
#ifdef DEBUG_MODE
    Serial.print("!!Alarm!! set to ");
    Serial.print(Alarm_hour);
    Serial.print(" : ");
    Serial.println(Alarm_minute);
    Serial.println("######################################");
    Serial.println("EEprom update TIME");
    Serial.println("NOW is");
    Serial.print(day);
    Serial.print("/");
    Serial.print(month);
    Serial.print("/");
    Serial.print(year);
    Serial.print("    Hour : ");
    Serial.print(hour);
    Serial.print(" minute : ");
    Serial.print(minute);
    Serial.print(" second : ");
    Serial.println(second);
    Serial.println("######################################");
#endif
    if (Selected == 0 && mode == 0) { //ถ้าไม่ได้อยู่ในการตั้งค่าให้แสดงวันที่/ถ้าตั้งค่าอยู่ไม่แสดงวันที่
      DMY();
      delay(5000);
    }
    delay(500);
  }
  if (hour % 24 == 0 && second % 60 == 0) {
    EEPROM.update(3, year);
    EEPROM.update(4, month);
    EEPROM.update(5, day);
#ifdef DEBUG_MODE
    Serial.println("######################################");
    Serial.println("EEprom DATE update");
    Serial.print(day);
    Serial.print("/");
    Serial.print(month);
    Serial.print("/");
    Serial.println(year);
    Serial.println("######################################");
#endif
  }
  plot(10, 2, 1);
  plot(10, 4, 1);
  plot(20, 2, 1);
  plot(20, 4, 1);
  if (reset == true && mode == 1) {
    plot(10, 2, 1);
    plot(10, 4, 1);
    plot(20, 2, 1);
    plot(20, 4, 1);
    delay(500);
    plot(20, 2, 0);
    plot(20, 4, 0);
    plot(10, 2, 0);
    plot(10, 4, 0);
    delay(500);
  }
  // display second count line
  if (second < 30)
  {
    for (i = 1; i <= second + 1; i++)
      plot(i, 7, 1);
    for (i = 32; i >= second + 2; i--)
      plot(i, 7, 0);
  }
  else
  {
    for (i = 26; i <= 30; i++)
      plot(i, 7, 0);
    /*for (i = 1; i <= 60 - second; i++)
      plot(i, 7, 1);*/
    for (i = 32; i >= second; i--)
      plot(i, 7, 0);
    plot(60 - second, 7, 0);
  }
  if (second == 0) {
    for (int i = 1; i <= 30; i++) {
      plot(i, 7, 0);
    }
  }
}

void DMY() {
  plot(20, 2, 0);
  plot(20, 4, 0);
  plot(10, 2, 0);
  plot(10, 4, 0);
  print_tiny_char( 2, 1,  day / 10 + '0'); //print day
  print_tiny_char( 6, 1,  day % 10 + '0');
  plot(10, 5, 1);
  print_tiny_char(12, 1, month / 10 + '0'); //print month
  print_tiny_char(16, 1, month % 10 + '0');
  plot(20, 5, 1);
  int show_year = year % 1000;
  print_tiny_char(22, 1, show_year / 10 + '0'); //print year
  print_tiny_char(26, 1, show_year % 10 + '0');
}

void checkbutton() {
  int bt1state = digitalRead(BT1);
  int bt2state = digitalRead(BT2);
  int bt3state = digitalRead(BT3);

  if (bt1state == HIGH && lastbt1 == LOW) {//push bt1 to reset or confirm
    if (setting != true)
      mode++;
    if (mode == 0) {
      ShowClock = true;
      SetAlarm = false;
    }
    else if (mode == 2) {
      SetAlarm = true;
      ShowClock = false;
    }
    else if (mode > 2) {
      mode = 0;
      ShowClock = true;
      SetAlarm = false;
    }

    if (Selected == 0)
      Selected = 1;
    else if (Selected > 1) {
      Selected = 0;
      ShowClock = true;
    }
    reset = !reset;
#ifdef DEBUG_MODE
    Serial.print("ShowClock : ");
    Serial.println(ShowClock);
    Serial.print("SetAlarm : ");
    Serial.println(SetAlarm);
    Serial.print("mode : ");
    Serial.println(mode);
    Serial.print("Selected : ");
    Serial.println(Selected);
#endif

    if (setting == true) {                  //หลังจากตั้งเวลา(กดปุ่ม3)แล้วถ้ากดปุ่ม1อีกให้บันทึก
      mode = 0;
      Selected = 0;
      EEPROM.update(0, hour);
      EEPROM.update(1, minute);
      EEPROM.update(2, second);
      EEPROM.update(3, year);
      EEPROM.update(4, month);
      EEPROM.update(5, day);
      playTone(10500, 200);
      playTone(9000, 150);
      playTone(8000, 100);
#ifdef DEBUG_MODE
      Serial.print("mode : ");
      Serial.println(mode);
      Serial.print("Selected : ");
      Serial.println(Selected);
      Serial.print("reset button status : ");
      if (reset == true) {
        Serial.println("true");
      }
      else {
        Serial.println("false");
      }
      Serial.println("time date confirm and //UPDATE to EEPROM//");
      Serial.println("Hour : minute : second ");
      Serial.print(hour);
      Serial.print(" : ");
      Serial.print(minute);
      Serial.print(" : ");
      Serial.println(second);
      Serial.print("dd/mm/yy : ");
      Serial.print(day);
      Serial.print(" / ");
      Serial.print(month);
      Serial.print(" / ");
      Serial.println(year);
#endif
      setting = false;
    }
    if (SetAlarmComplete == true) {
      Selected = 0;
      EEPROM.update(7, Alarm_hour);
      EEPROM.update(6, Alarm_minute);
#ifdef DEBUG_MODE
      Serial.print("!!Alarm!! ");
      Serial.print(Alarm_hour);
      Serial.print(" : ");
      Serial.print(Alarm_minute);
      Serial.println(" UPDATE to EEPROM");
#endif
      SetAlarmComplete = false;
    }
  }
  lastbt1 = bt1state;

  if (bt2state == HIGH && lastbt2 == LOW && reset == true && mode == 1) {
    Selected++;
    if (Selected > 5) {
      Selected = 1;
    }
    Serial.print("Selected : ");
    Serial.println(Selected);
  }

  if (bt2state == HIGH && lastbt2 == LOW && mode == 2) {

    Selected++;
    if (Selected > 2) {
      Selected = 1;
    }
    Serial.print("Selected : ");
    Serial.println(Selected);
  }
  lastbt2 = bt2state;

  if (bt3state == HIGH && reset == true && mode == 1) { //hold to set time u want
    setting = true;
    switch (Selected) {
      case 1 :
#ifdef SET_TIME
        hour = 16;
        minute = 25;
        second = 0;
        day = 13;
        month = 11;
        year = 21;
#else
        minute++;
#endif
        break;
      case 2 :
        hour++;
        break;
      case 3 :
        ShowClock = false;
        day++;
        break;
      case 4 :
        month++;
        break;
      case 5 :
        year++;
        break;
    }
#ifdef DEBUG_MODE
    Serial.print("setting clock button status : ");
    if (setting == true) {
      Serial.println("pressing bt3");
    }
    else
      Serial.println("not press bt3");
#endif
  }
  if (bt3state == HIGH && SetAlarm == true && mode == 2) {
    Serial.println("alarm is setting");
    switch (Selected) {
      case 1 :
        Alarm_minute += 10;
        break;
      case 2 :
        Alarm_hour++;
        break;
    }
    SetAlarmComplete = true;
    if (SetAlarmComplete == true)
      Serial.print("Set alarm complete");

  }
  lastbt3 = bt3state;
}
void AlarmSet() {
  Serial.println("Setting Alarm");
  print_tiny_char( 2, 1,  Alarm_hour / 10 + '0'); //print hour
  print_tiny_char( 6, 1,  Alarm_hour % 10 + '0');
  print_tiny_char(12, 1, Alarm_minute / 10 + '0'); //print minute
  print_tiny_char(16, 1, Alarm_minute % 10 + '0');
  print_tiny_char(22, 1, Alarm_second / 10 + '0'); //print second
  print_tiny_char(26, 1, Alarm_second % 10 + '0');
  delay(350);
  plot(10, 2, 1);
  plot(10, 4, 1);
  plot(20, 2, 1);
  plot(20, 4, 1);
  delay(250);
  plot(10, 2, 0);
  delay(250);
  plot(20, 2, 0);
  delay(250);
  plot(10, 4, 0);
  delay(250);
  plot(20, 4, 0);
  delay(250);
}

void blinkclock() {
  if (mode == 1 || mode == 2) {
    if (Selected == 1 || Selected == 4) {
      for (int r = 1; r <= 5; r++) {
        for (int c = 12; c <= 18; c++) {
          plot(c, r, 0);
        }
      }
      delay(100);
    }
    else if (Selected == 2 || Selected == 3 ) {
      for (int r = 1; r <= 5; r++) {
        for (int c = 2; c <= 8; c++) {
          plot(c, r, 0);
        }
      }
      delay(100);
    }
    else if (Selected == 5) {
      for (int r = 1; r <= 5; r++) {
        for (int c = 22; c <= 28; c++) {
          plot(c, r, 0);
        }
      }
      delay(100);
    }
  }

}
ISR(TIMER1_OVF_vect)        // interrupt service routine
{
  TCNT1 = 3036;             /* preload timern */
  Serial.println(second++);
  if (second > 59) {
    second = 0; minute++;
  }
  if (minute > 59) {
    minute = 0; hour++;
  }
  if (hour > 23) {
    hour = 0; day++;
  }
  if (day > 28) {
    if (month == 2) {
      day = 1;
      month++;
    }
    if (day > 30 && (month == 4 || month == 6 || month == 9 || month == 11)) {
      day = 1;
      month++;
    }
    if (day > 31 && (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12)) {
      day = 1;
      month++;
    }
  }
  if (month > 12) {
    month = 1;
    year++;
  }
  if (Alarm_minute > 59) {
    Alarm_minute = 0;
    Alarm_hour++;
  }
  if (Alarm_hour > 23) {
    Alarm_hour = 0;
  }
}
void sound() {
  if (minute == 0 && second == 0) {//ส่งเสียงทุกๆ1ชั่วโมง
    playTone(7000, 150);
    playTone(8500, 150);
  }
  else
    noTone(Speaker);
  if (Alarm_minute == minute && Alarm_hour == hour && second < 30) {
    playTone(4000, 100);
    delay(200);
    playTone(4000, 350);

  }
}


void loop() {
  if (ShowClock == true) {
    clocks();
  }
  if (Selected >= 3) {
    ShowClock = false;
    DMY();
  }
  else if (Selected <= 3)
    ShowClock = true;

  checkbutton();
  if (SetAlarm == true) {
    ShowClock = false;
    AlarmSet();
  }
  blinkclock();
  sound();
  //delay(100);
}
