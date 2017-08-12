#include <Wire.h>
#include <Keypad.h>
#include <SPI.h>
#include <TimerOne.h>
#include "LedControl.h"
#include "RTClib.h"
#include <DHT.h>
#include <Streaming.h>

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
// PIN часы (шина I2C):
// 	- A4(SDA)
//	- A5(SCL)
//	-

// PIN Реле
#define PIN_RELE 8 // Реле

// блок работы с клавиатурой
#define PIN_KYE_1 4
#define PIN_KYE_2 5
#define PIN_KYE_3 6
#define PIN_KYE_4 7

// PIN DHT22
#define DHTPIN 13         // Датчик тепературы и влажности
#define DHTTYPE DHT22     // тип датчика температуры и влажности

// PIN LED
#define LED_PIN_DIN 10
#define LED_PIN_CLK 11
#define LED_PIN_CS  12

// PIN датчика освещения
#define PINSOLT A3

// Блок констант

// Интервалы
const char interval_type_0 = 'N';
const char interval_type_h = 'H';
const char interval_type_m = 'M';
const char interval_type_s = 'S';

const String range_type_000  = "NUL";
const String range_type_hhh  = "HHH";
const String range_type_hmm  = "HMM";
const String range_type_hms  = "HMS";


// тут ещё что то сделаем

// Структура для определения диапазона часов, время работы, диапазон
typedef struct wa_clock {
  int zhour;
  int zminute;
  int zsecond;
};

typedef struct wa_works {
  // Диапазон времени от и до часы минуты
  wa_clock  range_clock_low;
  wa_clock  range_clock_high;
  String    range_type;
  int       interval_wait;
  char      interval_wait_type;
  int       interval_work;
  char      interval_work_type;
};

typedef struct wa_work_programm {
  bool  is_work;
  int   work;
  char  work_type;
  int   wait;
  char  wait_type;
  bool  is_rele;
};

// Глобальные переменные
RTC_DS1307 rtc;
const int TCNT1_PRELOAD = 57536;
const int factor   = 1;
const unsigned long DelayPWM = 1000000 / factor;
int NumberProgramm = 0;
bool g_is_error    = false;
int  g_oldMinute   = 0;
int  g_oldSecond   = 0;
bool g_is_houre    = true;

// Глобальная переменная определяющая текущую работу системы
wa_work_programm g_ls_work_programm;

DateTime gTime;

DHT gDHT(DHTPIN, DHTTYPE); // Установка сесора температцуры

LedControl gLC  = LedControl(LED_PIN_DIN, LED_PIN_CLK, LED_PIN_CS, 8);

// Определяем для каждой программы массив для работы (в будующем попробуем настроить её на работу по настройке, а пока так жестко)
int gCountProgramRange[3] = {5, 3, 3};

wa_works lt_works_prog1[5];
wa_works lt_works_prog2[3];
wa_works lt_works_prog3[3];

