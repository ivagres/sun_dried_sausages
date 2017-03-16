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
#define PIN_RELE 11 // Реле

// блок работы с клавиатурой
#define PIN_KYE_1 4
#define PIN_KYE_2 5
#define PIN_KYE_3 6
#define PIN_KYE_4 7

// Блок констант

// Интервалы
const char interval_type_h = 'H';
const char interval_type_m = 'M';
const char interval_type_s = 'S';

// тут ещё что то сделаем

// Структура для определения диапазона часов, время работы, диапазон 
typedef struct wa_clock{
  int zhour;
  int zminute;
  int zsecond;
};

typedef struct wa_works{
  // Диапазон времени от и до часы минуты
  wa_clock  range_clock_low;
  wa_clock  range_clock_high;
  int       interval_wait;
  char      interval_wait_type; 
  int       interval_work;
  char      interval_work_type;
};

// Глобальные переменные
RTC_DS1307 rtc;
const int DelayPWM = 1000000 / 2;
int NumberProgramm = 0;

DateTime gTime;

// Определяем для каждой программы массив для работы (в будующем попробуем настроить её на работу по настройке, а пока так жестко)
int gCountProgramRange[3]={5,3,3};

wa_works lt_works_prog1[5];
wa_works lt_works_prog2[3];
wa_works lt_works_prog3[3];

