#include <Servo.h> 
#include <Wire.h>
#include "RTClib.h"

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

Servo servo1;         //объект сервопривод 
 
const uint8_t servoPIN = 6;     //контакт сервопривода
const uint8_t MOSFET_pin = 3;   // пин мосфета


uint32_t period_timing = 20000; //30*60*1000 // интервал между открытиями в мс = 12ч*60мин*60сек*1000
uint32_t period_time = 1000; //3*60*60*1000 // интервал проверок
uint32_t my_timer; // переменная таймера, максимально большой целочисленный тип (он же uint32_t)

uint32_t my_timing; // Пауза



const uint8_t Y0=0; //угол сервы в закрытом положении
const uint32_t Y1=20; //угол сервы в открытом положении
const uint32_t t=50; //приостановка в промежуточных положениях при открытии, мс (режим тряски)

uint32_t hourNow;

void setup() {
  Serial.begin(9600); //test последовательный порт для отображения данных
  delay(1000); // wait for console opening

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  pinMode(MOSFET_pin, OUTPUT); // пин мосфета как выход

  my_timer = millis();   // "сбросить" таймер
  my_timing = millis();   // "сбросить" таймер
  
  Serial.println("setup"); 
  DateTime now = rtc.now();
    
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println(); 

  Open() ;// Открытие крышки
  delay(100);

}
 
void loop() {
  // Пауза
  

  if (millis() - my_timing > period_time){
    my_timing = millis();   // "сбросить" таймер

    //DateTime now = rtc.now();
    //hourNow = now.hour(), DEC;
    hourNow = 10;
    Serial.println(hourNow);

    if (hourNow > 6 && millis() - my_timer > period_timing) {

	  Serial.println(millis());
      Serial.println(my_timer);

      my_timer = millis();   // "сбросить" таймер
      // набор функций, который хотим выполнить один раз за период
        
      Open() ;// Открытие крышки
      delay(1000);
    }
  }
 

  delay(1000);


} //loop

////////////////////////////////////////////////////////////////////

//Открытие крышки
void Open() {
  Serial.println("function start");  

// открыть мосфет
  Serial.println("open MOSFET");  
  digitalWrite(MOSFET_pin, 1);    // подаём сигнал на пин мосфета
  delay(100);

  servo1.attach(servoPIN); // подкючаем сервопривод
  int32_t pos; // положение сервы

// цикл плавного открытия
  for(pos = Y0; pos < Y1; pos += 5) // шаг=угол
  {
    servo1.write(pos);       // передвинься на следующую позицию
    delay(40);               // небольшой перерыв чтобы он успел передвинуться
  }
     delay(110);             // небольшой перерыв 

//---- 
// цикл зкрытия с потрясыванием
  for(pos = Y1; pos > Y0; pos -= 5) //шаг=угол
  {                               
    servo1.write(pos);  // передвинься на следующую позицию
    delay(t);               // небольшой перерыв чтобы он успел передвинуться
    
    //Serial.println(pos);
    
    servo1.write(pos-5);  // чуть призакроем
    delay(t);
    servo1.write(pos);  // передвинься на следующую позицию
    delay(t);  
  }


//цикл дополнительной тряски в конце
  for(uint32_t i = 0; i <= 3; i += 1) //
  {
    servo1.write(14);     // передвинься на следующую позицию
    delay(400);
    servo1.write(Y0);     // повернись до уровня закрытия
    delay(200);
  }
 //----

  delay(1000); // пауза перед расслаблением

  servo1.detach(); //отключаем сервопривод
  Serial.println("function stop");  

// закрыть мосфет
  digitalWrite(MOSFET_pin, 0);    // подаём сигнал на пин мосфета
  Serial.println("close MOSFET");  
  delay(100);  
}