void init_valume_programm() {
  // Для программы 1
  // ============================================
  lt_works_prog1[0].range_clock_low.zhour   = 7;
  lt_works_prog1[0].range_clock_low.zminute = 0;
  lt_works_prog1[0].range_clock_low.zsecond = 0;

  lt_works_prog1[0].range_clock_high.zhour   = 9;
  lt_works_prog1[0].range_clock_high.zminute = 0;
  lt_works_prog1[0].range_clock_high.zsecond = 0;

  lt_works_prog1[0].range_type = range_type_hhh;

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

  lt_works_prog1[1].range_type = range_type_hhh;

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

  lt_works_prog1[2].range_type = range_type_hhh;

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

  lt_works_prog1[3].range_type = range_type_hhh;

  lt_works_prog1[3].interval_wait       = 7;
  lt_works_prog1[3].interval_wait_type  = interval_type_m;
  lt_works_prog1[3].interval_work       = 5;
  lt_works_prog1[3].interval_work_type  = interval_type_s;

  // ============================================
  lt_works_prog1[4].range_clock_low.zhour   = 17;
  lt_works_prog1[4].range_clock_low.zminute = 0;
  lt_works_prog1[4].range_clock_low.zsecond = 0;

  lt_works_prog1[4].range_clock_high.zhour   = 19;
  lt_works_prog1[4].range_clock_high.zminute = 59;
  lt_works_prog1[4].range_clock_high.zsecond = 0;

  lt_works_prog1[4].range_type = range_type_hmm;

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

  lt_works_prog2[0].range_type = range_type_hhh;

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

  lt_works_prog2[1].range_type = range_type_hhh;

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

  lt_works_prog2[2].range_type = range_type_hhh;

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

  lt_works_prog3[0].range_type = range_type_hhh;

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

  lt_works_prog3[1].range_type = range_type_hhh;

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

  lt_works_prog3[2].range_type = range_type_hhh;

  lt_works_prog3[2].interval_wait       = 20;
  lt_works_prog3[2].interval_wait_type  = interval_type_m;
  lt_works_prog3[2].interval_work       = 5;
  lt_works_prog3[2].interval_work_type  = interval_type_s;

}


/**
   Раскладывает целое число в массив цифр

   Функция factor_digits извлекает цифры числа и размещает в заданном
   массиве в обратной последовательности, в соответствии с нумерацией
   разрядов, то есть элемент массива с индексом нуль получит значение
   нулевого разряда (крайней правой цифры) и т.д. В случае, если при
   разложении достигнуто заданное третьим параметром максимальное
   количество цифр, функция прекращает работу, возвращая нуль, в
   противном случае возвращает количество цифр в разложении.

   @param num    Число для разложения
   @param digits Массив для размещения результата разложения
   @param limit  Максимально допустимое количество цифр
   @returns      количество полученных цифр в разложении,
                 0, если превышено предельное количество цифр
*/
int factor_digits(long num, int *digits, int limit)
{
  int digits_count = 0;
  // Отдельно обрабатываем ситуацию, когда число изначально равно нулю
  if (num == 0) {
    digits[0] = 0;
    return 1;
  }

  // Последовательно извлекаем цифры числа, пока они есть
  //&& limit
  while (num > 0 && limit) {
    // Копируем очередную цифру в массив
    digits[digits_count++] = num % 10;
    // Переходим к следующей цифре
    num /= 10;
    // Уменьшаем допустимое количество оставшихся цифр
    limit--;
  }
  return num ? 0 : digits_count;
}


void print_message(char *type_message) {
  if (type_message == "GE") {
    gLC.setChar(0, 3, 'E', false);
    gLC.setChar(0, 2, '-', false);
    gLC.setChar(0, 1, '0', false);
  }

}