void init_valume_programm(){
  // Для программы 1
  // ============================================
  lt_works_prog1[0].range_clock_low.zhour   = 7;
  lt_works_prog1[0].range_clock_low.zminute = 0;
  lt_works_prog1[0].range_clock_low.zsecond = 0;

  lt_works_prog1[0].range_clock_high.zhour   = 9;
  lt_works_prog1[0].range_clock_high.zminute = 0;
  lt_works_prog1[0].range_clock_high.zsecond = 0;
  
  lt_works_prog1[0].interval_wait       = 10;
  lt_works_prog1[0].interval_wait_type  = interval_type_m;
  lt_works_prog1[0].interval_work       = 5;
  lt_works_prog1[0].interval_work_type  = interval_type_s;

  // ============================================
  lt_works_prog1[1].range_clock_low.zhour   = 9;
  lt_works_prog1[1].range_clock_low.zminute = 0;
  lt_works_prog1[1].range_clock_low.zsecond = 0;

  lt_works_prog1[1].range_clock_high.zhour   = 11;
  lt_works_prog1[1].range_clock_high.zminute = 0;
  lt_works_prog1[1].range_clock_high.zsecond = 0;
  
  lt_works_prog1[1].interval_wait       = 7;
  lt_works_prog1[1].interval_wait_type  = interval_type_m;
  lt_works_prog1[1].interval_work       = 5;
  lt_works_prog1[1].interval_work_type  = interval_type_s;

  // ============================================
  lt_works_prog1[2].range_clock_low.zhour   = 11;
  lt_works_prog1[2].range_clock_low.zminute = 0;
  lt_works_prog1[2].range_clock_low.zsecond = 0;

  lt_works_prog1[2].range_clock_high.zhour   = 15;
  lt_works_prog1[2].range_clock_high.zminute = 0;
  lt_works_prog1[2].range_clock_high.zsecond = 0;
  
  lt_works_prog1[2].interval_wait       = 5;
  lt_works_prog1[2].interval_wait_type  = interval_type_m;
  lt_works_prog1[2].interval_work       = 5;
  lt_works_prog1[2].interval_work_type  = interval_type_s;

  // ============================================
  lt_works_prog1[3].range_clock_low.zhour   = 15;
  lt_works_prog1[3].range_clock_low.zminute = 0;
  lt_works_prog1[3].range_clock_low.zsecond = 0;

  lt_works_prog1[3].range_clock_high.zhour   = 17;
  lt_works_prog1[3].range_clock_high.zminute = 0;
  lt_works_prog1[3].range_clock_high.zsecond = 0;
  
  lt_works_prog1[3].interval_wait       = 7;
  lt_works_prog1[3].interval_wait_type  = interval_type_m;
  lt_works_prog1[3].interval_work       = 5;
  lt_works_prog1[3].interval_work_type  = interval_type_s;

  // ============================================
  lt_works_prog1[4].range_clock_low.zhour   = 17;
  lt_works_prog1[4].range_clock_low.zminute = 0;
  lt_works_prog1[4].range_clock_low.zsecond = 0;

  lt_works_prog1[4].range_clock_high.zhour   = 20;
  lt_works_prog1[4].range_clock_high.zminute = 0;
  lt_works_prog1[4].range_clock_high.zsecond = 0;
  
  lt_works_prog1[4].interval_wait       = 10;
  lt_works_prog1[4].interval_wait_type  = interval_type_m;
  lt_works_prog1[4].interval_work       = 5;
  lt_works_prog1[4].interval_work_type  = interval_type_s;

  // +++++++++++++++++++++++++++++++++++++++++++
  // Для программы 2
  // ============================================
  lt_works_prog2[0].range_clock_low.zhour   = 7;
  lt_works_prog2[0].range_clock_low.zminute = 0;
  lt_works_prog2[0].range_clock_low.zsecond = 0;

  lt_works_prog2[0].range_clock_high.zhour   = 12;
  lt_works_prog2[0].range_clock_high.zminute = 0;
  lt_works_prog2[0].range_clock_high.zsecond = 0;
  
  lt_works_prog2[0].interval_wait       = 15;
  lt_works_prog2[0].interval_wait_type  = interval_type_m;
  lt_works_prog2[0].interval_work       = 5;
  lt_works_prog2[0].interval_work_type  = interval_type_s;

  // ============================================
  lt_works_prog2[1].range_clock_low.zhour   = 12;
  lt_works_prog2[1].range_clock_low.zminute = 0;
  lt_works_prog2[1].range_clock_low.zsecond = 0;

  lt_works_prog2[1].range_clock_high.zhour   = 17;
  lt_works_prog2[1].range_clock_high.zminute = 0;
  lt_works_prog2[1].range_clock_high.zsecond = 0;
  
  lt_works_prog2[1].interval_wait       = 10;
  lt_works_prog2[1].interval_wait_type  = interval_type_m;
  lt_works_prog2[1].interval_work       = 5;
  lt_works_prog2[1].interval_work_type  = interval_type_s;

  // ============================================
  lt_works_prog2[2].range_clock_low.zhour   = 17;
  lt_works_prog2[2].range_clock_low.zminute = 0;
  lt_works_prog2[2].range_clock_low.zsecond = 0;

  lt_works_prog2[2].range_clock_high.zhour   = 20;
  lt_works_prog2[2].range_clock_high.zminute = 0;
  lt_works_prog2[2].range_clock_high.zsecond = 0;
  
  lt_works_prog2[2].interval_wait       = 15;
  lt_works_prog2[2].interval_wait_type  = interval_type_m;
  lt_works_prog2[2].interval_work       = 5;
  lt_works_prog2[2].interval_work_type  = interval_type_s;

  // +++++++++++++++++++++++++++++++++++++++++++
  // Для программы 3
  // ============================================
  lt_works_prog3[0].range_clock_low.zhour   = 7;
  lt_works_prog3[0].range_clock_low.zminute = 0;
  lt_works_prog3[0].range_clock_low.zsecond = 0;

  lt_works_prog3[0].range_clock_high.zhour   = 12;
  lt_works_prog3[0].range_clock_high.zminute = 0;
  lt_works_prog3[0].range_clock_high.zsecond = 0;
  
  lt_works_prog3[0].interval_wait       = 20;
  lt_works_prog3[0].interval_wait_type  = interval_type_m;
  lt_works_prog3[0].interval_work       = 5;
  lt_works_prog3[0].interval_work_type  = interval_type_s;

  // ============================================
  lt_works_prog3[1].range_clock_low.zhour   = 7;
  lt_works_prog3[1].range_clock_low.zminute = 0;
  lt_works_prog3[1].range_clock_low.zsecond = 0;

  lt_works_prog3[1].range_clock_high.zhour   = 12;
  lt_works_prog3[1].range_clock_high.zminute = 0;
  lt_works_prog3[1].range_clock_high.zsecond = 0;
  
  lt_works_prog3[1].interval_wait       = 15;
  lt_works_prog3[1].interval_wait_type  = interval_type_m;
  lt_works_prog3[1].interval_work       = 5;
  lt_works_prog3[1].interval_work_type  = interval_type_s;

  // ============================================
  lt_works_prog3[2].range_clock_low.zhour   = 7;
  lt_works_prog3[2].range_clock_low.zminute = 0;
  lt_works_prog3[2].range_clock_low.zsecond = 0;

  lt_works_prog3[2].range_clock_high.zhour   = 12;
  lt_works_prog3[2].range_clock_high.zminute = 0;
  lt_works_prog3[2].range_clock_high.zsecond = 0;
  
  lt_works_prog3[2].interval_wait       = 20;
  lt_works_prog3[2].interval_wait_type  = interval_type_m;
  lt_works_prog3[2].interval_work       = 5;
  lt_works_prog3[2].interval_work_type  = interval_type_s;
  
}

