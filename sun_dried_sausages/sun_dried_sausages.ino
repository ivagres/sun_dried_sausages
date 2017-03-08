#include <Keypad.h>
#include <SPI.h>
#include <TimerOne.h>
#include "LedControl.h"
#include "RTClib.h"

// Основная описательная часть
//
//
//

// ==========================================================================================================================

// Блок констант по программам
// Всего 4 программы:
//  - 1. Ясное солнце
//  - 2. Облачно
//  - 3. Дождь
//  - 4. Цикличность устанавливается по параметрам приборов включая наиболее подходящую или по датчику влажноси от 80 до 90 %

// Структура для определения диапазона часов, время работы, диапазон 


// Блок PIN
// PIN реле 13
// PIN часы (шина I2C):
// 	- 
//	-
//	-
#define PIN_RELE 11; // Реле

// блок работы с клавиатурой
#define PIN_KYE_1 4
#define PIN_KYE_2 5
#define PIN_KYE_3 6
#define PIN_KYE_4 7


// тут ещё что то сделаем

// Структура для определения диапазона часов, время работы, диапазон 


// Функция вызываемая кажые 0,5 секунды. Проверяем работоспособность алгоритмов в ней, за одно и мегаем светодиодом.


// Глобальные переменные
RTC_DS1307 rtc;
const int DelayPWM = 1000000 / 2;
int NumberProgramm = 0;


void timerIsr(){
  // тут изменить можно глобальные переменные и все что нам нужно на работу вне основной программе  
  
}

void setup() {
  // Установка pin для реле срабатывания
  pinMode(PIN_RELE,OUTPUT);

  // Установка инициализиции часов
  if (! rtc.begin()) {
    while (100);
  }

  if (! rtc.isrunning()) {
    //RTC не запустился значит ошибка, работа не возможна
  }

// Установка PWM
  Timer1.initialize(DelayPWM); // set a timer of length 1000000 microseconds (or 1 sec - or 1Hz => the led will blink 5 times, 5 cycles of on-and-off, per second)
  Timer1.attachInterrupt( timerIsr ); // attach the service routine here

// Установка клавиатуры
  pinMode(PIN_KYE_1,INPUT_PULLUP);
  pinMode(PIN_KYE_2,INPUT_PULLUP);
  pinMode(PIN_KYE_3,INPUT_PULLUP);
  pinMode(PIN_KYE_4,INPUT_PULLUP);

}

void loop() {
  // Блок статических переменных для управления временем
  static DateTime iTime;
  static int TimeProgramOld = 0;
  // Получаем время с часов 
  iTime = rtc.now();

  // обработка нажатии на клавиатуру и меняем прогрумму
  
  // Проверяем какой у нас сейчас актуальный алгоритм
  // И по типу алгоритма запускаем функцию для реализации
  switch(NumberProgramm){
    case 1:
      // вызов функции программы 1
      break;
    default:
      // ни чего не делаем, пока не сменим программу
      break;
  }

  
}