void print_time(bool is_print = false) {
  // поделим время на состовляющие
  int p_hour    = gTime.hour();
  int p_minute  = gTime.minute();
  int p_second  = gTime.second();

  static char _status = 'H';

  if ( (g_oldMinute != p_minute) || (is_print) ) {

    g_oldMinute = p_minute;

    // Очистим дисплей
    //gLC.clearDisplay(0);
    // Номер программы
    gLC.setChar(0, 3, 'P', false);
    gLC.setChar(0, 2, '-', false);

    gLC.setDigit(0, 1, NumberProgramm , false);

    if (g_ls_work_programm.is_work) {
      _status = 'L';
    } else {
      _status = 'H';
    }

    gLC.setChar(0, 0, _status , false);

    if (g_is_houre) {
      g_is_houre = !g_is_houre;
      
      // Выводим состояние и время
      long _hour = (int)p_hour;     // Разложим для примера число 123456789
      int factored_number_hour[8];  // Массив для хранения результата
      int factored_number_len_hour; // Количество цифр в числе

      // Выполняем разложение
      factored_number_len_hour = factor_digits(_hour, factored_number_hour, 8);
      if    (p_hour < 10) {
        gLC.setDigit(0, 7, 0, false);
      }
      for (int i = 0; i <= factored_number_len_hour - 1; i++) {
        gLC.setDigit(0, i + 6, factored_number_hour[i], false);
      }
      long _minute = (int)p_minute;     // Разложим для примера число 123456789
      int factored_number_minute[8];  // Массив для хранения результата
      int factored_number_len_minute; // Количество цифр в числе

      // Выполняем разложение
      factored_number_len_minute = factor_digits(_minute, factored_number_minute, 8);
      if    (p_minute < 10) {
        gLC.setDigit(0, 5, 0, false);
      }
      for (int i = 0; i <= factored_number_len_minute - 1; i++) {
        gLC.setDigit(0, i + 4, factored_number_minute[i], false);
      }
    } else {
      g_is_houre = !g_is_houre;
      
      float _t_temp = gDHT.readTemperature();
      float _h_temp = gDHT.readHumidity();

      //Выводим влажность
      long _h = (int)_h_temp;     // Разложим для примера число 123456789
      int factored_number_h[8];  // Массив для хранения результата
      int factored_number_len_h; // Количество цифр в числе

      // Выполняем разложение
      factored_number_len_h = factor_digits(_h, factored_number_h, 8);
      if    (_h_temp < 10) {
        gLC.setDigit(0, 7, 0, false);
      }
      for (int i = 0; i <= factored_number_len_h - 1; i++) {
        gLC.setDigit(0, i + 6, factored_number_h[i], false);
      }
      gLC.setChar(0, 5, ' ', false);
      gLC.setChar(0, 4, 'H' , true);
    }
  }

  if ( (g_oldSecond != p_second ) && (!g_is_houre) ) {
    g_oldSecond = p_second;
    // Разделитель на часах
    static bool led_event = false;
    led_event = !led_event;
    gLC.setLed(0, 6, 0, led_event);
  }

}