// Функция вызываемая кажые 0,5 секунды. Проверяем работоспособность алгоритмов в ней, за одно и мегаем светодиодом.
void timerIsr(){
  // тут изменить можно глобальные переменные и все что нам нужно на работу вне основной программе  
  
  // поделим время на состовляющие
  int p_hour    = gTime.hour();
  int p_minute  = gTime.minute();
  int p_second  = gTime.second();
  
  // Прочтем номер программы и узнаем что нам делать
  // во первых проверим по номеру программы на дипазон времени
  // во вторых проверим совпадает ли временные интервалы при котором должно все работать, Должна быть переменная которая учитывает интервалы
  int iNumberProgramm = 0;
  iNumberProgramm = NumberProgramm;
  if ( NumberProgramm == 4 ){
      // Получим номер оптимальной программы для выполнения
  }
  

  wa_works it_works_prog[gCountProgramRange[(iNumberProgramm - 1)]];
  
  switch (iNumberProgramm){
    case 1:
      for (int i = 0; i < gCountProgramRange[(iNumberProgramm - 1)]; i++) {
        it_works_prog[i] = lt_works_prog1[i];
      }
      break;
    case 2:
      for (int i = 0; i < gCountProgramRange[(iNumberProgramm - 1)]; i++) {
        it_works_prog[i] = lt_works_prog2[i];
      }
      break;
    case 3:
      for (int i = 0; i < gCountProgramRange[(iNumberProgramm - 1)]; i++) {
        it_works_prog[i] = lt_works_prog3[i];
      }
      break;
    default:
      break;
  }
 

}

void setup() {
  // Установим значения для работы начальный (так сказать заводские :) )
  init_valume_programm();
  
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
  static int TimeProgramOld = 0;
  
  // Статические переменные по номеру нажатой кнопки
  
  // Получаем время с часов 
  gTime = rtc.now();

  // обработка нажатии на клавиатуру и меняем прогрумму
  if ( digitalRead(PIN_KYE_1) == LOW ){
    NumberProgramm = 1;
  }
  if ( digitalRead(PIN_KYE_2) == LOW ){
    NumberProgramm = 2;
  }
  if ( digitalRead(PIN_KYE_3) == LOW ){
    NumberProgramm = 3;
  }
  if ( digitalRead(PIN_KYE_4) == LOW ){
    NumberProgramm = 4;
  }

  // Проверяем какой у нас сейчас актуальный алгоритм
  // И по типу алгоритма запускаем функцию для реализации
  switch(NumberProgramm){
    case 1:
      // вызов функции программы 1
      break;
    case 2:
      // вызов функции программы 2
      break;
    case 3:
      // вызов функции программы 3
      break;
    case 4:
      // вызов функции программы 4
      break;
    default:
      // ни чего не делаем, пока не сменим программу
      break;
  }

  // тут выводим на дисплей информацию по текущему состоянию
  
  
  
}
