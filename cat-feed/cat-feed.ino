#include <Servo.h> 

Servo servo1;         // servo /объект сервопривод 
 
int servoPIN = 6; 	  //Servo pin /контакт сервопривода
int MOSFET_pin = 3;   // пин мосфета / реле

//feeding delay, ms 12h*60min*60sec*1000 
// время между открытиями в мс = 12ч*60мин*60сек*1000
unsigned long period_time = (long)3*60*60*1000; //2*60*60*1000
// переменная таймера, максимально большой целочисленный тип (он же uint32_t)
unsigned long my_timer;

int Y0=0; // close angle /угол сервы в закрытом положении
int Y1=25; // close angle /угол сервы в открытом положении
int t=100; // delay for shake mode /приостановка в промежуточных положениях при открытии, мс (режим тряски)
 
void setup() {
  pinMode(MOSFET_pin, OUTPUT); // пин реле как выход
  
  my_timer = millis();   // "сбросить" таймер
  Serial.begin(9600); //test последовательный порт для отображения данных

  Serial.println("setup"); 

  Serial.println("open MOSFET");  
  digitalWrite(MOSFET_pin, 1);    // подаём сигнал на пин мосфета
  delay(1000);

  Open () ;//open function/ функция открытия крышки
  delay(1000);

  digitalWrite(MOSFET_pin, 0);    // подаём сигнал на пин мосфета
  Serial.println("close MOSFET");  
  delay(1000);  
   
}
 
void loop() {
  if ((long)millis() - my_timer > period_time) {
      my_timer = millis();   // "сбросить" таймер
    
      // набор функций, который хотим выполнить один раз за период
 
	  Serial.println("open MOSFET");  
	  digitalWrite(MOSFET_pin, 1);    // подаём сигнал на пин мосфета
	  delay(1000);

	  Open () ;//open function/ функция открытия крышки
	  delay(1000);

	  digitalWrite(MOSFET_pin, 0);    // подаём сигнал на пин мосфета
	  Serial.println("close MOSFET");  
	  delay(1000);  
   
  }

} //loop

////////////////////////////////////////////////////////////////////

// open function //функция открытия крышки
void Open() {
  Serial.println("function start");  

  servo1.attach(servoPIN); // подкючаем сервопривод
  int pos; // position/ положение сервы
    
// open//цикл плавного открытия
  for(pos = Y0; pos < Y1; pos += 5) // шаг=угол
  {
    servo1.write(pos);  // передвинься на следующую позицию
    delay(40);               // небольшой перерыв чтобы он успел передвинуться
  }
     delay(110);               // небольшой перерыв 

//---- 
  
  
//---- close- shake mode / цикл зкрытия с потрясыванием
  for(pos = Y1; pos >= Y0; pos -= 8) //шаг=угол
  {                               
    servo1.write(pos);  // передвинься на следующую позицию
    delay(t);               // небольшой перерыв чтобы он успел передвинуться
    servo1.write(pos-5);  // чуть призакроем
    delay(t);
    servo1.write(pos);  // передвинься на следующую позицию
    delay(t);  
  }

//----  additional shaking /цикл дополнительной тряски в конце
  for(int i = 0; i <= 3; i += 1) //
  {
    servo1.write(14);     // передвинься на следующую позицию
    delay(400);
    servo1.write(Y0);     // повернись до уровня закрытия
        delay(200);
  }  
 //----  
 
    delay(1500); // пауза перед расслаблением

    servo1.detach(); //отключаем сервопривод
    Serial.println("function stop");  

}