// Функция вызываемая кажые 0,5 секунды. Проверяем работоспособность алгоритмов в ней, за одно и мегаем светодиодом.
void timerIsr(void) {
  noInterrupts();
  // тут изменить можно глобальные переменные и все что нам нужно на работу вне основной программе

  // поделим время на состовляющие
  int p_hour    = gTime.hour();
  int p_minute  = gTime.minute();
  int p_second  = gTime.second();

  int foto_solt = 0;

  float _t  = 0.00;
  float _h   = 0.00;

  // Прочтем номер программы и узнаем что нам делать
  // во первых проверим по номеру программы на дипазон времени
  // во вторых проверим совпадает ли временные интервалы при котором должно все работать, Должна быть переменная которая учитывает интервалы
  int iNumberProgramm = 0;
  iNumberProgramm = NumberProgramm;
  if ( NumberProgramm == 4 ) {
    // Получим номер оптимальной программы для выполнения
    _t = gDHT.readTemperature();
    _h = gDHT.readHumidity();

    foto_solt = analogRead(PINSOLT);
    // Тут алгоритм оптимальной программы, переписываем значение переменной iNumberProgramm;

  }

  wa_works it_works_prog[gCountProgramRange[(iNumberProgramm - 1)]];

  switch (iNumberProgramm) {
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

  // Тут будем бегать по циклу и проверять что у нас есть
  // Нужны переменные по таймеру! Путь это будет gTimeWork - это структура со свойствами

  // Вспомогательные переменные
  wa_works  ls_works;

  bool is_range = false;
  
  is_range = false;
  
  // Пробежимся по циклу и посмотрим в каком диапазоне мы сейчас, и если у нас он есть то смотрим по стотянию что бы должны делать будем
  for ( int i = 0; i < gCountProgramRange[(iNumberProgramm - 1)]; i++ ) {
    ls_works = it_works_prog[i];
    // Проверка на интервалы, в каком мы сейчас интервале
    if ( ls_works.range_type == range_type_hhh ) {
      if ( ( p_hour >= ls_works.range_clock_low.zhour ) && ( p_hour <= ls_works.range_clock_high.zhour ) ) {
        is_range = true;
      }
    }

    if ( ls_works.range_type == range_type_hmm ) {
      if ( ( p_hour >= ls_works.range_clock_low.zhour ) && ( p_hour <= ls_works.range_clock_high.zhour ) ) {
        if ( ( p_minute >= ls_works.range_clock_low.zminute ) && ( p_minute <= ls_works.range_clock_high.zminute ) ) {
          is_range = true;
        }
      }
    }

    if ( ls_works.range_type == range_type_hms ) {
      if ( ( p_hour >= ls_works.range_clock_low.zhour ) && ( p_hour <= ls_works.range_clock_high.zhour )) {
        if ( ( p_minute >= ls_works.range_clock_low.zminute ) && ( p_minute <= ls_works.range_clock_high.zminute ) ) {
          if ( ( p_second >= ls_works.range_clock_low.zsecond ) && ( p_second <= ls_works.range_clock_high.zsecond ) ) {
            is_range = true;
          }
        }
      }
    }

    if (is_range) break;
  }

  
  
  if (is_range) {

    static int kof_work = 1;
    static int kof_wait = 1;

    if ( ls_works.interval_work_type == interval_type_0 ) {
      kof_work = -1;
    }
    if ( ls_works.interval_work_type == interval_type_h ) {
      kof_work =  60 * 60;
    }
    if ( ls_works.interval_work_type == interval_type_m ) {
      kof_work = 60;
    }
    if ( ls_works.interval_work_type == interval_type_s ) {
      kof_work = 1;
    }

    if ( ls_works.interval_wait_type == interval_type_0 ) {
      kof_wait = -1;
    }
    if ( ls_works.interval_wait_type == interval_type_h ) {
      kof_wait =  60 * 60;
    }
    if ( ls_works.interval_wait_type == interval_type_m ) {
      kof_wait = 60;
    }
    if ( ls_works.interval_wait_type == interval_type_s ) {
      kof_wait = 1;
    }

    if (!g_ls_work_programm.is_work) {
      g_ls_work_programm.is_work      = true;
      g_ls_work_programm.work         = ls_works.interval_work * factor * kof_work;
      g_ls_work_programm.work_type    = ls_works.interval_work_type;
      g_ls_work_programm.wait         = ls_works.interval_wait * factor * kof_wait;
      g_ls_work_programm.wait_type    = ls_works.interval_wait_type;
      g_ls_work_programm.is_rele      = false;
      print_time(true);
    }

    if (g_ls_work_programm.is_work) {

      if ( g_ls_work_programm.work > 0 ) {
        Serial.println(g_ls_work_programm.work);
        g_ls_work_programm.work = g_ls_work_programm.work - 1 ;
        if (!g_ls_work_programm.is_rele) {
          g_ls_work_programm.is_rele    = true;
          digitalWrite(PIN_RELE, HIGH);
        }
      } else if (g_ls_work_programm.wait > 0) {
        Serial.println(g_ls_work_programm.wait);
        g_ls_work_programm.wait = g_ls_work_programm.wait - 1;
        if (g_ls_work_programm.is_rele) {
          g_ls_work_programm.is_rele    = false;
          digitalWrite(PIN_RELE, LOW);
        }
      } else {
        g_ls_work_programm.work         = ls_works.interval_work * factor * kof_work;
        g_ls_work_programm.wait         = ls_works.interval_wait * factor * kof_wait;
      }
    }
  } else {
    // Отключим реле и все что с этим связано
    g_ls_work_programm.is_work      = false;
    g_ls_work_programm.work         = 0;
    g_ls_work_programm.work_type    = interval_type_0;
    g_ls_work_programm.wait         = 0;
    g_ls_work_programm.wait_type    = interval_type_0;

    if (g_ls_work_programm.is_rele) {
      print_time(true);
      g_ls_work_programm.is_rele    = false;
      digitalWrite(PIN_RELE, LOW);
    }
  }

  interrupts();
}


void setup() {
  // Установим значения для работы начальный (так сказать заводские :) )
  init_valume_programm();

  // Установка фото датчика
  pinMode(PINSOLT, OUTPUT);

  // Установка pin для реле срабатывания
  pinMode(PIN_RELE, OUTPUT);
  // Установка pin для реле срабатывания
  digitalWrite(PIN_RELE, LOW);

  // Установка инициализиции часов
  if (! rtc.begin()) {
    while (100);
  }

  if (! rtc.isrunning()) {
    //RTC не запустился значит ошибка, работа не возможна
    //rtc.adjust(DateTime(__DATE__, __TIME__));
    delay(100);
    if (! rtc.isrunning()) {
      g_is_error = true;
    }
  }

  if (!g_is_error) {
    // Установка PWM
    noInterrupts();
    Timer1.initialize(DelayPWM); // set a timer of length 1000000 microseconds (or 1 sec - or 1Hz => the led will blink 5 times, 5 cycles of on-and-off, per second)
    //Timer1.pwm(9, 512);
    Timer1.attachInterrupt( timerIsr ); // attach the service routine here
    interrupts();
  }

  // Установка клавиатуры
  pinMode(PIN_KYE_1, INPUT_PULLUP);
  pinMode(PIN_KYE_2, INPUT_PULLUP);
  pinMode(PIN_KYE_3, INPUT_PULLUP);
  pinMode(PIN_KYE_4, INPUT_PULLUP);

  // установка дисплеев
  gLC.shutdown(0, false);  // Включение дисплея
  gLC.setIntensity(0, 15);  // Установка уровня яркости (0 - минимальное, 15 - максимальное)
  gLC.clearDisplay(0);     // Очистить дисплей

  // Инициализация при включении программы, переменная отвечающая за работу реле и остального при об
  g_ls_work_programm.is_work      = false;
  g_ls_work_programm.work         = 0;
  g_ls_work_programm.work_type    = interval_type_0;
  g_ls_work_programm.wait         = 0;
  g_ls_work_programm.wait_type    = interval_type_0;
  g_ls_work_programm.is_rele      = false;

  // Инициализация отладки
  Serial.begin(9600);

  // Если ошибка то выводим сообщение об этом на экран
  if (g_is_error) {
    print_message("GE");
    Serial.println("error is clock");
  }

}

void loop() {
  // глобальная ошибка, то ни чего и не делаем
  if (g_is_error) {
    return;
  }

  // Блок статических переменных для управления временем
  // Статические переменные по номеру нажатой кнопки
  static int NumberProgrammOld = 0;

  // Получаем время с часов
  gTime = rtc.now();

  // обработка нажатии на клавиатуру и меняем прогрумму
  if ( digitalRead(PIN_KYE_1) == LOW ) {
    NumberProgramm = 1;
  }
  if ( digitalRead(PIN_KYE_2) == LOW ) {
    NumberProgramm = 2;
  }
  if ( digitalRead(PIN_KYE_3) == LOW ) {
    NumberProgramm = 3;
  }
  if ( digitalRead(PIN_KYE_4) == LOW ) {
    NumberProgramm = 4;
  }

  if (NumberProgramm == 0) NumberProgramm = 4;

  if (NumberProgrammOld != NumberProgramm ) {
    NumberProgrammOld = NumberProgramm;

    g_ls_work_programm.is_work      = false;
    g_ls_work_programm.work         = 0;
    g_ls_work_programm.work_type    = interval_type_0;
    g_ls_work_programm.wait         = 0;
    g_ls_work_programm.wait_type    = interval_type_0;

    if (g_ls_work_programm.is_rele) {
      g_ls_work_programm.is_rele    = false;
      digitalWrite(PIN_RELE, LOW);
    }
    print_time(true);
  }

  // Проверяем какой у нас сейчас актуальный алгоритм
  // И по типу алгоритма запускаем функцию для реализации
  switch (NumberProgramm) {
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
  print_time();

  // Выведем то что какая у нас программа

}
